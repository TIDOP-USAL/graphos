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


#include "ImageLoaderCommand.h"

#include "graphos/process/images/LoadImagesProcess.h"
#include "graphos/process/MultiProcess.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

#include <fstream>

namespace graphos
{
  
double parseFocal(const std::string &focal, double def)
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

ImageLoaderCommand::ImageLoaderCommand()
  : Command("image_manager", "Image manager"),
    mProjectFile(""),
    mImage(""),
    mImageList(""),
    mDelete(false),
    mCamera("")
{
  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentPathOptional("image", 'i', "Image added or removed (with option [--delete|-d]) from project.", &mImage));
  this->push_back(CreateArgumentPathOptional("image_list", 'l', "List of images added or removed (with option [--delete|-d]) from project.", &mImageList));
  this->push_back(CreateArgumentBooleanOptional("delete", 'd', "Delete an image in project", &mDelete));
  this->push_back(CreateArgumentStringOptional("camera", 'c', "Camera type", &mCamera));

  this->addExample("image_manager --p 253/253.xml -i image001.jpg");
  this->addExample("image_manager --p 253/253.xml -i image001.jpg -d");
}

ImageLoaderCommand::~ImageLoaderCommand()
{
}

bool ImageLoaderCommand::run()
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

    std::vector<QString> image_list;
    image_list.reserve(1000);

    if (!mImage.empty()) image_list.push_back(QString::fromStdWString(mImage.toWString()));

    if (!mImageList.empty() && mImageList.exists()) {

      std::ifstream ifs;
      ifs.open(mImageList.toWString(), std::ifstream::in);
      if (ifs.is_open()) {

        std::string line;
        while (std::getline(ifs, line)) {
          image_list.push_back(QString::fromStdString(line));
        }

        ifs.close();
      }

    }

    tl::Chrono chrono("Images loaded");
    chrono.run();

    //QStringList images;
    QSqlDatabase database_cameras = QSqlDatabase::addDatabase("QSQLITE");
    QString database_cameras_path;
