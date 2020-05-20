#include "CamerasModel.h"

#include <tidop/core/messages.h>

//#include <colmap/base/database.h>
//#include <colmap/base/camera_models.h>
#include <colmap/util/bitmap.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QFileInfo>

namespace inspector
{

namespace ui
{

CamerasModelImp::CamerasModelImp(Project *project,
                                 QObject *parent)
  : CamerasModel(parent),
    mProject(project)
{
  init();
}

int CamerasModelImp::addCamera(const QString &image)
{
  colmap::Bitmap bitmap;
  int id_camera = 0;
  if (!bitmap.Read(image.toStdString(), false)) {
    TL_TODO("Devolver una excepción")
    msgError("  Failed to read image file");
    return -1;
  } /*else {
    msgInfo("Read image: %s", image.toStdString().c_str());
  }*/

  int width = bitmap.Width();
  int height = bitmap.Height();

  Camera camera;

  std::string camera_make;
  std::string camera_model;

  if (bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Make", &camera_make) &&
      bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Model", &camera_model)) {

    msgInfo(" - Camera: %s %s", camera_make.c_str(), camera_model.c_str());

    id_camera = cameraID(camera_make.c_str(), camera_model.c_str());
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
          return -1;
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

      id_camera = mProject->addCamera(camera);
    } /*else {
      camera = mProject->findCamera(id_camera);
    }*/

  } else {
    /// Camara desconocida
//    int id_camera = mProject->cameraId("Unknown camera", "Unknown camera");
//    Camera camera2 = mProject->findCamera(id_camera);
//    if (id_camera == 0 ||
//        (camera2.width() != width && camera2.height() != height)){
//      camera = Camera("Unknown camera", "Unknown camera");
//      camera.setWidth(width);
//      camera.setHeight(height);
//      camera.setFocal(1.2 * std::max(width, height));
//      id_camera = mProject->addCamera(camera);
//    } else {
//      camera = mProject->findCamera(id_camera);
//    }
    msgWarning("Unknow camera for image: %s", image.toStdString().c_str());
    Camera camera2;
    bool bFound = false;
    //int id_camera;
    int counter = 0;
    for (auto it = mProject->cameraBegin(); it != mProject->cameraEnd(); it++) {
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
      id_camera = mProject->addCamera(camera);
      camera.setModel(QString::number(id_camera));
    }

  }

  return id_camera;
}



int CamerasModelImp::addCamera(const Camera &camera)
{
  int id = mProject->addCamera(camera);
//  QString database_path = mProject->database();
//  colmap::Database database_colmap(database_path.toStdString());
//  colmap::camera_t id_colmap = static_cast<colmap::camera_t>(id);
//  colmap::Camera camera_colmap;
//  if (database_colmap.ExistsCamera(id)){
//    camera_colmap = database_colmap.ReadCamera(id);
//  } else {
//    double focal_length = camera->focal();
//    if (camera->focal() > 0.) {
//      camera_colmap.SetPriorFocalLength(true);
//    } else {
//      focal_length = 1.2 * std::max(camera->width(), camera->height());
//      camera_colmap.SetPriorFocalLength(false);
//    }

//    int camera_model_id = colmap::CameraModelNameToId("SIMPLE_RADIAL");
//    camera_colmap.InitializeWithId(camera_model_id,
//                                   focal_length,
//                                   static_cast<size_t>(camera->width()),
//                                   static_cast<size_t>(camera->height()));
////    if (!camera_colmap.VerifyParams()) {
////      return 1;
////    }

//    camera_colmap.SetCameraId(id_colmap);
//    database_colmap.WriteCamera(camera_colmap);
//  }
  return id;
}

int CamerasModelImp::cameraID(const Camera &camera) const
{
  return cameraID(camera.make(), camera.model());
}

int CamerasModelImp::cameraID(const QString &make, const QString &model) const
{
  int id_camera = 0;
  for (auto it = begin(); it != end(); it++){
    QString camera_make = it->second.make();
    QString camera_model = it->second.model();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      id_camera = it->first;
      break;
    }
  }
  return id_camera;
}

Camera CamerasModelImp::camera(int id) const
{
  return mProject->findCamera(id);
}

Camera CamerasModelImp::camera(const QString &make, const QString &model) const
{
  int camera_id = cameraID(make, model);
  return camera(camera_id);
}

bool CamerasModelImp::updateCamera(int id, const Camera &camera)
{
  return mProject->updateCamera(id, camera);
}

bool CamerasModelImp::removeCamera(int id)
{
  return mProject->removeCamera(id);
}

bool CamerasModelImp::removeCamera(const Camera &camera)
{
  int id_camera = cameraID(camera.make(), camera.model());
  return mProject->removeCamera(id_camera);
}

CamerasModel::camera_iterator CamerasModelImp::begin()
{
  return mProject->cameraBegin();
}

CamerasModel::camera_const_iterator CamerasModelImp::begin() const
{
  return mProject->cameraBegin();
}

CamerasModel::camera_iterator CamerasModelImp::end()
{
  return mProject->cameraEnd();
}

CamerasModel::camera_const_iterator CamerasModelImp::end() const
{
  return mProject->cameraEnd();
}

void CamerasModelImp::init()
{

}

void CamerasModelImp::clear()
{
  for (auto it = begin(); it != end();){
    removeCamera(it->first);
    it = begin();
  }
}

} // namespace ui

} // namespace inspector


