#include "LoadImagesProcess.h"

#include <tidop/core/messages.h>

#include <colmap/util/bitmap.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

namespace inspector
{

LoadImagesProcess::LoadImagesProcess(const QStringList &images, 
                                     const std::vector<Camera> &cameras)
  : mImages(images),
    mCameras(cameras)
{

}

bool LoadImagesProcess::existCamera(const QString &make, const QString &model) const
{
  bool camera_exist = false;
  
  for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
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

Camera LoadImagesProcess::findCamera(const QString &make, const QString &model) const
{
  Camera camera;
  for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
    QString camera_make = it->make();
    QString camera_model = it->model();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      camera = *it;
      break;
    }
  }
  return camera;
}

void inspector::LoadImagesProcess::run()
{
  tl::Chrono chrono("Load Images");
  chrono.run();

  QStringList images;
  colmap::Bitmap bitmap;

  for (auto &image : mImages) {
    try {

      int id_camera = 0;
      if (!bitmap.Read(image.toStdString(), false)) {
        throw std::runtime_error("  Failed to read image file");
      }

      int width = bitmap.Width();
      int height = bitmap.Height();

      Camera camera;

      std::string camera_make;
      std::string camera_model;

      if (bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Make", &camera_make) &&
          bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Model", &camera_model)) {

        msgInfo(" - Camera: %s %s", camera_make.c_str(), camera_model.c_str());

        if (existCamera(camera_make.c_str(), camera_model.c_str())) {

          camera = findCamera(camera_make.c_str(), camera_model.c_str());

        } else {

          camera = Camera(camera_make.c_str(), camera_model.c_str());

          camera.setWidth(width);
          camera.setHeight(height);

          double focal;
          if (bitmap.ExifFocalLength(&focal)) {
            camera.setFocal(focal);
          } else {
            camera.setFocal(1.2 * std::max(width, height));
          }

          QSqlDatabase database_cameras = QSqlDatabase::addDatabase("QSQLITE");
          QString database_cameras_path;
#ifdef _DEBUG
          database_cameras_path = QString(INSPECTOR_SOURCE_PATH).append("/res");
#else
          database_cameras_path = qApp->applicationDirPath();
#endif
          database_cameras.setDatabaseName(database_cameras_path);
          bool db_open = false;
          if (QFileInfo(database_cameras_path).exists()) {
            db_open = database_cameras.open();
          } else {
            msgError("The camera database does not exist");
          }

          /// Lectura del tamaño del sensor de la base de datos
          if (db_open) {
            double sensor_width_px = std::max(bitmap.Width(), bitmap.Height());
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
          mCameras.push_back(camera);
        }

      } else {
        /// Camara desconocida
        msgWarning("Unknow camera for image: %s", image.toStdString().c_str());
        Camera camera2;
        bool bFound = false;
        //int id_camera;
        int counter = 0;
        for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
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
        }

        mCameras.push_back(camera);

      }

      Image img(image);
      double lon;
      if (bitmap.ExifLongitude(&lon)) {
        img.setLongitudeExif(lon);
      }
      double lat;
      if (bitmap.ExifLatitude(&lat)) {
        img.setLatitudeExif(lat);
      }
      double altitude;
      if (bitmap.ExifAltitude(&altitude)) {
        img.setAltitudeExif(altitude);
      }

      msgInfo(" - Coordinates: (%.4lf, %.4lf, %.2lf)", img.longitudeExif(), img.latitudeExif(), img.altitudeExif());

      emit imageAdded(img, camera);

      //img.setCameraId(id_camera);
      //mImagesModel->addImage(img);

      //images.push_back(image);
    } catch (std::exception & e) {
      msgError(e.what());
    }
  }

  uint64_t time = chrono.stop();
  msgInfo("[Time: %f seconds]", time / 1000.);
}

} // inspector
