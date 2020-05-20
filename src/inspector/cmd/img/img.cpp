#include <inspector/process/features/FeatureExtractorProcess.h>
#include <inspector/core/utils.h>
#include <inspector/core/project.h>
#include <inspector/core/camera.h>
#include <inspector/core/features/sift.h>
#include <inspector/core/features/featio.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

//#include <colmap/base/database.h>
//#include <colmap/base/camera_models.h>

//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/imgproc.hpp>

#include <QDir>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

#include <cstdlib>
#include <iostream>


using namespace inspector;
using namespace tl;

static std::string cmd_description = QT_TRANSLATE_NOOP("", "Images");

void consoleConfig()
{

  tl::Console &console = tl::Console::instance();
  console.setLogLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);
}

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  tl::Chrono chrono("img");
  chrono.run();

  consoleConfig();

/* Parseo de argumentos */

  std::string project_file;
  std::string image_to_add;
  std::string image_to_remove;
  size_t idx_camera = 2; /// Camera RADIAL
  std::vector<std::string> cameras {
    "SIMPLE_PINHOLE",
    "SIMPLE_RADIAL",
    "RADIAL",
    "FULL_RADIAL"};

  Command cmd("featextract", cmd_description);
  cmd.push_back(std::make_shared<ArgumentStringRequired>("prj", 'p', "Project file", &project_file));
  cmd.push_back(std::make_shared<ArgumentListStringOptional>("camera_model", 'c', "Set camera model. Available cameras: SIMPLE_PINHOLE, SIMPLE_RADIAL, RADIAL (default), FULL_RADIAL", cameras, &idx_camera));
  cmd.push_back(std::make_shared<ArgumentStringOptional>("image_add", 'a', "Add an image to the project", &image_to_add));
  cmd.push_back(std::make_shared<ArgumentStringOptional>("image_remove", 'r', "Remove an image from the project", &image_to_remove));

  cmd.addExample("img --p 253/253.xml --image_add image001.jpg");
  cmd.addExample("img --p 253/253.xml --image_remove image001.jpg");

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

  QFileInfo file_info(project_file.c_str());
  if (!file_info.exists()){
    msgError("The project doesn't exist");
    return 1;
  }

  ProjectImp project;
  project.load(file_info.absoluteFilePath());

  QString project_dir = project.projectFolder();

/* Fichero log */

  QString log_file = file_info.path() + "/" + file_info.baseName() + ".log";

  tl::Log &log = tl::Log::instance();
  log.setLogLevel(tl::MessageLevel::msg_verbose);
  log.setLogFile(log_file.toLatin1());
  tl::MessageManager::instance().addListener(&log);

