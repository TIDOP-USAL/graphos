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


#include "FeatureExtractorCommand.h"

#include "graphos/core/features/featio.h"
#include "graphos/core/features/sift.h"
#include "graphos/core/camera/colmap.h"
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

#include <atomic>



#include <QDir>
#include <QTextStream>

/**/
#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/base/image_reader.h>
#include <colmap/feature/extraction.h>
#include <colmap/base/camera_models.h>


bool VerifyCameraParams(const std::string &camera_model,
                        const std::string &params)
{
  if (!colmap::ExistsCameraModelWithName(camera_model)) {
    std::cerr << "ERROR: Camera model does not exist" << std::endl;
    return false;
  }

  const std::vector<double> camera_params = colmap::CSVToVector<double>(params);
  const int camera_model_id = colmap::CameraModelNameToId(camera_model);

  if (camera_params.size() > 0 &&
      !colmap::CameraModelVerifyParams(camera_model_id, camera_params)) {
    std::cerr << "ERROR: Invalid camera parameters" << std::endl;
    return false;
  }
  return true;
}

void ScaleKeypoints(const colmap::Bitmap &bitmap, const colmap::Camera &camera,
                    colmap::FeatureKeypoints *keypoints)
{
  if (static_cast<size_t>(bitmap.Width()) != camera.Width() ||
      static_cast<size_t>(bitmap.Height()) != camera.Height()) {
    const float scale_x = static_cast<float>(camera.Width()) / bitmap.Width();
    const float scale_y = static_cast<float>(camera.Height()) / bitmap.Height();
    for (auto &keypoint : *keypoints) {
      keypoint.Rescale(scale_x, scale_y);
    }
  }
}

/**/

using namespace tl;

