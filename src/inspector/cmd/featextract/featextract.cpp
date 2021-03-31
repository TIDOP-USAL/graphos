#include <inspector/process/features/FeatureExtractorProcess.h>
#include <inspector/core/utils.h>
#include <inspector/core/project.h>
#include <inspector/core/features/sift.h>
#include <inspector/core/features/featio.h>
#include <inspector/core/image.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>
#include <tidop/core/chrono.h>
#include <tidop/img/imgreader.h>
#include <tidop/core/concurrency.h>

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#ifdef HAVE_CUDA
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>
#endif // HAVE_CUDA

#include <QDir>
#include <QTextStream>

#include <cstdlib>
#include <iostream>


using namespace inspector;
using namespace tl;

std::atomic<bool> bOpenCvRead = true;
std::mutex mutex;

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

  ProducerImp(ProjectImp *project,
              colmap::Database *database,
              int maxImageSize,
              bool useGPU,
              QueueMPMC<queue_data> *buffer)
    : mProject(project),
      mDatabase(database),
      mMaxImageSize(maxImageSize),
      bUseGPU(useGPU),
      mBuffer(buffer)
  {
  }
  
  void operator() (size_t ini, size_t end)
  {
    //for (auto it = mProject->imageBegin(); it != mProject->imageEnd(); it++){
    for (auto it = mProject->imageBegin()+ini; it != mProject->imageBegin()+end; it++){
      Image image = (*it);
      //std::string image_name = (*it)->name().toStdString();
      std::string image_path = image.path().toStdString();
      QFileInfo file_info(image.path());
      std::string image_name = file_info.fileName().toStdString();

      msgInfo("Read image: %s", image_path.c_str());
      
      colmap::image_t image_id;

      mutex.lock();
      bool exist_image = mDatabase->ExistsImageWithName(image_name);
      mutex.unlock();
      if (!exist_image){

        colmap::Camera camera_colmap;
        colmap::camera_t camera_id = static_cast<colmap::camera_t>(image.cameraId());

        mutex.lock();
        bool exists_camera = mDatabase->ExistsCamera(image.cameraId());
        mutex.unlock();

        if (!exists_camera) {
          Camera camera = mProject->findCamera(camera_id);
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
        image_colmap.SetName(image_name);
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
        colmap::Image image_colmap = mDatabase->ReadImageWithName(image_name);
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
          if (mat.cols > mat.rows){
            max_dimension = mat.cols;
          } else {
            max_dimension = mat.rows;
          }
      
          if (mMaxImageSize < max_dimension){
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
      
      
          if (mMaxImageSize < max_dimension) {
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
      //std::cout << "image name: " << image.name().toStdString() << "\n";
      //std::cout << "image id: " << image_id << "\n";
      //std::cout << "scale: " << scale << "\n";

      mBuffer->push(data); /// Aqui se pasa lo necesario al consumidor

    }
  }

protected:

  ProjectImp *mProject;
  colmap::Database *mDatabase;
  int mMaxImageSize;
  bool bUseGPU;
  QueueMPMC<queue_data> *mBuffer;

};

std::atomic<bool> done(false);

class ConsumerImp
{

public:

  ConsumerImp(FeatureExtractor *feat_extractor,
              ProjectImp *project,
              colmap::Database *database,
              QueueMPMC<queue_data> *buffer)
    : mFeatExtractor(feat_extractor),
      mProject(project),
      mDatabase(database),
      mBuffer(buffer)
  {
  }

  void operator() ()
  {
    while (!done) {
      while (mBuffer->size()) {
        queue_data data;
        mBuffer->pop(data);
        
        //std::cout << "[Consumer] image name: " << data.image_name.toStdString() << "\n";
        //std::cout << "[Consumer] image id: " << data.image_id << "\n";
        //std::cout << "[Consumer] scale: " << data.scale << "\n";

        /* Feature extraction */
        
        colmap::FeatureKeypoints featureKeypoints;
        colmap::FeatureDescriptors featureDescriptors;
        
        mFeatExtractor->run(data.mat, featureKeypoints, featureDescriptors);

        QByteArray ba = data.image_name.toLocal8Bit();
        const char *img_file = ba.data();
        msgInfo("%i features extracted from image %s", featureKeypoints.size(), img_file);

        if (data.scale > 1) {
          for (auto & featureKeypoint : featureKeypoints){
            featureKeypoint.Rescale(data.scale, data.scale);
          }
        }

        // Escritura en la base de datos
                
        mutex.lock();
        mDatabase->WriteKeypoints(data.image_id, featureKeypoints);
        mDatabase->WriteDescriptors(data.image_id, featureDescriptors);
        mutex.unlock();

        // añade features al proyecto
        mProject->addFeatures(data.image_name, data.image_name + "@" + mProject->database());
      }
    }

    while (mBuffer->size()) {
      queue_data data;
      mBuffer->pop(data);

      //std::cout << "[Consumer] image name: " << data.image_name.toStdString() << "\n";
      //std::cout << "[Consumer] image id: " << data.image_id << "\n";
      //std::cout << "[Consumer] scale: " << data.scale << "\n";

      /* Feature extraction */

      colmap::FeatureKeypoints featureKeypoints;
      colmap::FeatureDescriptors featureDescriptors;

      mFeatExtractor->run(data.mat, featureKeypoints, featureDescriptors);

      QByteArray ba = data.image_name.toLocal8Bit();
      const char *img_file = ba.data();
      msgInfo("%i features extracted from image %s", featureKeypoints.size(), img_file);

      if (data.scale < 1) {
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
      mProject->addFeatures(data.image_name, data.image_name + "@" + mProject->database());
    }

  }

private:

  FeatureExtractor *mFeatExtractor;
  ProjectImp *mProject;
  colmap::Database *mDatabase;
  QueueMPMC<queue_data> *mBuffer;
};



int main(int argc, char** argv)
{

  tl::Chrono chrono("feature extraction");
  chrono.run();

/* Configuración de consola */

  std::string cmd_description = "Feature extraction";

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

/* Parseo de argumentos */

  std::string project_file;
  size_t idx_camera = 2; /// Camera RADIAL
  std::vector<std::string> cameras {
    "SIMPLE_PINHOLE",
    "SIMPLE_RADIAL",
    "RADIAL",
    "FULL_RADIAL"};

  SiftProperties siftProperties;
  int maxImageSize = 3200;
  int maxFeaturesNumber = siftProperties.featuresNumber();
  int octaveResolution = siftProperties.octaveLayers();
  double contrastThreshold = siftProperties.contrastThreshold();
  double edgeThreshold = siftProperties.edgeThreshold();
  double sigma = siftProperties.sigma();

  Command cmd("featextract", cmd_description);
  cmd.push_back(std::make_shared<ArgumentStringRequired>("prj", 'p', "Project file", &project_file));
  cmd.push_back(std::make_shared<ArgumentListStringOptional>("camera_model", 'c', "Set camera model. Available cameras: SIMPLE_PINHOLE, SIMPLE_RADIAL, RADIAL (default), FULL_RADIAL", cameras, &idx_camera));
  cmd.push_back(std::make_shared<ArgumentIntegerOptional>("max_image_size", 's', "Maximum image size (default = 3200)", &maxImageSize));
  cmd.push_back(std::make_shared<ArgumentIntegerOptional>("max_features_number", "Maximum number of features to detect (default = 8192)", &maxFeaturesNumber));
  cmd.push_back(std::make_shared<ArgumentIntegerOptional>("SIFT:octave_resolution", "SIFT: Number of layers in each octave (default = 3)", &octaveResolution));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("SIFT:contrast_threshold", "SIFT: Contrast Threshold", &contrastThreshold));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("SIFT:edge_threshold", "SIFT: Threshold used to filter out edge-like features (default = 10.)", &edgeThreshold));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("SIFT:sigma", "", &sigma));

  cmd.addExample("featextract --p 253/253.xml -c RADIAL");

  // Parseo de los argumentos y comprobación de los mismos
  Command::Status status = cmd.parse(argc, argv);
  if (status == Command::Status::parse_error ) {
    return 1;
  } else if (status == Command::Status::show_help) {
    return 0;
  } else if (status == Command::Status::show_licence) {
    return 0;
  } else if (status == Command::Status::show_version) {
    return 0;
  }

  if (QFileInfo(project_file.c_str()).exists()){

  } else {
    msgError("The project doesn't exist");
    return 1;
  }

  ProjectImp project;
  project.load(project_file.c_str());

  QString database_path = project.database();
  QString project_dir = project.projectFolder();

  QDir dir(project_dir);

/* Fichero log */

  ///TODO: Revisar log
  //QString log_file = project_dir + "/" + project.name() + ".log";

  //tl::Log &log = tl::Log::instance();
  //log.setMessageLevel(tl::MessageLevel::msg_verbose);
  //log.setLogFile(log_file.toStdString());
  //tl::MessageManager::instance().addListener(&log);

/* Chequeo de soporte CUDA */

  /// TODO: Si el equipo no es compatible con CUDA no he conseguido que funcione la opción de OpenGL
  ///       Se desactiva la GPU por ahora si no es compatible con CUDA
  bool bUseGPU = cudaEnabled(8.0, 5.0);

/* Chequeos previos para comprobar que todo este bien */

  if (QFileInfo(database_path).exists()) {
    QFile(database_path).remove();
  }

  /// Se crea la base de datos
  std::string database_path_string = database_path.toStdString();
  colmap::Database database(database_path_string);

//  std::streambuf *backup = nullptr;
//  std::streambuf *psbuf;
//  LogStream logStream;
//  logStream.open(log_file.toStdString());
//  if (logStream.isOpen()){
//    backup = std::cout.rdbuf();
//    psbuf = logStream.rdbuf();
//    std::cout.rdbuf(psbuf);
//  }

  /// Se añaden las imagenes y las cámaras a la base de datos

  //bool bOpenCvRead = true;

//  try {
    std::shared_ptr<FeatureExtractor> feat_extractor(new SiftCudaDetectorDescriptor(maxFeaturesNumber,
                                                                                    octaveResolution,
                                                                                    edgeThreshold,
                                                                                    sigma,
                                                                                    contrastThreshold));

    project.setFeatureExtractor(std::dynamic_pointer_cast<Feature>(feat_extractor));

//    colmap::FeatureKeypoints featureKeypoints;
//    colmap::FeatureDescriptors featureDescriptors;
//    colmap::Camera camera_colmap;
//
//    int camera_id = -1;
//    for (auto it = project.imageBegin(); it != project.imageEnd(); it++){
//      Image image = (*it);
//      //std::string image_name = (*it)->name().toStdString();
//      std::string image_path = image.path().toStdString();
//      QFileInfo file_info(image.path());
//      std::string image_name = file_info.fileName().toStdString();
//
//      msgInfo("Read image: %s", image_path.c_str());
//      
//      colmap::image_t image_id;
//
//      if (!database.ExistsImageWithName(image_name)){
//
//        colmap::Camera camera_colmap;
//        colmap::camera_t camera_id = static_cast<colmap::camera_t>(image.cameraId());
//        if (!database.ExistsCamera(image.cameraId())) {
//          Camera camera = project.findCamera(camera_id);
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
//          camera_id = database.WriteCamera(camera_colmap);
//          camera_colmap.SetCameraId(camera_id);
//        }
//        
//        colmap::Image image_colmap;
//        image_colmap.SetName(image_name);
//        image_colmap.SetCameraId(camera_id);
//
//        image_id = database.WriteImage(image_colmap, false);
//
//      } else {
//        colmap::Image image_colmap = database.ReadImageWithName(image_name);
//        image_id = image_colmap.ImageId();
//      }
//
//      
//      /* Lectura de imagen */
//
//      //cv::Mat mat = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
//
//      //double max_dimension;
//      //if (mat.cols > mat.rows){
//      //  max_dimension = mat.cols;
//      //} else {
//      //  max_dimension = mat.rows;
//      //}
//
//      //double scale = 1;
//      //if (maxImageSize < max_dimension){
//      //  cv::Size size(mat.cols, mat.rows);
//      //  scale = maxImageSize / max_dimension;
//      //  size.width *= scale;
//      //  size.height *= scale;
//      //  cv::resize(mat, mat, size);
//      //}
//      
//      cv::Mat mat;
//      double scale = 1.;
//      
//      if (bOpenCvRead) {
//      
//        mat = cv::imread(image.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE);
//        //cv::Mat mat2 = cv::imread(mImage.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_COLOR);
//        //cv::Mat color_boost;
//        //cv::decolor(mat2, mat, color_boost);
//        //mat2.release();
//        //color_boost.release();
//      
//        if (mat.empty()) {
//          bOpenCvRead = false;
//        } else {
//          double max_dimension;
//          if (mat.cols > mat.rows){
//            max_dimension = mat.cols;
//          } else {
//            max_dimension = mat.rows;
//          }
//      
//          if (maxImageSize < max_dimension){
//            cv::Size size(mat.cols, mat.rows);
//            scale = max_dimension / maxImageSize;
//            size.width /= scale;
//            size.height /= scale;
//#ifdef HAVE_CUDA
//            if (bUseGPU) {
//              cv::cuda::GpuMat gImgIn(mat);
//              cv::cuda::GpuMat gImgResize;
//              cv::cuda::resize(gImgIn, gImgResize, size);
//              gImgResize.download(mat);
//            } else {
//#endif
//              cv::resize(mat, mat, size);
//#ifdef HAVE_CUDA
//            }
//#endif
//          }
//        }
//
//      }
//
//      if (!bOpenCvRead) {
//        std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.path().toStdString());
//        imageReader->open();
//        if (imageReader->isOpen()) {
//          int w = imageReader->cols();
//          int h = imageReader->rows();
//          double max_dimension;
//          if (w > h) {
//            max_dimension = w;
//          } else {
//            max_dimension = h;
//          }
//      
//      
//          if (maxImageSize < max_dimension) {
//            scale = maxImageSize / max_dimension;
//            mat = imageReader->read(scale, scale);
//            scale = 1. / scale;
//          } else {
//            mat = imageReader->read();
//          }
//      
//          if (imageReader->depth() != 8) {
//#ifdef HAVE_CUDA
//            if (bUseGPU) {
//              cv::cuda::GpuMat gImgIn(mat);
//              cv::cuda::GpuMat gImgOut;
//              cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
//              gImgOut.download(mat);
//            } else {
//#endif
//              cv::normalize(mat, mat, 0., 255., cv::NORM_MINMAX, CV_8U);
//#ifdef HAVE_CUDA
//            }
//#endif
//          }
//
//          if (mat.channels() >= 3) {
//#ifdef HAVE_CUDA
//            cv::cuda::GpuMat gImgIn(mat);
//            cv::cuda::GpuMat gImgGray;
//            cv::cuda::cvtColor(gImgIn, gImgGray, cv::COLOR_BGR2GRAY);
//            gImgGray.download(mat);
//#else
//            cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
//#endif
//          }
//
//
//
//          imageReader->close();
//        }
//      }
//
//
///* Feature extraction */
//
//      feat_extractor->run(mat, featureKeypoints, featureDescriptors);
//
//      QByteArray ba = image.name().toLocal8Bit();
//      const char *img_file = ba.data();
//      msgInfo("%i features extracted from image %s", featureKeypoints.size(), img_file);
//
//      if (scale < 1) {
//        for (auto & featureKeypoint : featureKeypoints){
//          featureKeypoint.Rescale(scale, scale);
//        }
//      }
//
//      // Escritura en la base de datos
//      database.WriteKeypoints(image_id, featureKeypoints);
//      database.WriteDescriptors(image_id, featureDescriptors);
//
//      // añade features al proyecto
//      project.addFeatures(image.name(), image.name() + "@" + database_path);
//
//    }
//
//    project.save(project_file.c_str());
//    msgInfo("Project saved");
//
//    uint64_t time = chrono.stop();
//    msgInfo("[Time: %f seconds]", time/1000.);
//
//  } catch (std::exception &e) {
//    msgError(e.what());
//    return 1;
//  }

/********************************************************************************/

  //  for (auto it = project.imageBegin(); it != project.imageEnd(); it++) {
  //    Image image = (*it);

  //    std::string image_path = image.path().toStdString();
  //    QFileInfo file_info(image.path());
  //    QString image_name = file_info.fileName();

  //    colmap::image_t image_id;

  //    if (database.ExistsImageWithName(image_name.toStdString())) {

  //      colmap::Image image_colmap = database.ReadImageWithName(image_name.toStdString());
  //      image_id = image_colmap.ImageId();
  //      QStringList l = image_name.split(" ");
  //      image_colmap.SetName(QString(l[0]).append("_").append(l[1]).toStdString() );
  //      database.UpdateImage(image_colmap);
  //    }

  //  }

  //return 0;

/********************************************************************************/

  QueueMPMC<queue_data> buffer(50);
  ProducerImp producer(&project,
                       &database,
                       maxImageSize,
                       bUseGPU,
                       &buffer);
  ConsumerImp consumer(feat_extractor.get(),
                       &project,
                       &database,
                       &buffer);

  //std::thread producer_thread(producer);
  //std::thread consumer_thread(consumer);

  //producer_thread.join();
  //done = true;
  //consumer_thread.join();

  size_t num_threads = tl::optimalNumberOfThreads();
  std::vector<std::thread> producer_threads(num_threads);
  std::vector<std::thread> consumer_threads(num_threads);

  
  size_t size = project.imagesCount() / num_threads;
  for (int i = 0; i < num_threads; ++i) {
    size_t _ini = i * size;
    size_t _end = _ini + size;
    if (i == num_threads - 1) _end = project.imagesCount();

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

  project.save(project_file.c_str());
  msgInfo("Project saved");

  chrono.stop();

  return 0;
}