/* Chequeos previos para comprobar que todo este bien */

  if (!image_to_add.empty()) {
    /// Se abre la base de datos de cámaras
    QSqlDatabase database_cameras = QSqlDatabase::addDatabase("QSQLITE");
    QString database_cameras_path = qApp->applicationDirPath();
    database_cameras_path.append("/cameras.db");
    database_cameras.setDatabaseName(database_cameras_path);
    bool db_open = false;
    if (QFileInfo(database_cameras_path).exists()){
      db_open = database_cameras.open();
    } else {
      msgError("The camera database does not exist");
    }

    colmap::Bitmap bitmap;

    Image image(image_to_add.c_str());

    if (!bitmap.Read(image_to_add, false)) {
      msgError("  Failed to read image file");
      return 1;
    } else {
      msgInfo("Read image: %s", image_to_add.c_str());
    }

    double lon;
    if (bitmap.ExifLongitude(&lon)){
      image.setLongitudeExif(lon);
    }
    double lat;
    if (bitmap.ExifLatitude(&lat)){
      image.setLatitudeExif(lat);
    }
    double altitude;
    if (bitmap.ExifAltitude(&altitude)){
      image.setAltitudeExif(altitude);
    }

    msgInfo(" - Coordinates: (%.4lf, %.4lf, %.2lf)", image.longitudeExif(), image.latitudeExif(), image.altitudeExif());

    int width = bitmap.Width();
    int height = bitmap.Height();

    msgInfo(" - Dimensions: %ix%i", width, height);

    Camera camera;

    std::string camera_make;
    std::string camera_model;

    if (bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Make", &camera_make) &&
        bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Model", &camera_model)) {

      msgInfo(" - Camera: %s %s", camera_make.c_str(), camera_model.c_str());

      int id_camera = project.cameraId(camera_make.c_str(), camera_model.c_str());
      if (id_camera == 0){  /// Se añade la cámara al proyecto

        camera = Camera(camera_make.c_str(), camera_model.c_str());

        camera.setWidth(width);
        camera.setHeight(height);

        double focal;
        if (bitmap.ExifFocalLength(&focal)){
          camera.setFocal(focal);
        } else {
          camera.setFocal(1.2 * std::max(width, height));
        }

        /// Lectura del tamaño del sensor de la base de datos
        if (db_open) {
          double sensor_width_px = std::max(bitmap.Width(), bitmap.Height());
          double sensor_width_mm = -1;
          double scale = 1.0;

          QSqlQuery query;
          query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
          query.bindValue(":camera_make", camera_make.c_str());
          if (query.exec()){

            while(query.next()){
              id_camera = query.value(0).toInt();
            }

          } else {
            QSqlError err = query.lastError();
            msgWarning("%s", err.text().toStdString().c_str());
            return 1;
          }

          if (id_camera == -1) {
            msgWarning("Camera '%s' not found in database", camera_make.c_str());
          } else {
            query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera LIKE :id_camera");
            query.bindValue(":camera_model", camera_model.c_str());
            query.bindValue(":id_camera", id_camera);
            if (query.exec()){
              while(query.next()){
                sensor_width_mm = query.value(0).toDouble();
                scale = sensor_width_mm / sensor_width_px;
                camera.setSensorSize(sensor_width_mm);

                ///TODO: Focal en mm?
                //msgInfo("Sensor size for camera %s %s is %f (mm)", camera_make.c_str(), camera_model.c_str(), sensor_width_mm);
              }

              if (sensor_width_mm < 0.){
                msgWarning("Camera model (%s %s) not found in database", camera_make.c_str(), camera_model.c_str());
              }

            } else {
              QSqlError err = query.lastError();
              msgWarning("%s", err.text().toStdString().c_str());
            }
          }

        }

        if (db_open) database_cameras.close();

        id_camera = project.addCamera(camera);
      } else {
        camera = project.findCamera(id_camera);
      }

      image.setCameraId(id_camera);

    } else {
      /// Camara desconocida
      TL_TODO("Cambiar el modelo de camara... Si no no se podria tener varias camaras desconocidas")
      Camera camera2;
      bool bFound = false;
      int id_camera;
      int counter = 0;
      for (auto it = project.cameraBegin(); it != project.cameraEnd(); it++) {
        camera2 = it->second;
        if (camera2.make().compare("Unknown camera") == 0){
          if (camera2.width() == width && camera2.height() == height) {
            // Misma camara
            id_camera = it->first;
            bFound = true;
            break;
          }
          counter++;
        }
      }

      if (bFound == false){
        camera = Camera("Unknown camera", QString::number(counter));
        camera.setWidth(width);
        camera.setHeight(height);
        camera.setFocal(1.2 * std::max(width, height));
        id_camera = project.addCamera(camera);
        camera.setModel(QString::number(id_camera));
      }

      image.setCameraId(id_camera);
      msgWarning("Unknow camera for image: %s", image_to_add.c_str());
    }


    project.addImage(image);

    project.save(file_info.absoluteFilePath());

  }

  if (!image_to_remove.empty()){

  }

  uint64_t time = chrono.stop();
  msgInfo("[Time: %f seconds]", time/1000.);

  return 0;
}
