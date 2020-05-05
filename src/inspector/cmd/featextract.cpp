#include <inspector/process/features/FeatureExtractorProcess.h>
#include <inspector/core/utils.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

#include <QDir>
#include <QTextStream>

#include <boost/algorithm/string.hpp>

#include <cstdlib>
#include <iostream>


using namespace inspector;
using namespace tl;

#if (__cplusplus >= 201703L)
namespace fs = std::filesystem;
#else
namespace fs = boost::filesystem;
#endif

int main(int argc, char** argv)
{

/* Configuración de consola */

  std::string cmd_description = "Feature extraction";

  tl::Console &console = tl::Console::instance();
  console.setLogLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

/* Parseo de argumentos */

  fs::path database_path;
  fs::path image_path;
  fs::path output_path;
  size_t idx_camera = 2; /// Camera RADIAL
  std::vector<std::string> cameras {
    "SIMPLE_PINHOLE",
    "SIMPLE_RADIAL",
    "RADIAL",
    "FULL_RADIAL"};

  int maxImageSize = TL_INT_MIN;
  int maxFeaturesNumber = TL_INT_MIN;
  int octaveResolution = TL_INT_MIN;
  double peakThreshold = TL_DOUBLE_MIN;
  double edgeThreshold = TL_DOUBLE_MIN;

  Command cmd("featextract", cmd_description);
  cmd.push_back(std::make_shared<ArgumentPathRequired>("database_path", 'd', "Database path", &database_path));
  cmd.push_back(std::make_shared<ArgumentPathRequired>("image_path", 'i', "Images path", &image_path));
  cmd.push_back(std::make_shared<ArgumentPathOptional>("output_path", 'o', "Output path", &output_path));
  cmd.push_back(std::make_shared<ArgumentListStringOptional>("camera_model", 'c', "Set camera model. Available cameras: SIMPLE_PINHOLE, SIMPLE_RADIAL, RADIAL (default), FULL_RADIAL", cameras, &idx_camera));
  cmd.push_back(std::make_shared<ArgumentIntegerOptional>("max_image_size", 's', "Maximum image size (default = 3200)", &maxImageSize));
  cmd.push_back(std::make_shared<ArgumentIntegerOptional>("max_features_number", "SIFT: Maximum number of features to detect (default = 8192)", &maxFeaturesNumber));
  cmd.push_back(std::make_shared<ArgumentIntegerOptional>("octave_resolution", "SIFT: Number of layers in each octave (default = 3)", &octaveResolution));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("peak_threshold", "SIFT: Peak Threshold", &peakThreshold));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("edge_threshold", "SIFT: Threshold used to filter out edge-like features (default = 10.)", &edgeThreshold));

  cmd.addExample("featextract --database_path database.db --image_path images --output_path output -c RADIAL");

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

  std::string s_path = output_path.string();
  QDir dir(s_path.c_str());
  if (!dir.exists()){
    if (dir.mkpath(".") == false){
      msgWarning("The output directory cannot be created: %s", output_path.c_str());
    }
  }

/* Fichero log */

  tl::Log &log = tl::Log::instance();
  log.setLogLevel(tl::MessageLevel::msg_verbose);
  QString log_file = dir.filePath("featextract.log");
  log.setLogFile(log_file.toLatin1());

  /// Se subscribe el log al gestor de mensajes
  tl::MessageManager::instance().addListener(&log);

/* Chequeo de soporte CUDA */

  /// TODO: Si el equipo no es compatible con CUDA no he conseguido que funcione la opción de OpenGL
  ///       Se desactiva la GPU por ahora si no es compatible con CUDA
  bool bUseGPU = cudaEnabled(8.0, 5.0);

/* Chequeos previos para comprobar que todo este bien */

  if (boost::iequals(database_path.extension().string(), ".db")) {
    msgInfo("Set database: %s", database_path.string().c_str());
  } else {
    msgError("Invalid database: %s", database_path.string().c_str());
    return 1;
  }

  if (fs::exists(database_path)){
    //if (overwrite){
    fs::remove(database_path);
    //} else {
    //  msgWarning("Database %s already exist", database_path.c_str());
    //}
  }

  if (!image_path.empty()) {
    if (fs::exists(image_path)) {
      msgInfo("Set image path: %s", image_path.string().c_str());
      ///TODO: ¿Listar imagenes del directorio?
    } else {
      msgError("Directory does not exist: %s", image_path.string().c_str());
      return 1;
    }
  }

  //if (!image_list_path.empty()) {
  //  if (fs::exists(image_list_path)) {
  //    msgInfo("Set image list: %s", image_list_path.string().c_str());
  //  } else {
  //    msgError("Image list does not exist: %s", image_list_path.string().c_str());
  //    return 1;
  //  }
  //} else {
  //  return 1;
  //}

