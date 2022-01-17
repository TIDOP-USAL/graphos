#include "FeatureExtractorProcess.h"

#include "graphos/core/features/featio.h"
#include "graphos/core/features/sift.h"
#include "graphos/core/camera/colmap.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/img/imgreader.h>
#include <tidop/core/concurrency.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>

#ifdef HAVE_CUDA
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>
#endif // HAVE_CUDA

#include <QFileInfo>

using namespace tl;

namespace graphos
{

std::atomic<bool> bOpenCvRead = true;
std::mutex mutex;
std::atomic<bool> done(false);

struct queue_data
{
  cv::Mat mat;
  colmap::image_t image_id;
  double scale;
  QString image_name;
};

class ProducerImp
{

public:

  ProducerImp(const std::vector<Image> *images,
              const std::map<int, tl::Camera> *cameras,
              const colmap::Database *database,
              int maxImageSize,
              bool useGPU,
              QueueMPMC<queue_data> *buffer,
              FeatureExtractorProcess *featureExtractorProcess)
    : mImages(images),
      mCameras(cameras),
      mDatabase(database),
      mMaxImageSize(maxImageSize),
      bUseGPU(useGPU),
      mBuffer(buffer),
      mFeatureExtractorProcess(featureExtractorProcess)
  {
  }

  void operator() (size_t , size_t )
  {
    for (const auto &image : *mImages) {
      
      if (mFeatureExtractorProcess->isWaitingForFinished()) {
        done = true;
        break;
      }

      producer(image);
    }
  }

private:

  void producer(const Image &image)
  {
    try {

      tl::Chrono chrono;
      chrono.run();

      std::string image_path = image.path().toStdString();

      colmap::image_t image_id;

      mutex.lock();
      bool exist_image = mDatabase->ExistsImageWithName(image_path);
      mutex.unlock();
      if (!exist_image) {

        colmap::Camera camera_colmap;
        colmap::camera_t camera_id = static_cast<colmap::camera_t>(image.cameraId());

        mutex.lock();
        bool exists_camera = mDatabase->ExistsCamera(camera_id);
        mutex.unlock();

        if (!exists_camera) {

          Camera camera;
          auto it = mCameras->find(camera_id);
          if (it != mCameras->end()) {
            camera = mCameras->at(camera_id);
          } else {
            throw std::runtime_error(std::string("Camera not found for image: ").append(image_path));
          }

          QString colmap_camera_type = cameraToColmapType(camera);
          int camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
          if (camera_model_id == -1) throw std::runtime_error("Camera model unknow");
          //double focal_length = camera.focal();
          size_t width = static_cast<size_t>(camera.width());
          size_t height = static_cast<size_t>(camera.height());
          //if (round(1.2 * std::max(width, height)) == round(focal_length)) {
          //  camera_colmap.SetPriorFocalLength(false);
          //} else {
          //  camera_colmap.SetPriorFocalLength(true);
          //}
          colmap::Bitmap bitmap;
          double focal_lenght = 0.0;
          if (bitmap.Read(image_path, false)) {
            if (bitmap.ExifFocalLength(&focal_lenght)) {
              camera_colmap.SetPriorFocalLength(true);
            } else {
              focal_lenght = 1.2 * std::max(width, height);
              camera_colmap.SetPriorFocalLength(false);
            }
          }

          camera_colmap.InitializeWithId(camera_model_id, focal_lenght, width, height);
          camera_id = mDatabase->WriteCamera(camera_colmap);
          camera_colmap.SetCameraId(camera_id);
        }

        colmap::Image image_colmap;
        image_colmap.SetName(image_path);
        image_colmap.TvecPrior(0) = image.cameraPose().position().x;
        image_colmap.TvecPrior(1) = image.cameraPose().position().y;
        image_colmap.TvecPrior(2) = image.cameraPose().position().z;
        image_colmap.QvecPrior(0) = image.cameraPose().quaternion().w;
        image_colmap.QvecPrior(1) = image.cameraPose().quaternion().x;
        image_colmap.QvecPrior(2) = image.cameraPose().quaternion().y;
        image_colmap.QvecPrior(3) = image.cameraPose().quaternion().z;
        image_colmap.SetCameraId(camera_id);

        mutex.lock();
        image_id = mDatabase->WriteImage(image_colmap, false);
        mutex.unlock();

      } else {

        mutex.lock();
        colmap::Image image_colmap = mDatabase->ReadImageWithName(image_path);
        mutex.unlock();
        image_id = image_colmap.ImageId();
      }

      /* Lectura de imagen */

      cv::Mat mat;
      //std::shared_ptr<colmap::Bitmap> bitmap = std::shared_ptr<colmap::Bitmap>(new colmap::Bitmap());
      double scale = 1.;

      ////TODO: Esto se tiene que cambiar cuando no necesite usar el formato colmap::Bitmap para
      //bitmap->Read(image.path().toStdString(), false);

      //cv::Size size(bitmap->Width(), bitmap->Height());
      //double max_dimension = std::max(size.width, size.height);
      //if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
      //  scale = max_dimension / mMaxImageSize;
      //  size.width /= scale;
      //  size.height /= scale;
      //  bitmap->Rescale(size.width, size.height);
      //}
      //std::vector<uchar> data_uint8 = bitmap->ConvertToRowMajorArray();
      //cv::Mat _bitmap = cv::Mat(size, CV_8U, data_uint8.data());
      //mat = _bitmap.clone();

      if (bOpenCvRead) {

        mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE);
        //mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_COLOR);

        //cv::Mat color_boost;
        //cv::decolor(mat, mat, color_boost);
        //color_boost.release();

        if (mat.empty()) {
          bOpenCvRead = false;
        } else {

          cv::Size size(mat.cols, mat.rows);
          double max_dimension = std::max(size.width, size.height);

          if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
            
            scale = max_dimension / mMaxImageSize;
            size.width /= scale;
            size.height /= scale;
#ifdef HAVE_CUDA
            if (bUseGPU) {
              cv::cuda::GpuMat gImgIn(mat);
              cv::cuda::GpuMat gImgResize;
              cv::cuda::resize(gImgIn, gImgResize, size);
              gImgResize.download(mat);
            } else {
#endif
              cv::resize(mat, mat, size);
#ifdef HAVE_CUDA
            }
#endif
          }
        }
      }