namespace graphos
{

/**********************************************************************/

//namespace internal
//{
//std::atomic<bool> featextract_opencv_read = true;
//std::mutex featextract_mutex;
//std::atomic<bool> featextract_done(false);
//
//struct queue_data
//{
//  cv::Mat mat;
//  std::shared_ptr<colmap::Bitmap> bitmap; // Por ahora para procesar con CPU
//  colmap::image_t image_id;
//  double scale;
//  QString image_name;
//};
//
//class ProducerImp
//{
//
//public:
//
//  ProducerImp(const std::vector<Image> *images,
//    const std::map<int, tl::Camera> *cameras,
//    const colmap::Database *database,
//    int maxImageSize,
//    bool useGPU,
//    QueueMPMC<queue_data> *buffer)
//    : mImages(images),
//      mCameras(cameras),
//      mDatabase(database),
//      mMaxImageSize(maxImageSize),
//      bUseGPU(useGPU),
//      mBuffer(buffer)
//  {
//  }
//
//  void operator() (size_t, size_t)
//  {
//    for (const auto& image : *mImages) {
//      producer(image);
//    }
//  }
//
//private:
//
//  void producer(const Image& image)
//  {
//    try {
//
//      tl::Chrono chrono;
//      chrono.run();
//
//      std::string image_path = image.path().toStdString();
//
//      colmap::image_t image_id;
//
//      featextract_mutex.lock();
//      bool exist_image = mDatabase->ExistsImageWithName(image_path);
//      featextract_mutex.unlock();
//      if (!exist_image) {
//
//        colmap::Camera camera_colmap;
//        colmap::camera_t camera_id = static_cast<colmap::camera_t>(image.cameraId());
//
//        featextract_mutex.lock();
//        bool exists_camera = mDatabase->ExistsCamera(camera_id);
//        featextract_mutex.unlock();
//
//        if (!exists_camera) {
//
//          Camera camera;
//          auto it = mCameras->find(camera_id);
//          if (it != mCameras->end()) {
//            camera = mCameras->at(camera_id);
//          } else {
//            throw std::runtime_error(std::string("Camera not found for image: ").append(image_path));
//          }
//
//          QString colmap_camera_type = cameraToColmapType(camera);
//          int camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
//          if (camera_model_id == -1) throw std::runtime_error("Camera model unknow");
//          double focal_length = camera.focal();
//          size_t width = static_cast<size_t>(camera.width());
//          size_t height = static_cast<size_t>(camera.height());
//          if (round(1.2 * std::max(width, height)) == round(focal_length)) {
//            camera_colmap.SetPriorFocalLength(false);
//          } else {
//            camera_colmap.SetPriorFocalLength(true);
//          }
//          camera_colmap.InitializeWithId(camera_model_id, focal_length, width, height);
//          camera_id = mDatabase->WriteCamera(camera_colmap);
//          camera_colmap.SetCameraId(camera_id);
//        }
//
//        colmap::Image image_colmap;
//        image_colmap.SetName(image_path);
//        image_colmap.TvecPrior(0) = image.cameraPose().position().x;
//        image_colmap.TvecPrior(1) = image.cameraPose().position().y;
//        image_colmap.TvecPrior(2) = image.cameraPose().position().z;
//        image_colmap.QvecPrior(0) = image.cameraPose().quaternion().w;
//        image_colmap.QvecPrior(1) = image.cameraPose().quaternion().x;
//        image_colmap.QvecPrior(2) = image.cameraPose().quaternion().y;
//        image_colmap.QvecPrior(3) = image.cameraPose().quaternion().z;
//        image_colmap.SetCameraId(camera_id);
//
//        featextract_mutex.lock();
//        image_id = mDatabase->WriteImage(image_colmap, false);
//        featextract_mutex.unlock();
//
//      } else {
//
//        featextract_mutex.lock();
//        colmap::Image image_colmap = mDatabase->ReadImageWithName(image_path);
//        featextract_mutex.unlock();
//        image_id = image_colmap.ImageId();
//      }
//
//      /* Lectura de imagen */
//
//      cv::Mat mat;
//      std::shared_ptr<colmap::Bitmap> bitmap = std::shared_ptr<colmap::Bitmap>(new colmap::Bitmap());
//      double scale = 1.;
//
//      if (bUseGPU == false) {
//        //TODO: Esto se tiene que cambiar cuando no necesite usar el formato colmap::Bitmap para
//        bitmap->Read(image.path().toStdString(), false);
//
//        cv::Size size(bitmap->Width(), bitmap->Height());
//        double max_dimension = std::max(size.width, size.height);
//        if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
//          scale = max_dimension / mMaxImageSize;
//          size.width /= scale;
//          size.height /= scale;
//          bitmap->Rescale(size.width, size.height);
//        }
//
//      } else {
//        if (featextract_opencv_read) {
//
//          mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE);
//
//          if (mat.empty()) {
//            featextract_opencv_read = false;
//          } else {
//            double max_dimension;
//            if (mat.cols > mat.rows) {
//              max_dimension = mat.cols;
//            } else {
//              max_dimension = mat.rows;
//            }
//
//            if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
//              cv::Size size(mat.cols, mat.rows);
//              scale = max_dimension / mMaxImageSize;
//              size.width /= scale;
//              size.height /= scale;
//#ifdef HAVE_CUDA
//              if (bUseGPU) {
//                cv::cuda::GpuMat gImgIn(mat);
//                cv::cuda::GpuMat gImgResize;
//                cv::cuda::resize(gImgIn, gImgResize, size);
//                gImgResize.download(mat);
//              } else {
//#endif
//                cv::resize(mat, mat, size);
//#ifdef HAVE_CUDA
//              }
//#endif
//            }
//          }
//        }
//
//        if (!featextract_opencv_read) {
//          std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.path().toStdString());
//          imageReader->open();
//          if (imageReader->isOpen()) {
//            int w = imageReader->cols();
//            int h = imageReader->rows();
//            double max_dimension;
//            if (w > h) {
//              max_dimension = w;
//            } else {
//              max_dimension = h;
//            }
//
//
//            if (mMaxImageSize > 0 && mMaxImageSize < max_dimension) {
//              scale = mMaxImageSize / max_dimension;
//              mat = imageReader->read(scale, scale);
//              scale = 1. / scale;
//            } else {
//              mat = imageReader->read();
//            }
//
//            if (imageReader->depth() != 8) {
//#ifdef HAVE_CUDA
//              if (bUseGPU) {
//                cv::cuda::GpuMat gImgIn(mat);
//                cv::cuda::GpuMat gImgOut;
//                cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
//                gImgOut.download(mat);
//              } else {
//#endif
//                cv::normalize(mat, mat, 0., 255., cv::NORM_MINMAX, CV_8U);
//#ifdef HAVE_CUDA
//              }
//#endif
//            }
//
//            if (mat.channels() >= 3) {
//#ifdef HAVE_CUDA
//              cv::cuda::GpuMat gImgIn(mat);
//              cv::cuda::GpuMat gImgGray;
//              cv::cuda::cvtColor(gImgIn, gImgGray, cv::COLOR_BGR2GRAY);
//              gImgGray.download(mat);
//#else
//              cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
//#endif
//            }
//
//            imageReader->close();
//          }
//        }
//      }
//
//      queue_data data;
//      data.mat = mat;
//      data.bitmap = bitmap;
//      data.image_id = image_id;
//      data.scale = scale;
//      data.image_name = image.name();
//
//      mBuffer->push(data); /// Aqui se pasa lo necesario al consumidor
//
//      double time = chrono.stop();
//      msgInfo("Read image %s: [Time: %f seconds]", image_path.c_str(), time);
//
//    } catch (std::exception& e) {
//      msgError(e.what());
//      featextract_done = true;
//    }
//  }
//
//protected:
//
//  const std::vector<Image>* mImages;
//  const std::map<int, Camera>* mCameras;
//  const colmap::Database* mDatabase;
//  int mMaxImageSize;
//  bool bUseGPU;
//  QueueMPMC<queue_data>* mBuffer;
//};
//
//
//
//class ConsumerImp
//{
//
//public:
//
//  ConsumerImp(FeatureExtractor* feat_extractor,
//              ProjectImp* project,
//              //const std::string &databaseFile,
//              colmap::Database* database,
//              QueueMPMC<queue_data>* buffer/*,
//              FeatureExtractorProcess *featureExtractorProcess*/,
//              bool useGPU)
//    : mFeatExtractor(feat_extractor),
//      mProject(project),
//      //mDatabaseFile(databaseFile),
//      mDatabase(database),
//      mBuffer(buffer)/*,
//      mFeatureExtractorProcess(featureExtractorProcess)*/,
//      bUseGPU(useGPU)
//  {
//  }
//
//  void operator() ()
//  {
//    while (!featextract_done) {
//      while (mBuffer->size()) {
//        consumer();
//      }
//    }
//
//    while (mBuffer->size()) {
//      consumer();
//    }
//
//  }
//
//private:
//
//  void consumer()
//  {
//    try {
//
//      tl::Chrono chrono;
//      chrono.run();
//
//      queue_data data;
//      mBuffer->pop(data);
//
//      /* Feature extraction */
//
//      colmap::FeatureKeypoints featureKeypoints;
//      colmap::FeatureDescriptors featureDescriptors;
//
//      if (bUseGPU) {
//        mFeatExtractor->run(data.mat, featureKeypoints, featureDescriptors);
//      } else {
//        mFeatExtractor->run(data.mat, featureKeypoints, featureDescriptors);
//      }
//
//      QByteArray ba = data.image_name.toLocal8Bit();
//      double time = chrono.stop();
//      msgInfo("%i features extracted [Time: %f seconds]", featureKeypoints.size(), time);
//
//      if (data.scale > 1) {
//        for (auto& featureKeypoint : featureKeypoints) {
//          featureKeypoint.Rescale(data.scale, data.scale);
//        }
//      }
//
//      // Escritura en la base de datos
//
//      featextract_mutex.lock();
//      mDatabase->WriteKeypoints(data.image_id, featureKeypoints);
//      mDatabase->WriteDescriptors(data.image_id, featureDescriptors);
//      featextract_mutex.unlock();
//
//      // añade features al proyecto
//      mProject->addFeatures(data.image_name, data.image_name + "@" + mProject->database());
//      //emit featuresExtracted(mImage.name(), mFeatureFile);
//      //emit statusChangedNext();
//      //mFeatureExtractorProcess->featuresExtracted(data.image_name, data.image_name + "@" + mDatabaseFile.c_str());
//      //mFeatureExtractorProcess->statusChangedNext();
//
//    } catch (std::exception& e) {
//      msgError(e.what());
//      featextract_done = true;
//    }
//  }
//
//private:
//
//  FeatureExtractor* mFeatExtractor;
//  ProjectImp* mProject;
//  //std::string mDatabaseFile;
//  colmap::Database* mDatabase;
//  QueueMPMC<queue_data>* mBuffer;
//  //FeatureExtractorProcess *mFeatureExtractorProcess;
//  bool bUseGPU;
//};
//
//}

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
  mSigma = siftProperties.sigma();

  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentIntegerOptional("max_image_size", 's', std::string("Maximum image size (default = ").append(std::to_string(mMaxImageSize)).append(")"), &mMaxImageSize));
  this->push_back(CreateArgumentIntegerOptional("max_features_number", std::string("Maximum number of features to detect (default = ").append(std::to_string(mMaxFeaturesNumber)).append(")"), &mMaxFeaturesNumber));
  this->push_back(CreateArgumentIntegerOptional("SIFT:octave_resolution", std::string("SIFT: Number of layers in each octave (default = ").append(std::to_string(mOctaveResolution)).append(")"), &mOctaveResolution));
  this->push_back(CreateArgumentDoubleOptional("SIFT:contrast_threshold", std::string("SIFT: Contrast Threshold (default = ").append(std::to_string(mContrastThreshold)).append(")"), &mContrastThreshold));
  this->push_back(CreateArgumentDoubleOptional("SIFT:edge_threshold", std::string("SIFT: Threshold used to filter out edge-like features (default = ").append(std::to_string(mEdgeThreshold)).append(")"), &mEdgeThreshold));
  this->push_back(CreateArgumentDoubleOptional("SIFT:sigma", std::string("(default = ").append(std::to_string(mSigma)).append(")"), &mSigma));
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