//  /// Se crea la base de datos

//  colmap::Database database(database_path.string());

//  colmap::Camera camera;
//  int camera_model_id = colmap::CameraModelNameToId(cameras[2]);
//  colmap::Bitmap bitmap;

//  /// Se añaden las imagenes y las cámaras a la base de datos

//  std::vector<std::string> images;
//  std::list<std::string> files;
//  ///TODO: por ahora...
//  tl::fileListByExt(image_path.string(), &files, ".jpg");
//  tl::fileListByExt(image_path.string(), &files, ".JPG");
//  for (auto &file : files) {
//    fs::path img_path(image_path);
//    img_path.append(file);
//    images.push_back(img_path.string());


//    colmap::Image image;

//    /// Nombre de la imagen (extensión incluida)
//    image.SetName(file);
//    std::string img_string = img_path.string();
//    msgInfo("Read image: %s", img_string.c_str());
//    if (!bitmap.Read(img_string, false)) {
//      msgError("  Failed to read image file");
//      return 1;
//    }

//    if (camera.CameraId() == colmap::kInvalidCameraId) {
//      double focal_length = 0.0;
//      size_t w = static_cast<size_t>(bitmap.Width());
//      size_t h = static_cast<size_t>(bitmap.Height());

//      if (bitmap.ExifFocalLength(&focal_length)) {
//        camera.SetPriorFocalLength(true);
//      }
//      else {
//        focal_length = 1.2 * std::max(w, h);
//        camera.SetPriorFocalLength(false);
//      }


//      camera.InitializeWithId(camera_model_id, focal_length, w, h);
//      camera.SetWidth(w);
//      camera.SetHeight(h);

//      //msgInfo("  Dimensions:       %i x %i", w, h);
//      //msgInfo("  Camera:           %s", camera.ModelName().c_str());
//      //msgInfo("  Focal lenght:     %.2fpx", focal_length);

//      if (!camera.VerifyParams()) {
//        return 1;
//      }

//      camera.SetCameraId(database.WriteCamera(camera));
//    }

//    image.SetCameraId(camera.CameraId());

//    if (!bitmap.ExifLatitude(&image.TvecPrior(0)) ||
//      !bitmap.ExifLongitude(&image.TvecPrior(1)) ||
//      !bitmap.ExifAltitude(&image.TvecPrior(2))) {
//      image.TvecPrior().setConstant(std::numeric_limits<double>::quiet_NaN());
//    }

//    //msgInfo("  GPS:              Lat=%.3f, Lon=%.3f, Alt=%3.f", image.TvecPrior(0), image.TvecPrior(1), image.TvecPrior(2));

//    database.WriteImage(image, false);
//  }

//  /* Feature extraction */

//  std::streambuf *backup = nullptr;
//  std::streambuf *psbuf;
//  LogStream logStream;
//  logStream.open(log_file.toStdString());
//  if (logStream.isOpen()){
//    backup = std::cout.rdbuf();
//    psbuf = logStream.rdbuf();
//    std::cout.rdbuf(psbuf);
//  }

//  std::string database_path_string = database_path.string();
//  std::shared_ptr<FeatureExtractorSift> feat_extractor = std::make_shared<FeatureExtractorSift>();

//  feat_extractor->setUseGPU(bUseGPU);
//  feat_extractor->setCamera(cameras[2]);  /// TODO: hay un bug en ArgumentListStringOptional
//  feat_extractor->setDatabasePath(database_path.string());
//  feat_extractor->setImagePath(image_path.string());
//  feat_extractor->setSingleCamera(true);  ///TODO: revisar esto que estamos leyendo las imágenes para ver si son diferentes cámaras

//  if (maxImageSize != TL_INT_MIN) feat_extractor->setMaxImageSize(maxImageSize);
//  if (maxFeaturesNumber != TL_INT_MIN) feat_extractor->setMaxFeaturesNumber(maxFeaturesNumber);
//  if (octaveResolution != TL_INT_MIN) feat_extractor->setOctaveResolution(octaveResolution);
//  if (peakThreshold > 0.) feat_extractor->setPeakThreshold(peakThreshold);
//  else feat_extractor->setPeakThresholdAuto(true);
//  if (edgeThreshold > 0.) feat_extractor->setEdgeThreshold(edgeThreshold);

//  if (feat_extractor->run() == FeatureExtractorSift::Status::error) {
//    msgError("Feature Extractor fails");
//    feat_extractor.reset();

//    if (logStream.isOpen()){
//      std::cout.rdbuf(backup);
//      logStream.close();
//    }

//    return 1;
//  } else {
//    ///TODO: ¿Añadir algo mas a la salida de colmap??
//  }

//  feat_extractor.reset();

//  if (logStream.isOpen()){
//    std::cout.rdbuf(backup);
//    logStream.close();
//  }

  return 0;
}