      if (!bOpenCvRead) {
        std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.path().toStdString());
        imageReader->open();
        if (imageReader->isOpen()) {

          double max_dimension = std::max(imageReader->cols(), imageReader->rows());

          if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
            scale = mMaxImageSize / max_dimension;
            mat = imageReader->read(scale, scale);
            scale = 1. / scale;
          } else {
            mat = imageReader->read();
          }

          if (imageReader->depth() != 8) {
#ifdef HAVE_CUDA
            if (bUseGPU) {
              cv::cuda::GpuMat gImgIn(mat);
              cv::cuda::GpuMat gImgOut;
              cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
              gImgOut.download(mat);
            } else {
#endif
              cv::normalize(mat, mat, 0., 255., cv::NORM_MINMAX, CV_8U);
#ifdef HAVE_CUDA
            }
#endif
          }

          if (mat.channels() >= 3) {
#ifdef HAVE_CUDA
            cv::cuda::GpuMat gImgIn(mat);
            cv::cuda::GpuMat gImgGray;
            cv::cuda::cvtColor(gImgIn, gImgGray, cv::COLOR_BGR2GRAY);
            gImgGray.download(mat);
#else
            cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
#endif
          }

          imageReader->close();
        }
      }



      queue_data data;
      data.mat = mat;
      data.image_id = image_id;
      data.scale = scale;
      data.image_name = image.name();

      mBuffer->push(data); /// Aqui se pasa lo necesario al consumidor

      double time = chrono.stop();
      msgInfo("Read image %s: [Time: %f seconds]", image_path.c_str(), time);

    } catch (std::exception &e) {
      msgError(e.what());
      done = true;
    }
  }

protected:

  const std::vector<Image> *mImages;
  const std::map<int, Camera> *mCameras;
  const colmap::Database *mDatabase;
  int mMaxImageSize;
  bool bUseGPU;
  QueueMPMC<queue_data> *mBuffer;
  FeatureExtractorProcess *mFeatureExtractorProcess;
};



class ConsumerImp
{

public:

  ConsumerImp(FeatureExtractor *feat_extractor,
              const std::string &databaseFile,
              colmap::Database *database,
              QueueMPMC<queue_data> *buffer,
              FeatureExtractorProcess *featureExtractorProcess,
              bool useGPU)
    : mFeatExtractor(feat_extractor),
      mDatabaseFile(databaseFile),
      mDatabase(database),
      mBuffer(buffer),
      mFeatureExtractorProcess(featureExtractorProcess),
      bUseGPU(useGPU)
  {
  }

  void operator() ()
  {
    while (!done) {
      while (mBuffer->size()) {
        consumer();
      }
    }

    while (mBuffer->size()) {
      consumer();
    }

  }

private: 