//  QString file_path;
//  QString project_path;
//
//  try {
//
//    tl::Chrono chrono("Feature extraction finished ");
//    chrono.run();
//
//    if (!mProjectFile.exists()) {
//      msgError("Project doesn't exist");
//      return 1;
//    }
//
//    QString project_file = QString::fromStdWString(mProjectFile.toWString());
//
//    ProjectImp project;
//    project.load(project_file);
//    QString database_path = project.database();
//    std::vector<Image> images = project.images();
//    std::map<int, tl::Camera> cameras = project.cameras();
//
//    colmap::Database database(database_path.toStdString());
//
//    std::shared_ptr<FeatureExtractor> feature_extractor = std::make_shared<SiftCudaDetectorDescriptor>(mMaxFeaturesNumber,
//                                                                                                       mOctaveResolution,
//                                                                                                       mEdgeThreshold,
//                                                                                                       mSigma,
//                                                                                                       mContrastThreshold);
//
//    QueueMPMC<internal::queue_data> buffer(50);
//    internal::ProducerImp producer(&images,
//                                   &cameras,
//                                   &database,
//                                   mMaxImageSize,
//                                   mDisableCuda,
//                                   &buffer);
//
//    internal::ConsumerImp consumer(feature_extractor.get(),
//                                   &project,
//                                   &database,
//                                   &buffer,
//                                   mDisableCuda);
//
//    size_t num_threads = 1;// tl::optimalNumberOfThreads();
//    std::vector<std::thread> producer_threads(num_threads);
//    std::vector<std::thread> consumer_threads(num_threads);
//
//    internal::featextract_done = false;
//
//    size_t size = images.size() / num_threads;
//    for (size_t i = 0; i < num_threads; ++i) {
//      size_t _ini = i * size;
//      size_t _end = _ini + size;
//      if (i == num_threads - 1) _end = images.size();
//
//      producer_threads[i] = std::thread(producer, _ini, _end);
//    }
//
//    for (size_t i = 0; i < num_threads; ++i) {
//      consumer_threads[i] = std::thread(consumer);
//    }
//
//    for (size_t i = 0; i < num_threads; ++i)
//      producer_threads[i].join();
//
//    internal::featextract_done = true;
//
//    for (size_t i = 0; i < num_threads; ++i)
//      consumer_threads[i].join();
//
//    project.save(project_file);
//
//    chrono.stop();
//
//  } catch (const std::exception& e) {
//    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
//    r = true;
//  }

  QString project_file = QString::fromStdWString(mProjectFile.toWString());

  if (QFileInfo(project_file).exists()) {

  } else {
    msgError("The project doesn't exist");
    return 1;
  }

  ProjectImp project;
  project.load(project_file);

  QString database_path = project.database();
  QString project_dir = project.projectFolder();

  QDir dir(project_dir);

  /* Fichero log */

  //QString log_file = project_dir + "/" + project.name() + ".log";

  //tl::Log &log = tl::Log::instance();
  //log.setLogLevel(tl::MessageLevel::msg_verbose);
  //log.setLogFile(log_file.toLatin1());
  //tl::MessageManager::instance().addListener(&log);

  /* Chequeo de soporte CUDA */

    /// TODO: Si el equipo no es compatible con CUDA no he conseguido que funcione la opción de OpenGL
    ///       Se desactiva la GPU por ahora si no es compatible con CUDA
  bool bUseGPU = true;

  /* Chequeos previos para comprobar que todo este bien */

  if (QFileInfo(database_path).exists()) {
    QFile(database_path).remove();
  }

