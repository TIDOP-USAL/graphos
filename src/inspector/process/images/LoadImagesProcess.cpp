#include "LoadImagesProcess.h"

#include <tidop/core/messages.h>

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

//Camera LoadImagesProcess::findCamera(const QString &make, const QString &model) const
//{
//  Camera camera;
//  for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
//    QString camera_make = it->make();
//    QString camera_model = it->model();
//    if (make.compare(camera_make) == 0 &&
//        model.compare(camera_model) == 0){
//      camera = *it;
//      break;
//    }
//  }
//  return camera;
//}

int LoadImagesProcess::findCamera(const QString &make, const QString &model) const
{
  int camera_id = -1;
  //for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
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
  tl::Chrono chrono("Load Images");
  chrono.run();

  QStringList images;
  colmap::Bitmap bitmap;

  for (size_t i = 0; i < mImages->size(); i++){
    try {

      QString image = (*mImages)[i].path();
      int id_camera = 0;
      if (!bitmap.Read(image.toStdString(), false)) {
        throw std::runtime_error("  Failed to read image file");
      }

      int width = bitmap.Width();
      int height = bitmap.Height();

      Camera camera;
      int camera_id = -1;

      std::string camera_make;
      std::string camera_model;

      if (bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Make", &camera_make) &&
          bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Model", &camera_model)) {

        msgInfo(" - Camera: %s %s", camera_make.c_str(), camera_model.c_str());

        if (existCamera(camera_make.c_str(), camera_model.c_str())) {

          camera_id = findCamera(camera_make.c_str(), camera_model.c_str());

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
        }
         
        camera_id = mCameras->size();
        mCameras->push_back(camera);
      }

      //Image img(image);
      double lon;
      if (bitmap.ExifLongitude(&lon)) {
        (*mImages)[i].setLongitudeExif(lon);
      }
      double lat;
      if (bitmap.ExifLatitude(&lat)) {
        (*mImages)[i].setLatitudeExif(lat);
      }
      double altitude;
      if (bitmap.ExifAltitude(&altitude)) {
        (*mImages)[i].setAltitudeExif(altitude);
      }

      msgInfo(" - Coordinates: (%.4lf, %.4lf, %.2lf)", (*mImages)[i].longitudeExif(), (*mImages)[i].latitudeExif(), (*mImages)[i].altitudeExif());

      emit imageAdded(i, camera_id);

      //img.setCameraId(id_camera);
      //mImagesModel->addImage(img);

      //images.push_back(image);
    } catch (std::exception & e) {
      msgError(e.what());
    }

    emit statusChangedNext();
  }

  uint64_t time = chrono.stop();
  msgInfo("[Time: %f seconds]", time / 1000.);
}

} // inspector
