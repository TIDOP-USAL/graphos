/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/process/features/FeatureExtractorProcess.h"

#include "graphos/core/features/featio.h"
#include "graphos/core/features/sift.h"
#include "graphos/core/camera/Colmap.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/img/imgreader.h>
#include <tidop/core/concurrency.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#ifdef HAVE_CUDA
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>
#endif // HAVE_CUDA

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>

#include <QFileInfo>

#include <atomic>

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

          size_t width = static_cast<size_t>(camera.width());
          size_t height = static_cast<size_t>(camera.height());

          double focal_lenght = camera.focal();

          if (focal_lenght > 0.) {
            camera_colmap.SetPriorFocalLength(true);
          } else {
            focal_lenght = 1.2 * std::max(width, height);
            camera_colmap.SetPriorFocalLength(false);
          }

          camera_colmap.InitializeWithId(camera_model_id, focal_lenght, width, height);
          camera_id = mDatabase->WriteCamera(camera_colmap);
          camera_colmap.SetCameraId(camera_id);
        }

        colmap::Image image_colmap;
        image_colmap.SetName(image_path);

        tl::Point3D position = image.cameraPose().position();
        if (position != tl::Point3D()) {
          image_colmap.TvecPrior(0) = image.cameraPose().position().x;
          image_colmap.TvecPrior(1) = image.cameraPose().position().y;
          image_colmap.TvecPrior(2) = image.cameraPose().position().z;
        }

        tl::math::Quaternion<double> q = image.cameraPose().quaternion();
        if (q != tl::math::Quaternion<double>::zero()) {
          image_colmap.QvecPrior(0) = q.w;
          image_colmap.QvecPrior(1) = q.x;
          image_colmap.QvecPrior(2) = q.y;
          image_colmap.QvecPrior(3) = q.z;
        } else {
          image_colmap.QvecPrior().setConstant(std::numeric_limits<double>::quiet_NaN());
        }

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

      TL_TODO("lectura de metadatos con exiftool")

      double scale = 1.;
      cv::Mat mat = readImage(image, scale);

      /* Write queue */

      queue_data data;
      data.mat = mat;
      data.image_id = image_id;
      data.scale = scale;
      data.image_name = image.name();

      mBuffer->push(data);

      double time = chrono.stop();
      msgInfo("Read image %s: [Time: %f seconds]", image_path.c_str(), time);

    } catch (std::exception &e) {
      msgError(e.what());
      done = true;
    }
  }

  cv::Mat readImage(const Image &image, double &scale)
  {
    cv::Mat mat;

    if (bOpenCvRead) {

      mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH);
      
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

          resizeImage(mat, size);
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

        if (mat.channels() >= 3) {
          convertRgbToGray(mat);
        }

        imageReader->close();
      }
    }

    normalizeImage(mat);

    return mat;
  }

  /*!
   * \brief Convert an RGB image to gray
   * \param [in|out] mat Image to convert
   */
  void convertRgbToGray(cv::Mat &mat)
  {
#ifdef HAVE_CUDA
    cv::cuda::GpuMat gImgIn(mat);
    cv::cuda::GpuMat gImgGray;
    cv::cuda::cvtColor(gImgIn, gImgGray, cv::COLOR_BGR2GRAY);
    gImgGray.download(mat);
#else
    cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
#endif
  }

  void normalizeImage(cv::Mat &mat)
  {
    if (mat.depth() != CV_8U) {
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
  }

  void resizeImage(cv::Mat &mat, const cv::Size &size)
  {
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

      featureExtraction(data.mat, featureKeypoints, featureDescriptors);
      resizeFeatures(featureKeypoints, data.scale);
      writeFeatures(data.image_id, featureKeypoints, featureDescriptors);

      // añade features al proyecto
      mFeatureExtractorProcess->featuresExtracted(data.image_name, data.image_name + "@" + mDatabaseFile.c_str());
      mFeatureExtractorProcess->statusChangedNext();

      double time = chrono.stop();
      msgInfo("%i features extracted [Time: %f seconds]", featureKeypoints.size(), time);

    } catch (std::exception &e) {
      msgError(e.what());
      done = true;
    }
  }



  void featureExtraction(const cv::Mat &mat,
                         colmap::FeatureKeypoints &featureKeypoints,
                         colmap::FeatureDescriptors &featureDescriptors)
  {
    mFeatExtractor->run(mat, featureKeypoints, featureDescriptors);
  }

  void resizeFeatures(colmap::FeatureKeypoints &featureKeypoints, 
                      double scale)
  {
    if (scale > 1) {
      for (auto &featureKeypoint : featureKeypoints) {
        featureKeypoint.Rescale(scale, scale);
      }
    }
  }

  void writeFeatures(const colmap::image_t &image_id,
                     colmap::FeatureKeypoints &featureKeypoints, 
                     colmap::FeatureDescriptors &featureDescriptors)
  {
    std::lock_guard<std::mutex> lck(mutex);

    mDatabase->WriteKeypoints(image_id, featureKeypoints);
    mDatabase->WriteDescriptors(image_id, featureDescriptors);

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
    mFeatureExtractor(featureExtractor)
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

      producer_threads[i] = std::move(std::thread(producer, _ini, _end));
    }

    for (size_t i = 0; i < num_threads; ++i) {
      consumer_threads[i] = std::move(std::thread(consumer));
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