//  /// Se crea la base de datos
  std::string database_path_string = database_path.toStdString();
  colmap::Database database(database_path_string);
//
//  //  std::streambuf *backup = nullptr;
//  //  std::streambuf *psbuf;
//  //  LogStream logStream;
//  //  logStream.open(log_file.toStdString());
//  //  if (logStream.isOpen()){
//  //    backup = std::cout.rdbuf();
//  //    psbuf = logStream.rdbuf();
//  //    std::cout.rdbuf(psbuf);
//  //  }
//
//    /// Se añaden las imagenes y las cámaras a la base de datos
//
//  std::shared_ptr<FeatureExtractor> feat_extractor = std::make_shared<SiftCudaDetectorDescriptor>(mMaxFeaturesNumber,
//                                                                                                  mOctaveResolution,
//                                                                                                  mEdgeThreshold,
//                                                                                                  mSigma,
//                                                                                                  mContrastThreshold);
//  colmap::FeatureKeypoints featureKeypoints;
//  colmap::FeatureDescriptors featureDescriptors;
//  colmap::Camera camera_colmap;
//
//  int camera_id = -1;
//  for (auto it = project.imageBegin(); it != project.imageEnd(); it++) {
//    Image image = (*it);
//    //std::string image_name = (*it)->name().toStdString();
//    std::string image_path = image.path().toStdString();
//    QFileInfo file_info(image.path());
//    std::string image_name = file_info.fileName().toStdString();
//
//    msgInfo("Read image: %s", image_path.c_str());
//
//    if (camera_id != image.cameraId()) {
//      camera_id = image.cameraId();
//      Camera camera = project.findCamera(camera_id);
//
//      QString colmap_camera_type = cameraToColmapType(camera);
//      int camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
//      double focal_length = camera.focal();
//      size_t width = static_cast<size_t>(camera.width());
//      size_t height = static_cast<size_t>(camera.height());
//      if (round(1.2 * std::max(width, height)) == round(focal_length)) {
//        camera_colmap.SetPriorFocalLength(false);
//      } else {
//        camera_colmap.SetPriorFocalLength(true);
//      }
//      camera_colmap.InitializeWithId(camera_model_id, focal_length, width, height);
//      camera_colmap.SetCameraId(database.WriteCamera(camera_colmap));
//    }
//
//
//    colmap::Image image_colmap;
//    image_colmap.SetName(image_name);
//    image_colmap.SetCameraId(camera_colmap.CameraId());
//
//    colmap::image_t image_id = database.WriteImage(image_colmap, false);
//
//    /* Feature extraction */
//
//    cv::Mat mat = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
//
//    double max_dimension;
//    if (mat.cols > mat.rows) {
//      max_dimension = mat.cols;
//    } else {
//      max_dimension = mat.rows;
//    }
//
//    double scale = 1;
//    
//    if (mMaxImageSize < max_dimension) {
//      cv::Size size(mat.cols, mat.rows);
//      scale = mMaxImageSize / max_dimension;
//      size.width *= scale;
//      size.height *= scale;
//      cv::resize(mat, mat, size);
//    }
//
//    feat_extractor->run(mat, featureKeypoints, featureDescriptors);
//
//    if (scale < 1) {
//      for (auto &featureKeypoint : featureKeypoints) {
//        featureKeypoint.Rescale(1/scale, 1/scale);
//      }
//    }
//
//    // Escritura en la base de datos
//    database.WriteKeypoints(image_id, featureKeypoints);
//    database.WriteDescriptors(image_id, featureDescriptors);
//
//  }

  using namespace colmap;

  OptionManager options;
  options.AddDatabaseOptions();
  options.AddImageOptions();
  //options.AddDefaultOption("image_list_path", &image_list_path);
  options.AddExtractionOptions();
  //options.Parse(argc, argv);

  options.image_reader->camera_model = "OPENCV";
  options.image_reader->single_camera = true;

  ImageReaderOptions reader_options = *options.image_reader;
  reader_options.database_path = database_path.toStdString(); // *options.database_path;
  reader_options.image_path = "C:\\datasets\\geomni\\607";// *options.image_path;

  for (auto it = project.imageBegin(); it != project.imageEnd(); it++) {
    Image image = (*it);
    std::string image_path = Path(image.path().toStdString()).fileName().toString();
    reader_options.image_list.push_back(image_path);
  }


  //if (!image_list_path.empty()) {
  //  reader_options.image_list = ReadTextFileLines(image_list_path);
  //  if (reader_options.image_list.empty()) {
  //    return EXIT_SUCCESS;
  //  }
  //}

  if (!ExistsCameraModelWithName(options.image_reader->camera_model)) {
    std::cerr << "ERROR: Camera model does not exist" << std::endl;
  }

  if (!VerifyCameraParams(options.image_reader->camera_model,
                          options.image_reader->camera_params)) {
    return EXIT_FAILURE;
  }

  options.sift_extraction->max_image_size = mMaxImageSize;
  options.sift_extraction->max_num_features = mMaxFeaturesNumber;

  //SiftFeatureExtractor feature_extractor(reader_options,
  //                                       *options.sift_extraction);
  //feature_extractor.Start();
  //feature_extractor.Wait();

  colmap::ImageReader image_reader_(reader_options, &database);
  size_t image_index = 0;
  while (image_reader_.NextIndex() < image_reader_.NumImages()) {

    colmap::internal::ImageData image_data;
    image_data.status =
      image_reader_.Next(&image_data.camera, &image_data.image,
                         &image_data.bitmap, &image_data.mask);

    if (image_data.status != colmap::ImageReader::Status::SUCCESS) {
      image_data.bitmap.Deallocate();
      continue;
    }

    /// Resize
    if (options.sift_extraction->max_image_size > 0) {
      if (static_cast<int>(image_data.bitmap.Width()) > options.sift_extraction->max_image_size ||
          static_cast<int>(image_data.bitmap.Height()) > options.sift_extraction->max_image_size) {
        // Fit the down-sampled version exactly into the max dimensions.
        const double scale =
          static_cast<double>(options.sift_extraction->max_image_size) /
          std::max(image_data.bitmap.Width(), image_data.bitmap.Height());
        const int new_width =
          static_cast<int>(image_data.bitmap.Width() * scale);
        const int new_height =
          static_cast<int>(image_data.bitmap.Height() * scale);

        image_data.bitmap.Rescale(new_width, new_height);
      }
    }

    /// Feature Extractor

    std::unique_ptr<SiftGPU> sift_gpu;
    if (options.sift_extraction->use_gpu) {

      sift_gpu.reset(new SiftGPU);
      if (!CreateSiftGPUExtractor(*options.sift_extraction, sift_gpu.get())) {
        std::cerr << "ERROR: SiftGPU not fully supported." << std::endl;
        //SignalInvalidSetup();
        continue;
      }
    }

    //SignalValidSetup();

    //while (true) {
    //  if (IsStopped()) {
    //    break;
    //  }

      //const auto input_job = input_queue_->Pop();
      //if (input_job.IsValid()) {
      //  auto image_data = input_job.Data();

        //if (image_data.status == colmap::ImageReader::Status::SUCCESS) {
          bool success = false;
          if (options.sift_extraction->estimate_affine_shape ||
              options.sift_extraction->domain_size_pooling) {
            success = ExtractCovariantSiftFeaturesCPU(
              *options.sift_extraction, image_data.bitmap, &image_data.keypoints,
              &image_data.descriptors);
          } else if (options.sift_extraction->use_gpu) {
            success = ExtractSiftFeaturesGPU(
              *options.sift_extraction, image_data.bitmap, sift_gpu.get(),
              &image_data.keypoints, &image_data.descriptors);
          } else {
            success = ExtractSiftFeaturesCPU(*options.sift_extraction, image_data.bitmap,
                                             &image_data.keypoints,
                                             &image_data.descriptors);
          }
          if (success) {
            ScaleKeypoints(image_data.bitmap, image_data.camera,
                           &image_data.keypoints);
            //if (camera_mask_) {
            //  MaskKeypoints(*camera_mask_, &image_data.keypoints,
            //                &image_data.descriptors);
            //}
            //if (image_data.mask.Data()) {
            //  MaskKeypoints(image_data.mask, &image_data.keypoints,
            //                &image_data.descriptors);
            //}
          } else {
            image_data.status = colmap::ImageReader::Status::FAILURE;
          }
        //}

        image_data.bitmap.Deallocate();

        //output_queue_->Push(image_data);
      //} else {
      //  break;
      //}
    //}

        /// Write

        image_index += 1;

        std::cout << StringPrintf("Processed file [%d/%d]", image_index,
                                  reader_options.image_list.size()/*num_images_*/)
          << std::endl;

        std::cout << StringPrintf("  Name:            %s",
                                  image_data.image.Name().c_str())
          << std::endl;

        if (image_data.status == colmap::ImageReader::Status::IMAGE_EXISTS) {
          std::cout << "  SKIP: Features for image already extracted."
            << std::endl;
        } else if (image_data.status == colmap::ImageReader::Status::BITMAP_ERROR) {
          std::cout << "  ERROR: Failed to read image file format." << std::endl;
        } else if (image_data.status ==
                   colmap::ImageReader::Status::CAMERA_SINGLE_DIM_ERROR) {
          std::cout << "  ERROR: Single camera specified, "
            "but images have different dimensions."
            << std::endl;
        } else if (image_data.status ==
                   colmap::ImageReader::Status::CAMERA_EXIST_DIM_ERROR) {
          std::cout << "  ERROR: Image previously processed, but current image "
            "has different dimensions."
            << std::endl;
        } else if (image_data.status == colmap::ImageReader::Status::CAMERA_PARAM_ERROR) {
          std::cout << "  ERROR: Camera has invalid parameters." << std::endl;
        } else if (image_data.status == colmap::ImageReader::Status::FAILURE) {
          std::cout << "  ERROR: Failed to extract features." << std::endl;
        }

        if (image_data.status != colmap::ImageReader::Status::SUCCESS) {
          continue;
        }

        std::cout << StringPrintf("  Dimensions:      %d x %d",
                                  image_data.camera.Width(),
                                  image_data.camera.Height())
          << std::endl;
        std::cout << StringPrintf("  Camera:          #%d - %s",
                                  image_data.camera.CameraId(),
                                  image_data.camera.ModelName().c_str())
          << std::endl;
        std::cout << StringPrintf("  Focal Length:    %.2fpx",
                                  image_data.camera.MeanFocalLength());
        if (image_data.camera.HasPriorFocalLength()) {
          std::cout << " (Prior)" << std::endl;
        } else {
          std::cout << std::endl;
        }
        if (image_data.image.HasTvecPrior()) {
          std::cout << StringPrintf(
            "  GPS:             LAT=%.3f, LON=%.3f, ALT=%.3f",
            image_data.image.TvecPrior(0),
            image_data.image.TvecPrior(1),
            image_data.image.TvecPrior(2))
            << std::endl;
        }
        std::cout << StringPrintf("  Features:        %d",
                                  image_data.keypoints.size())
          << std::endl;

        DatabaseTransaction database_transaction(&database);

        if (image_data.image.ImageId() == kInvalidImageId) {
          image_data.image.SetImageId(database.WriteImage(image_data.image));
        }

        if (!database.ExistsKeypoints(image_data.image.ImageId())) {
          database.WriteKeypoints(image_data.image.ImageId(),
                                    image_data.keypoints);
        }

        if (!database.ExistsDescriptors(image_data.image.ImageId())) {
          database.WriteDescriptors(image_data.image.ImageId(),
                                      image_data.descriptors);
        }

  }

  return r;
}

} // namespace graphos