#include <inspector/process/features/FeatureExtractorProcess.h>
#include <inspector/core/utils.h>
#include <inspector/core/project.h>
#include <inspector/core/features/sift.h>
#include <inspector/core/features/featio.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <QDir>
#include <QTextStream>

#include <cstdlib>
#include <iostream>


using namespace inspector;
using namespace tl;

int main(int argc, char** argv)
{

  tl::Chrono chrono("feature extraction");
  chrono.run();

/* Configuración de consola */

  std::string cmd_description = "Feature extraction";

  tl::Console &console = tl::Console::instance();
  console.setLogLevel(tl::MessageLevel::msg_verbose);
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
  int maxImageSize = -1;
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

  QString log_file = project_dir + "/" + project.name() + ".log";

  tl::Log &log = tl::Log::instance();
  log.setLogLevel(tl::MessageLevel::msg_verbose);
  log.setLogFile(log_file.toStdString());
  tl::MessageManager::instance().addListener(&log);

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

  try {
    std::shared_ptr<FeatureExtractor> feat_extractor(new SiftCudaDetectorDescriptor(maxFeaturesNumber,
                                                                                    octaveResolution,
                                                                                    contrastThreshold,
                                                                                    edgeThreshold,
                                                                                    sigma));

    project.setFeatureExtractor(std::dynamic_pointer_cast<Feature>(feat_extractor));

    colmap::FeatureKeypoints featureKeypoints;
    colmap::FeatureDescriptors featureDescriptors;
    colmap::Camera camera_colmap;

    int camera_id = -1;
    for (auto it = project.imageBegin(); it != project.imageEnd(); it++){
      Image image = (*it);
      //std::string image_name = (*it)->name().toStdString();
      std::string image_path = image.path().toStdString();
      QFileInfo file_info(image.path());
      std::string image_name = file_info.fileName().toStdString();

      msgInfo("Read image: %s", image_path.c_str());

      if (camera_id != image.cameraId()){
        camera_id = image.cameraId();
        Camera camera = project.findCamera(camera_id);

        int camera_model_id = colmap::CameraModelNameToId(camera.type().toStdString());
        double focal_length = camera.focal();
        size_t width = static_cast<size_t>(camera.width());
        size_t height = static_cast<size_t>(camera.height());
        if (round(1.2 * std::max(width, height)) == round(focal_length)){
          camera_colmap.SetPriorFocalLength(false);
        } else {
          camera_colmap.SetPriorFocalLength(true);
        }
        camera_colmap.InitializeWithId(camera_model_id, focal_length, width, height);
        camera_colmap.SetCameraId(database.WriteCamera(camera_colmap));
      }


      colmap::Image image_colmap;
      image_colmap.SetName(image_name);
      image_colmap.SetCameraId(camera_colmap.CameraId());

      colmap::image_t image_id = database.WriteImage(image_colmap, false);

      /* Feature extraction */

      cv::Mat mat = cv::imread(image_path, cv::IMREAD_GRAYSCALE);

      double max_dimension;
      if (mat.cols > mat.rows){
        max_dimension = mat.cols;
      } else {
        max_dimension = mat.rows;
      }

      double scale = 1;
      if (maxImageSize < max_dimension){
        cv::Size size(mat.cols, mat.rows);
        scale = maxImageSize / max_dimension;
        size.width *= scale;
        size.height *= scale;
        cv::resize(mat, mat, size);
      }

      feat_extractor->run(mat, featureKeypoints, featureDescriptors);

      QByteArray ba = image.name().toLocal8Bit();
      const char *img_file = ba.data();
      msgInfo("%i features extracted from image %s", featureKeypoints.size(), img_file);

      if (scale < 1) {
        for (auto & featureKeypoint : featureKeypoints){
          featureKeypoint.Rescale(scale, scale);
        }
      }

      // Escritura en la base de datos
      database.WriteKeypoints(image_id, featureKeypoints);
      database.WriteDescriptors(image_id, featureDescriptors);

      // añade features al proyecto
      project.addFeatures(image.name(), image.name() + "@" + database_path);

    }

    project.save(project_file.c_str());
    msgInfo("Project saved");

    uint64_t time = chrono.stop();
    msgInfo("[Time: %f seconds]", time/1000.);

  } catch (std::exception &e) {
    msgError(e.what());
    return 1;
  }

  return 0;
}
