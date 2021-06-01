#include "FeatureExtractorProcess.h"

#include "inspector/core/features/featio.h"
#include "inspector/core/features/sift.h"

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

namespace inspector
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
              const std::map<int, Camera> *cameras,
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

  void operator() (size_t ini, size_t end)
  {

    for (auto &image : *mImages) {

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
            continue;
          }

          QString colmap_camera_type = cameraToColmapType(camera);
          int camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
          if (camera_model_id == -1) throw std::runtime_error("Camera model unknow");
          double focal_length = camera.focal();
          size_t width = static_cast<size_t>(camera.width());
          size_t height = static_cast<size_t>(camera.height());
          if (round(1.2 * std::max(width, height)) == round(focal_length)) {
            camera_colmap.SetPriorFocalLength(false);
          } else {
            camera_colmap.SetPriorFocalLength(true);
          }
          camera_colmap.InitializeWithId(camera_model_id, focal_length, width, height);
          camera_id = mDatabase->WriteCamera(camera_colmap);
          camera_colmap.SetCameraId(camera_id);
        }

        colmap::Image image_colmap;
        image_colmap.SetName(image_path);
        image_colmap.TvecPrior(0) = image.cameraPosition().x();
        image_colmap.TvecPrior(1) = image.cameraPosition().y();
        image_colmap.TvecPrior(2) = image.cameraPosition().z();
        image_colmap.QvecPrior(0) = image.cameraPosition().quaternion().w;
        image_colmap.QvecPrior(1) = image.cameraPosition().quaternion().x;
        image_colmap.QvecPrior(2) = image.cameraPosition().quaternion().y;
        image_colmap.QvecPrior(3) = image.cameraPosition().quaternion().z;
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
      double scale = 1.;

      if (bOpenCvRead) {

        mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE);

        if (mat.empty()) {
          bOpenCvRead = false;
        } else {
          double max_dimension;
          if (mat.cols > mat.rows) {
            max_dimension = mat.cols;
          } else {
            max_dimension = mat.rows;
          }

          if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
            cv::Size size(mat.cols, mat.rows);
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
          int w = imageReader->cols();
          int h = imageReader->rows();
          double max_dimension;
          if (w > h) {
            max_dimension = w;
          } else {
            max_dimension = h;
          }


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
      msgInfo("Read image %s: [Time: %f seconds]",  image_path.c_str(), time);

      if (mFeatureExtractorProcess->isWaitingForFinished()) {
        done = true;
        return;
      }
        
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
              FeatureExtractorProcess *featureExtractorProcess)
    : mFeatExtractor(feat_extractor),
      mDatabaseFile(databaseFile),
      mDatabase(database),
      mBuffer(buffer),
      mFeatureExtractorProcess(featureExtractorProcess)
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

  void consumer()
  {
    tl::Chrono chrono;
    chrono.run();

    queue_data data;
    mBuffer->pop(data);

    /* Feature extraction */

    colmap::FeatureKeypoints featureKeypoints;
    colmap::FeatureDescriptors featureDescriptors;

    mFeatExtractor->run(data.mat, featureKeypoints, featureDescriptors);

    QByteArray ba = data.image_name.toLocal8Bit();
    const char *img_file = ba.data();
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
  }

private:

  FeatureExtractor *mFeatExtractor;
  std::string mDatabaseFile;
  colmap::Database *mDatabase;
  QueueMPMC<queue_data> *mBuffer;
  FeatureExtractorProcess *mFeatureExtractorProcess;
};





FeatureExtractorProcess::FeatureExtractorProcess(const std::vector<Image> &images,
                                                 const std::map<int, Camera> &cameras,
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
    mFeatureExtractor(featureExtractor),
    bOpenCvRead(true)
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
                         this);

    size_t num_threads = 1;// tl::optimalNumberOfThreads();
    std::vector<std::thread> producer_threads(num_threads);
    std::vector<std::thread> consumer_threads(num_threads);

    done = false;

    size_t size = mImages.size() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
      size_t _ini = i * size;
      size_t _end = _ini + size;
      if (i == num_threads - 1) _end = mImages.size();

      producer_threads[i] = std::thread(producer, _ini, _end);
    }

    for (int i = 0; i < num_threads; ++i) {
      consumer_threads[i] = std::thread(consumer);
    }

    for (int i = 0; i < num_threads; ++i)
      producer_threads[i].join();
    done = true;
    for (int i = 0; i < num_threads; ++i)
      consumer_threads[i].join();

    chrono.stop();

  } catch (std::exception &e) {
    msgError(e.what());
  } catch (...) {
    msgError("Keypoint Detector/descriptor unknown exception");
  }

}


} // inspector