#ifdef _DEBUG
    database_cameras_path = QString(GRAPHOS_SOURCE_PATH).append("/res");
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

    std::string epsg_out = "EPSG:326";
    std::shared_ptr<tl::geospatial::Crs> crs_in = std::make_shared<tl::geospatial::Crs>("EPSG:4326");
    std::shared_ptr<tl::geospatial::Crs> crs_out;
    if (!project.crs().isEmpty()) {
      crs_out = std::make_shared<tl::geospatial::Crs>(project.crs().toStdString());
      epsg_out = project.crs().toStdString();
    }

    for (auto &image : image_list) {

      try {

        Image img(image);

        msgInfo("Load image: %s", image.toStdString().c_str());

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

        if (bActiveCameraName && bActiveCameraModel) {

          if (project.existCamera(camera_make.c_str(), camera_model.c_str())) {

            camera_id = project.cameraId(camera_make.c_str(), camera_model.c_str());

          } else {

            msgInfo("New Camera detected: %s %s", camera_make.c_str(), camera_model.c_str());

            camera = Camera(camera_make.c_str(), camera_model.c_str());

            camera.setWidth(width);
            camera.setHeight(height);

            bool bActive = false;
            std::string focal = image_metadata->metadata("EXIF_FocalLength", bActive);
            if (bActive) {
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

            camera_id = project.addCamera(camera);
          }

        } else {

          /// Camara desconocida
          msgWarning("Unknow camera for image: %s", image.toStdString().c_str());
          Camera camera2;
          bool bFound = false;
          int counter = 0;
          for (auto it = project.cameraBegin(); it != project.cameraEnd(); it++) {
            camera2 = it->second;
            if (camera2.make().compare("Unknown camera") == 0) {
              if (camera2.width() == width && camera2.height() == height) {
                // Misma camara
                bFound = true;
                break;
              }
              counter++;
            }
          }

          if (bFound == false) {
            camera = Camera("Unknown camera", std::to_string(counter));
            camera.setWidth(width);
            camera.setHeight(height);
            camera.setFocal(1.2 * std::max(width, height));
            camera_id = project.addCamera(camera);
          } else {
            camera_id = project.camerasCount();
          }

        }

        tl::MessageManager::pause();

        double latitudeDecimalDegrees = 0.0;
        double longitudeDecimalDegrees = 0.0;
        double altitude = 0.0;

        bool bActive = false;
        std::string latitude = image_metadata->metadata("EXIF_GPSLatitude", bActive);
        std::string latitude_ref = image_metadata->metadata("EXIF_GPSLatitudeRef", bActive);
        if (bActive) {

          double degrees = TL_DOUBLE_MIN;
          double minutes = TL_DOUBLE_MIN;
          double seconds = TL_DOUBLE_MIN;

          size_t pos1 = latitude.find("(");
          size_t pos2 = latitude.find(")");
          if (pos1 != std::string::npos && pos2 != std::string::npos) {
            degrees = std::stod(latitude.substr(pos1 + 1, pos2 - pos1 + 1));
          }

          pos1 = latitude.find("(", pos2);
          pos2 = latitude.find(")", pos1);
          if (pos1 != std::string::npos && pos2 != std::string::npos) {
            minutes = std::stod(latitude.substr(pos1 + 1, pos2 - pos1 + 1));
          }

          pos1 = latitude.find("(", pos2);
          pos2 = latitude.find(")", pos1);
          if (pos1 != std::string::npos && pos2 != std::string::npos) {
            seconds = std::stod(latitude.substr(pos1 + 1, pos2 - pos1 + 1));
          }

          latitudeDecimalDegrees = tl::math::degreesToDecimalDegrees(degrees, minutes, seconds);
          if (latitude_ref.compare("S") == 0) latitudeDecimalDegrees = -latitudeDecimalDegrees;
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
            degrees = std::stod(longitude.substr(pos1 + 1, pos2 - pos1 + 1));
          }

          pos1 = longitude.find("(", pos2);
          pos2 = longitude.find(")", pos1);
          if (pos1 != std::string::npos && pos2 != std::string::npos) {
            minutes = std::stod(longitude.substr(pos1 + 1, pos2 - pos1 + 1));
          }

          pos1 = longitude.find("(", pos2);
          pos2 = longitude.find(")", pos1);
          if (pos1 != std::string::npos && pos2 != std::string::npos) {
            seconds = std::stod(longitude.substr(pos1 + 1, pos2 - pos1 + 1));
          }

          longitudeDecimalDegrees = tl::math::degreesToDecimalDegrees(degrees, minutes, seconds);
          if (longitude_ref.compare("W") == 0) longitudeDecimalDegrees = -longitudeDecimalDegrees;
        }

        std::string gps_altitude = image_metadata->metadata("EXIF_GPSAltitude", bActive);
        if (bActive) {
          size_t pos1 = gps_altitude.find("(");
          size_t pos2 = gps_altitude.find(")");

          if (pos1 != std::string::npos && pos2 != std::string::npos) {
            altitude = std::stod(gps_altitude.substr(pos1 + 1, pos2 - pos1 + 1));
          }
        }

        if (latitudeDecimalDegrees != 0.0 && longitudeDecimalDegrees != 0.0 && altitude != 0.0) {

          if (!crs_out) {
            int zone = tl::geospatial::utmZoneFromLongitude(longitudeDecimalDegrees);
            epsg_out.append(std::to_string(zone));
            crs_out = std::make_shared<tl::geospatial::Crs>(epsg_out);
          }

          bool bTrfCrs = crs_in->isValid() && crs_out->isValid();
          tl::geospatial::CrsTransform crs_trf(crs_in, crs_out);
          tl::Point3D pt_in(longitudeDecimalDegrees, latitudeDecimalDegrees, altitude);
          tl::Point3D pt_out = crs_trf.transform(pt_in);

          CameraPose camera_pose;
          camera_pose.setPosition(pt_out);
          camera_pose.setCrs(epsg_out.c_str());
          camera_pose.setSource("EXIF");
          img.setCameraPose(camera_pose);
        }

        tl::MessageManager::resume();

        img.setCameraId(camera_id);
        project.addImage(img);

        QString crs_proj = project.crs();
        QString crs_image = img.cameraPose().crs();
        if (crs_proj.isEmpty() && !crs_image.isEmpty()) {
          project.setCrs(crs_image);
        }

      } catch (std::exception& e) {
        msgError(e.what());
      }

    }

    chrono.stop();

    project.save(project_file);

  } catch (const std::exception& e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
    r = true;
  }

  return r;
}

} // namespace graphos