  void consumer()
  {
    try {

      if (mFeatureExtractorProcess->isWaitingForFinished()) {
        done = true;
        return;
      }

      tl::Chrono chrono;
      chrono.run();

      queue_data data;
      mBuffer->pop(data);

      /* Feature extraction */

      colmap::FeatureKeypoints featureKeypoints;
      colmap::FeatureDescriptors featureDescriptors;

      //if (bUseGPU) {
        mFeatExtractor->run(data.mat, featureKeypoints, featureDescriptors);
      //} else {
      //  auto w = data.bitmap->Width();
      //  colmap::FeatureKeypoints featureKeypoints2;
      //  colmap::FeatureDescriptors featureDescriptors2;
      //  mFeatExtractor->run(*data.bitmap, featureKeypoints2, featureDescriptors2);
      //}

      QByteArray ba = data.image_name.toLocal8Bit();
      double time = chrono.stop();
      msgInfo("%i features extracted [Time: %f seconds]", featureKeypoints.size(), time);

      if (data.scale > 1) {
        for (auto &featureKeypoint : featureKeypoints) {
          featureKeypoint.Rescale(data.scale, data.scale);
        }
      }

      // Escritura en la base de datos

      mutex.lock();
      mDatabase->WriteKeypoints(data.image_id, featureKeypoints);
      mDatabase->WriteDescriptors(data.image_id, featureDescriptors);
      mutex.unlock();

      // añade features al proyecto
      //mProject->addFeatures(data.image_name, data.image_name + "@" + mProject->database());
      //emit featuresExtracted(mImage.name(), mFeatureFile);
      //emit statusChangedNext();
      mFeatureExtractorProcess->featuresExtracted(data.image_name, data.image_name + "@" + mDatabaseFile.c_str());
      mFeatureExtractorProcess->statusChangedNext();

    } catch (std::exception &e) {
      msgError(e.what());
      done = true;
    }
  }

private:

  FeatureExtractor *mFeatExtractor;
  std::string mDatabaseFile;
  colmap::Database *mDatabase;
  QueueMPMC<queue_data> *mBuffer;
  FeatureExtractorProcess *mFeatureExtractorProcess;
  bool bUseGPU;
};





FeatureExtractorProcess::FeatureExtractorProcess(const std::vector<Image> &images,
                                                 const std::map<int, tl::Camera> &cameras,
                                                 const QString &database,
                                                 int maxImageSize,
                                                 bool cuda,
                                                 const std::shared_ptr<FeatureExtractor> &featureExtractor)
  : ProcessConcurrent(),
    mImages(images),
    mCameras(cameras),
    mDatabase(database),
    mMaxImageSize(maxImageSize),
    bUseCuda(cuda),
    mFeatureExtractor(featureExtractor)/*,
    bOpenCvRead(true)*/
{
}

void FeatureExtractorProcess::run()
{
  try {

    tl::Chrono chrono("Feature extraction finished ");
    chrono.run();

    colmap::Database database(mDatabase.toStdString());

    QueueMPMC<queue_data> buffer(50);
    ProducerImp producer(&mImages,
                         &mCameras,
                         &database,
                         mMaxImageSize,
                         bUseCuda,
                         &buffer,
                         this);
    ConsumerImp consumer(mFeatureExtractor.get(),
                         mDatabase.toStdString(),
                         &database,
                         &buffer,
                         this,
                         bUseCuda);

    size_t num_threads = 1;// tl::optimalNumberOfThreads();
    std::vector<std::thread> producer_threads(num_threads);
    std::vector<std::thread> consumer_threads(num_threads);

    done = false;

    size_t size = mImages.size() / num_threads;
    for (size_t i = 0; i < num_threads; ++i) {
      size_t _ini = i * size;
      size_t _end = _ini + size;
      if (i == num_threads - 1) _end = mImages.size();

      producer_threads[i] = std::thread(producer, _ini, _end);
    }

    for (size_t i = 0; i < num_threads; ++i) {
      consumer_threads[i] = std::thread(consumer);
    }

    for (size_t i = 0; i < num_threads; ++i)
      producer_threads[i].join();

    done = true;

    for (size_t i = 0; i < num_threads; ++i)
      consumer_threads[i].join();

    size_t keypoints = database.NumKeypoints();
    if (keypoints == 0) {
      emit error(0, "Keypoint Detector/descriptor");
    } else if (isWaitingForFinished()) {
      chrono.reset();
    } else {
      chrono.stop();
    }

  } catch (std::exception &e) {
    msgError(e.what());
  } catch (...) {
    msgError("Keypoint Detector/descriptor unknown exception");
  }

}


} // graphos
