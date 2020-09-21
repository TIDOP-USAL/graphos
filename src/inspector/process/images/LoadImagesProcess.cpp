#include "LoadImagesProcess.h"

#include <tidop/core/messages.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>

#include <colmap/util/bitmap.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

namespace inspector
{

LoadImagesProcess::LoadImagesProcess(std::vector<Image> *images, 
                                     std::vector<Camera> *cameras)
  : mImages(images),
    mCameras(cameras)
{

}

bool LoadImagesProcess::existCamera(const QString &make, const QString &model) const
{
  bool camera_exist = false;
  
  for (auto it = mCameras->begin(); it != mCameras->end(); it++) {
    QString camera_make = it->make();
    QString camera_model = it->model();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      camera_exist = true;
      break;
    }
  }

  return camera_exist;
}

int LoadImagesProcess::findCamera(const QString &make, const QString &model) const
{
  int camera_id = -1;
  for (size_t i = 0; i < mCameras->size(); i++){
    QString camera_make = (*mCameras)[i].make();
    QString camera_model = (*mCameras)[i].model();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      camera_id = i;
      break;
    }
  }
  return camera_id;
}

void inspector::LoadImagesProcess::run()
{
  tl::Chrono chrono("Images loaded");
  chrono.run();

  //QStringList images;
  QSqlDatabase database_cameras = QSqlDatabase::addDatabase("QSQLITE");
  QString database_cameras_path;
#ifdef _DEBUG
  database_cameras_path = QString(INSPECTOR_SOURCE_PATH).append("/res");
#else
  database_cameras_path = qApp->applicationDirPath();
#endif
  database_cameras_path.append("/cameras.db");
  database_cameras.setDatabaseName(database_cameras_path);
          
  bool db_open = false;
  if (QFileInfo(database_cameras_path).exists()) {
    db_open = database_cameras.open();
  } else {
    msgError("The camera database does not exist");
  }

  for (size_t i = 0; i < mImages->size(); i++){

    try {

      QString image = (*mImages)[i].path();
      
      msgInfo("Load image: %s", image.toStdString().c_str());
      //tl::Chrono crono(std::string("Read image :").append(image.toStdString()));
      //crono.run();

      std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.toStdString());
      imageReader->open();
      if (!imageReader->isOpen()) throw std::runtime_error("  Failed to read image file");

      int id_camera = 0;
      int width = imageReader->cols();
      int height = imageReader->rows();
      Camera camera;
      int camera_id = -1;

      tl::MessageManager::pause();
      std::shared_ptr<tl::ImageMetadata> image_metadata = imageReader->metadata();
      bool bActiveCameraName = false;
      bool bActiveCameraModel = false;
      std::string camera_make = image_metadata->metadata("EXIF_Make", bActiveCameraName);
      std::string camera_model = image_metadata->metadata("EXIF_Model", bActiveCameraModel);
      tl::MessageManager::resume();

      if (bActiveCameraName && bActiveCameraModel){

        msgInfo(" - Camera: %s %s", camera_make.c_str(), camera_model.c_str());

        if (existCamera(camera_make.c_str(), camera_model.c_str())) {

          camera_id = findCamera(camera_make.c_str(), camera_model.c_str());

        } else {

          camera = Camera(camera_make.c_str(), camera_model.c_str());

          camera.setWidth(width);
          camera.setHeight(height);

          bool bActive = false;
          std::string focal = image_metadata->metadata("EXIF_FocalLength", bActive);
          if (bActive){
              camera.setFocal(parseFocal(focal, 1.2 * std::max(width, height)));
          } else {
            camera.setFocal(1.2 * std::max(width, height));
          }

          /// Lectura del tamaño del sensor de la base de datos
          if (db_open) {
            double sensor_width_px = std::max(width, height);
            double sensor_width_mm = -1;
            double scale = 1.0;

            QSqlQuery query;
            query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
            query.bindValue(":camera_make", camera_make.c_str());
            if (query.exec()) {
              while (query.next()) {
                id_camera = query.value(0).toInt();
              }
            } else {
              QSqlError err = query.lastError();
              throw err.text().toStdString();
            }

            if (id_camera == -1) {
              msgWarning("Camera '%s' not found in database", camera_make.c_str());
            } else {
              query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera LIKE :id_camera");
              query.bindValue(":camera_model", camera_model.c_str());
              query.bindValue(":id_camera", id_camera);
              if (query.exec()) {
                while (query.next()) {
                  sensor_width_mm = query.value(0).toDouble();
                  scale = sensor_width_mm / sensor_width_px;
                  camera.setSensorSize(sensor_width_mm);

                  ///TODO: Focal en mm?
                  //msgInfo("Sensor size for camera %s %s is %f (mm)", camera_make.c_str(), camera_model.c_str(), sensor_width_mm);
                }

                if (sensor_width_mm < 0.) {
                  msgWarning("Camera model (%s %s) not found in database", camera_make.c_str(), camera_model.c_str());
                }

              } else {
                QSqlError err = query.lastError();
                msgWarning("%s", err.text().toStdString().c_str());
              }
            }

          }

          if (db_open) database_cameras.close();

          //id_camera = mCamerasModel->addCamera(camera);
          camera_id = mCameras->size();
          mCameras->push_back(camera);
        }

      } else {
        /// Camara desconocida
        msgWarning("Unknow camera for image: %s", image.toStdString().c_str());
        Camera camera2;
        bool bFound = false;
        //int id_camera;
        int counter = 0;
        for (auto it = mCameras->begin(); it != mCameras->end(); it++) {
          camera2 = *it;
          if (camera2.make().compare("Unknown camera") == 0) {
            if (camera2.width() == width && camera2.height() == height) {
              // Misma camara
              //id_camera = it->first;
              bFound = true;
              break;
            }
            counter++;
          }
        }

        if (bFound == false) {
          camera = Camera("Unknown camera", QString::number(counter));
          camera.setWidth(width);
          camera.setHeight(height);
          camera.setFocal(1.2 * std::max(width, height));
          //id_camera = mCamerasModel->addCamera(camera);
          //camera.setModel(QString::number(id_camera));
          camera_id = mCameras->size();
          mCameras->push_back(camera);
        } else {
          camera_id = mCameras->size() - 1;
        }

      }

      tl::MessageManager::pause();

      bool bActive = false;
      std::string latitude = image_metadata->metadata("EXIF_GPSLatitude", bActive);
      std::string latitude_ref = image_metadata->metadata("EXIF_GPSLatitudeRef", bActive);
      if (bActive){

        double degrees = TL_DOUBLE_MIN;
        double minutes = TL_DOUBLE_MIN;
        double seconds = TL_DOUBLE_MIN;

        size_t pos1 = latitude.find("(");
        size_t pos2 = latitude.find(")");
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
          degrees = std::stod(latitude.substr(pos1+1, pos2 - pos1+1));
        }

        pos1 = latitude.find("(", pos2);
        pos2 = latitude.find(")", pos1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
          minutes = std::stod(latitude.substr(pos1+1, pos2 - pos1+1));
        }
                
        pos1 = latitude.find("(", pos2);
        pos2 = latitude.find(")", pos1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
          seconds = std::stod(latitude.substr(pos1+1, pos2 - pos1+1));
        }

        double latitudeDecimalDegrees = tl::math::degreesToDecimalDegrees(degrees, minutes, seconds);
        if (latitude_ref.compare("S") == 0) latitudeDecimalDegrees = -latitudeDecimalDegrees;
        (*mImages)[i].setLatitudeExif(latitudeDecimalDegrees);
      }

      std::string longitude = image_metadata->metadata("EXIF_GPSLongitude", bActive);
      std::string longitude_ref = image_metadata->metadata("EXIF_GPSLongitudeRef", bActive);
      if (bActive) {
        double degrees = TL_DOUBLE_MIN;
        double minutes = TL_DOUBLE_MIN;
        double seconds = TL_DOUBLE_MIN;

        size_t pos1 = longitude.find("(");
        size_t pos2 = longitude.find(")");
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
          degrees = std::stod(longitude.substr(pos1+1, pos2 - pos1+1));
        }

        pos1 = longitude.find("(", pos2);
        pos2 = longitude.find(")", pos1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
          minutes = std::stod(longitude.substr(pos1+1, pos2 - pos1+1));
        }
                
        pos1 = longitude.find("(", pos2);
        pos2 = longitude.find(")", pos1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
          seconds = std::stod(longitude.substr(pos1+1, pos2 - pos1+1));
        }

        double longitudeDecimalDegrees = tl::math::degreesToDecimalDegrees(degrees, minutes, seconds);
        if (longitude_ref.compare("W") == 0) longitudeDecimalDegrees = -longitudeDecimalDegrees;
        (*mImages)[i].setLongitudeExif(longitudeDecimalDegrees);
      }

      std::string altitude = image_metadata->metadata("EXIF_GPSAltitude", bActive);
      if (bActive) {
        size_t pos1 = altitude.find("(");
        size_t pos2 = altitude.find(")");
        double h;
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
          h = std::stod(altitude.substr(pos1+1, pos2 - pos1+1));
        }
        (*mImages)[i].setAltitudeExif(h);
      }

      tl::MessageManager::resume();

      //msgInfo(" - Coordinates: (%.4lf, %.4lf, %.2lf)", (*mImages)[i].longitudeExif(), (*mImages)[i].latitudeExif(), (*mImages)[i].altitudeExif());

      emit imageAdded(i, camera_id);
      
      //crono.stop();

    } catch (std::exception & e) {
      msgError(e.what());
    }

    emit statusChangedNext();

  }

  /*uint64_t time = */chrono.stop();
  //msgInfo("[Time: %f seconds]", time / 1000.);
}

double LoadImagesProcess::parseFocal(const std::string &focal, double def)
{
  double r_focal;

  try {
    std::string f = focal;
  
    size_t pos = f.find("(");
    if (pos != std::string::npos) {
      f.erase(pos, 1);
    }

    pos = f.find(")");
    if (pos != std::string::npos) {
      f.erase(pos, 1);
    }

    std::string::size_type sz;
    r_focal = std::stod(f, &sz);

  } catch (...) {
    r_focal = def;
  }

  return r_focal;
}

} // inspector
