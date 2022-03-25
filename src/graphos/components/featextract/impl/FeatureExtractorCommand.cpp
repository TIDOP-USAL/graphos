/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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


#include "FeatureExtractorCommand.h"

#include "graphos/core/features/featio.h"
#include "graphos/core/features/sift.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/project.h"

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
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include <atomic>

using namespace tl;

namespace graphos
{

/**********************************************************************/

namespace internal
{
std::atomic<bool> featextract_opencv_read = true;
std::mutex featextract_mutex;
std::atomic<bool> featextract_done(false);

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

  void operator() (size_t, size_t)
  {
    for (const auto &image : *mImages) {

      //if(mFeatureExtractorProcess->isWaitingForFinished()) {
      //  done = true;
      //  break;
      //}

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

      featextract_mutex.lock();
      bool exist_image = mDatabase->ExistsImageWithName(image_path);
      featextract_mutex.unlock();
      if (!exist_image) {

        colmap::Camera camera_colmap;
        colmap::camera_t camera_id = static_cast<colmap::camera_t>(image.cameraId());

        featextract_mutex.lock();
        bool exists_camera = mDatabase->ExistsCamera(camera_id);
        featextract_mutex.unlock();

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

          if(focal_lenght > 0.) {
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
        if(position != tl::Point3D()) {
          image_colmap.TvecPrior(0) = image.cameraPose().position().x;
          image_colmap.TvecPrior(1) = image.cameraPose().position().y;
          image_colmap.TvecPrior(2) = image.cameraPose().position().z;
        }

        tl::math::Quaternion<double> q = image.cameraPose().quaternion();
        if(q != tl::math::Quaternion<double>::zero()) {
          image_colmap.QvecPrior(0) = q.w;
          image_colmap.QvecPrior(1) = q.x;
          image_colmap.QvecPrior(2) = q.y;
          image_colmap.QvecPrior(3) = q.z;
        } else {
          image_colmap.QvecPrior().setConstant(std::numeric_limits<double>::quiet_NaN());
        }

        image_colmap.SetCameraId(camera_id);

        featextract_mutex.lock();
        image_id = mDatabase->WriteImage(image_colmap, false);
        featextract_mutex.unlock();

      } else {

        featextract_mutex.lock();
        colmap::Image image_colmap = mDatabase->ReadImageWithName(image_path);
        featextract_mutex.unlock();
        image_id = image_colmap.ImageId();
      }

      /* Lectura de imagen */

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

    } catch (std::exception& e) {
      msgError(e.what());
      featextract_done = true;
    }
  }

  cv::Mat readImage(const Image &image, double &scale)
  {
    cv::Mat mat;

    if(featextract_opencv_read) {

      mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH);

      //mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_COLOR);
      //cv::Mat color_boost;
      //cv::decolor(mat, mat, color_boost);
      //color_boost.release();

      if(mat.empty()) {
        featextract_opencv_read = false;
      } else {

        cv::Size size(mat.cols, mat.rows);
        double max_dimension = std::max(size.width, size.height);

        if(mMaxImageSize > 0 && mMaxImageSize < max_dimension) {

          scale = max_dimension / mMaxImageSize;
          size.width /= scale;
          size.height /= scale;

          resizeImage(mat, size);
        }
      }
    }

    if(!featextract_opencv_read) {
      std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.path().toStdString());
      imageReader->open();
      if(imageReader->isOpen()) {

        double max_dimension = std::max(imageReader->cols(), imageReader->rows());

        if(mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
          scale = mMaxImageSize / max_dimension;
          mat = imageReader->read(scale, scale);
          scale = 1. / scale;
        } else {
          mat = imageReader->read();
        }

        if(mat.channels() >= 3) {
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
    if(mat.depth() != CV_8U) {
#ifdef HAVE_CUDA
      if(bUseGPU) {
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
    if(bUseGPU) {
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
              bool useGPU,
              tl::Progress *progressBar)
    : mFeatExtractor(feat_extractor),
      mDatabaseFile(databaseFile),
      mDatabase(database),
      mBuffer(buffer),
      mFeatureExtractorProcess(featureExtractorProcess),
      bUseGPU(useGPU),
      mProgressBar(progressBar)
  {
  }

  void operator() ()
  {
    while (!featextract_done) {
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

      //if(mFeatureExtractorProcess->isWaitingForFinished()) {
      //  done = true;
      //  return;
      //}

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


      double time = chrono.stop();
      msgInfo("%i features extracted [Time: %f seconds]", featureKeypoints.size(), time);

      // a�ade features al proyecto
      mFeatureExtractorProcess->featuresExtracted(data.image_name, data.image_name + "@" + mDatabaseFile.c_str());
      if(mProgressBar) (*mProgressBar)();

    } catch (std::exception& e) {
      msgError(e.what());
      featextract_done = true;
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
    if(scale > 1) {
      for(auto &featureKeypoint : featureKeypoints) {
        featureKeypoint.Rescale(scale, scale);
      }
    }
  }

  void writeFeatures(const colmap::image_t &image_id,
                     colmap::FeatureKeypoints &featureKeypoints,
                     colmap::FeatureDescriptors &featureDescriptors)
  {
    std::lock_guard<std::mutex> lck(featextract_mutex);

    mDatabase->WriteKeypoints(image_id, featureKeypoints);
    mDatabase->WriteDescriptors(image_id, featureDescriptors);

  }

private:

  FeatureExtractor *mFeatExtractor;
  std::string mDatabaseFile;
  colmap::Database* mDatabase;
  QueueMPMC<queue_data>* mBuffer;
  FeatureExtractorProcess *mFeatureExtractorProcess;
  bool bUseGPU;
  tl::Progress *mProgressBar;
};

}



/******************************************************************/



FeatureExtractorProcess::FeatureExtractorProcess(const std::vector<Image> &images,
                                                 const std::map<int, Camera> &cameras,
                                                 const QString &database,
                                                 int maxImageSize,
                                                 bool cuda,
                                                 const std::shared_ptr<FeatureExtractor> &featureExtractor)
  : tl::ProcessBase(),
    mImages(images),
    mCameras(cameras),
    mDatabase(database),
    mMaxImageSize(maxImageSize),
    bUseCuda(cuda),
    mFeatureExtractor(featureExtractor)
{
}


void FeatureExtractorProcess::execute(tl::Progress *progressBar)
{

  try {

    tl::Chrono chrono("Feature extraction finished ");
    chrono.run();

    colmap::Database database(mDatabase.toStdString());

    QueueMPMC<internal::queue_data> buffer(50);
    internal::ProducerImp producer(&mImages,
                                   &mCameras,
                                   &database,
                                   mMaxImageSize,
                                   bUseCuda,
                                   &buffer,
                                   this);
    internal::ConsumerImp consumer(mFeatureExtractor.get(),
                                   mDatabase.toStdString(),
                                   &database,
                                   &buffer,
                                   this,
                                   bUseCuda,
                                   progressBar);

    size_t num_threads = 1;
    std::vector<std::thread> producer_threads(num_threads);
    std::vector<std::thread> consumer_threads(num_threads);

    internal::featextract_done = false;

    size_t size = mImages.size() / num_threads;
    for(size_t i = 0; i < num_threads; ++i) {
      size_t _ini = i * size;
      size_t _end = _ini + size;
      if(i == num_threads - 1) _end = mImages.size();

      producer_threads[i] = std::move(std::thread(producer, _ini, _end));
    }

    for(size_t i = 0; i < num_threads; ++i) {
      consumer_threads[i] = std::move(std::thread(consumer));
    }

    for(size_t i = 0; i < num_threads; ++i)
      producer_threads[i].join();

    internal::featextract_done = true;

    for(size_t i = 0; i < num_threads; ++i)
      consumer_threads[i].join();

    size_t keypoints = database.NumKeypoints();
    TL_ASSERT(keypoints > 0, "Keypoints not detected");
    /*else if (isWaitingForFinished()) {
      chrono.reset();
    }*/
    chrono.stop();
  
  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Feature Extractor error");
  }

}

/******************************************************************/  


FeatureExtractorCommand::FeatureExtractorCommand()
  : Command("featextract", "Feature extraction (SIFT)"),
    mProjectFile(""),
    mMaxImageSize(3200),
    mDisableCuda(false)
{
  SiftProperties siftProperties;
  mMaxFeaturesNumber = siftProperties.featuresNumber();
  mOctaveResolution = siftProperties.octaveLayers();
  mContrastThreshold = siftProperties.contrastThreshold();
  mEdgeThreshold = siftProperties.edgeThreshold();

  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentIntegerOptional("max_image_size", 's', std::string("Maximum image size (default = ").append(std::to_string(mMaxImageSize)).append(")"), &mMaxImageSize));
  this->push_back(CreateArgumentIntegerOptional("max_features_number", std::string("Maximum number of features to detect (default = ").append(std::to_string(mMaxFeaturesNumber)).append(")"), &mMaxFeaturesNumber));
  this->push_back(CreateArgumentIntegerOptional("SIFT:octave_resolution", std::string("SIFT: Number of layers in each octave (default = ").append(std::to_string(mOctaveResolution)).append(")"), &mOctaveResolution));
  this->push_back(CreateArgumentDoubleOptional("SIFT:contrast_threshold", std::string("SIFT: Contrast Threshold (default = ").append(std::to_string(mContrastThreshold)).append(")"), &mContrastThreshold));
  this->push_back(CreateArgumentDoubleOptional("SIFT:edge_threshold", std::string("SIFT: Threshold used to filter out edge-like features (default = ").append(std::to_string(mEdgeThreshold)).append(")"), &mEdgeThreshold));
  this->push_back(CreateArgumentBooleanOptional("disable_cuda", "If true disable CUDA (default = false)", &mDisableCuda));

  this->addExample("featextract --p 253/253.xml");

  this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

FeatureExtractorCommand::~FeatureExtractorCommand()
{
}

bool FeatureExtractorCommand::run()
{
  bool r = false;

  QString file_path;
  QString project_path;

  try {

    if (!mProjectFile.exists()) {
      msgError("Project doesn't exist");
      return 1;
    }

    QString project_file = QString::fromStdWString(mProjectFile.toWString());

    ProjectImp project;
    project.load(project_file);
    QString database_path = project.database();
    std::vector<Image> images = project.images();
    std::map<int, Camera> cameras = project.cameras();

    TL_TODO("Colmap no permite borrar la tabla de keypoints ni sobreescribirla asi que por ahora borro la base de datos completa")
    QFile(database_path).remove();
    project.removeFeatures();

    colmap::Database database(database_path.toStdString());

    std::shared_ptr<FeatureExtractor> feature_extractor;

    if(mDisableCuda) {
      feature_extractor = std::make_shared<SiftCPUDetectorDescriptor>(mMaxFeaturesNumber,
                                                                      mOctaveResolution,
                                                                      mEdgeThreshold,
                                                                      mContrastThreshold);
      
    } else {
      feature_extractor = std::make_shared<SiftCudaDetectorDescriptor>(mMaxFeaturesNumber,
                                                                       mOctaveResolution,
                                                                       mEdgeThreshold,
                                                                       mContrastThreshold);
    }

    FeatureExtractorProcess Feature_extractor_process(images,
                                                      cameras,
                                                      database_path,
                                                      mMaxImageSize,
                                                      !mDisableCuda,
                                                      feature_extractor);

    connect(&Feature_extractor_process, &FeatureExtractorProcess::featuresExtracted, 
            [&](const QString &imageName, const QString &featuresFile){
              project.addFeatures(imageName, featuresFile);
            });

    Feature_extractor_process.run();

    project.save(project_file);

  } catch (const std::exception &e) {

    printException(e);
    
    r = true;
  }

  return r;
}

} // namespace graphos