/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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

#include "ImageLoaderProcess.h"

#include "graphos/core/utils.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Database.h"

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

namespace graphos
{

LoadImagesProcess::LoadImagesProcess(std::vector<Image> *images, 
                                     std::vector<Camera> *cameras,
                                     const QString &epsg)
  : tl::ProcessBase(), 
    mImages(images),
    mCameras(cameras),
    mEPSG(epsg)/*,
    mDatabaseCameras(nullptr)*/
{
#ifdef _DEBUG
  mDatabaseCamerasPath = QString(GRAPHOS_SOURCE_PATH).append("/res");
#else
  mDatabaseCamerasPath = qApp->applicationDirPath();
#endif
  mDatabaseCamerasPath.append("/cameras.db");
  //mDatabaseCameras = new DatabaseCameras(database_cameras_path);
}

LoadImagesProcess::~LoadImagesProcess()
{
  //if (mDatabaseCameras) {
  //  delete mDatabaseCameras;
  //  mDatabaseCameras = nullptr;
  //}
}

bool LoadImagesProcess::existCamera(const QString &make, const QString &model) const
{
  bool camera_exist = false;
  
  for (auto it = mCameras->begin(); it != mCameras->end(); it++) {
    QString camera_make = it->make().c_str();
    QString camera_model = it->model().c_str();
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
    QString camera_make = (*mCameras)[i].make().c_str();
    QString camera_model = (*mCameras)[i].model().c_str();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      camera_id = i;
      break;
    }
  }
  return camera_id;
}

void LoadImagesProcess::loadImage(size_t imageId)
{
  try {

    QString image = (*mImages)[imageId].path();

    msgInfo("Load image: %s", image.toStdString().c_str());

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.toStdString());
    imageReader->open();
    if (!imageReader->isOpen()) throw std::runtime_error("  Failed to read image file");

    int id_camera = 0;
    int width = imageReader->cols();
    int height = imageReader->rows();
    int camera_id = -1;

    tl::MessageManager::pause();
    std::shared_ptr<tl::ImageMetadata> image_metadata = imageReader->metadata();
    bool bActiveCameraName = false;
    bool bActiveCameraModel = false;
    std::string camera_make = image_metadata->metadata("EXIF_Make", bActiveCameraName);
    std::string camera_model = image_metadata->metadata("EXIF_Model", bActiveCameraModel);
    tl::MessageManager::resume();

    msgInfo(" - Camera: %s %s", camera_make.c_str(), camera_model.c_str());

    if (existCamera(camera_make.c_str(), camera_model.c_str())) {

      camera_id = findCamera(camera_make.c_str(), camera_model.c_str());

    } else {

      camera_id = loadCamera(imageReader.get());

    }

    tl::MessageManager::pause();

    tl::math::Degrees<double> latitude_degrees{};
    tl::math::Degrees<double> longitude_degrees{};
    double altitude{};

    bool latitude_active = false;

    std::string latitude = image_metadata->metadata("EXIF_GPSLatitude", latitude_active);
    if (latitude_active) {
      std::string latitude_ref = image_metadata->metadata("EXIF_GPSLatitudeRef", latitude_active);
      latitude_degrees = formatDegreesFromExif(latitude, latitude_ref);
    }

    bool longitude_active = false;

    std::string longitude = image_metadata->metadata("EXIF_GPSLongitude", longitude_active);
    if (longitude_active) {
      std::string longitude_ref = image_metadata->metadata("EXIF_GPSLongitudeRef", longitude_active);
      longitude_degrees = formatDegreesFromExif(longitude, longitude_ref);
    }

    bool altitude_active = false;

    std::string gps_altitude = image_metadata->metadata("EXIF_GPSAltitude", altitude_active);

    if (altitude_active) {

      size_t pos1 = gps_altitude.find("(");
      size_t pos2 = gps_altitude.find(")");

      if (pos1 != std::string::npos && pos2 != std::string::npos) {
        altitude = std::stod(gps_altitude.substr(pos1 + 1, pos2 - pos1 + 1));
      }

    }

    if (latitude_active && longitude_active && altitude_active) {

      std::string epsg_out;
      if (!mCrsOut) {
        int zone = tl::geospatial::utmZoneFromLongitude(longitude_degrees.value());
        epsg_out = "EPSG:326"; 
        epsg_out.append(std::to_string(zone));
        mCrsOut = std::make_shared<tl::geospatial::Crs>(epsg_out);
      } else {
        epsg_out = mEPSG.toStdString();
      }

      bool bTrfCrs = mCrsIn->isValid() && mCrsOut->isValid();
      tl::geospatial::CrsTransform crs_trf(mCrsIn, mCrsOut);
      tl::Point3D pt_in(longitude_degrees.value(), latitude_degrees.value(), altitude);
      tl::Point3D pt_out = crs_trf.transform(pt_in);

      CameraPose camera_pose;
      camera_pose.setPosition(pt_out);
      camera_pose.setCrs(epsg_out.c_str());
      camera_pose.setSource("EXIF");
      (*mImages)[imageId].setCameraPose(camera_pose);
    }

    tl::MessageManager::resume();

    emit imageAdded(imageId, camera_id);

  } catch (std::exception &e) {
    msgError(e.what());
  }
}

int LoadImagesProcess::loadCamera(tl::ImageReader *imageReader)
{

  int width = imageReader->cols();
  int height = imageReader->rows();

  tl::MessageManager::pause();
  std::shared_ptr<tl::ImageMetadata> image_metadata = imageReader->metadata();
  bool bActiveCameraName = false;
  bool bActiveCameraModel = false;
  std::string camera_make = image_metadata->metadata("EXIF_Make", bActiveCameraName);
  std::string camera_model = image_metadata->metadata("EXIF_Model", bActiveCameraModel);
  tl::MessageManager::resume();

  if (!bActiveCameraName && !bActiveCameraModel) {
    Camera camera2;
    int counter = 0;
    for (auto it = mCameras->begin(); it != mCameras->end(); it++) {
      camera2 = *it;
      if (camera2.make().compare("Unknown camera") == 0) {
        if (camera2.width() == width && camera2.height() == height) {
          return counter;
        }
        counter++;
      }
    }
    
    camera_make = "Unknown camera";
    camera_model = std::to_string(counter);
  }

  Camera camera(camera_make, camera_model);
  camera.setWidth(width);
  camera.setHeight(height);

  /// Extract sensor size
  double sensor_width_mm = -1.;
  DatabaseCameras databaseCameras(mDatabaseCamerasPath);
  databaseCameras.open();

  if (databaseCameras.isOpen()) {

    if (databaseCameras.existCameraMakeId(camera_make.c_str())) {
      int camera_make_id = databaseCameras.cameraMakeId(camera_make.c_str());

      if (databaseCameras.existCameraModel(camera_make_id, camera_model.c_str())) {
        sensor_width_mm = databaseCameras.cameraSensorSize(camera_make_id, camera_model.c_str());
      }
    }

    databaseCameras.close();
  }

  /// Extract focal

  double focal = -1.;

  if (bActiveCameraName && bActiveCameraModel) {
    bool bActive = false;
    int max_size = std::max(width, height);

    std::string focal_length_in_35_mm_film = image_metadata->metadata("EXIF_FocalLengthIn35mmFilm", bActive);

    if (bActive) {

      double focal_35mm = parseFocal(focal_length_in_35_mm_film, focal);
      focal = focal_35mm / 35.0 * max_size;

    }

    if (!bActive || focal < 0.) {

      std::string focal_length = image_metadata->metadata("EXIF_FocalLength", bActive);
      if (bActive ) {

        double focal_mm = parseFocal(focal_length, focal);
        
        if (sensor_width_mm > 0.) {
          
          focal = focal_mm / sensor_width_mm * max_size;

        } else {

          std::string exif_pixel_x_dimension = image_metadata->metadata("EXIF_PixelXDimension", bActive);
          if (bActive) {
            
            double pixel_x_dimension = std::stod(exif_pixel_x_dimension);
            
            double focal_plane_x_resolution = 0.;
            std::string exif_focal_plane_x_resolution = image_metadata->metadata("EXIF_FocalPlaneXResolution", bActive);
            if (bActive) {
              size_t pos1 = exif_focal_plane_x_resolution.find("(");
              size_t pos2 = exif_focal_plane_x_resolution.find(")");
              if (pos1 != std::string::npos && pos2 != std::string::npos) {
                focal_plane_x_resolution = std::stod(exif_focal_plane_x_resolution.substr(pos1 + 1, pos2 - pos1 + 1));
              }
            }
            
            std::string exif_focal_plane_resolution_unit = image_metadata->metadata("EXIF_FocalPlaneResolutionUnit", bActive);
            if (bActive) {
              
              if (exif_focal_plane_resolution_unit == "2") { // 2 = Inch.
                sensor_width_mm = pixel_x_dimension * 25.4 / focal_plane_x_resolution;
                focal = focal_mm / sensor_width_mm * max_size;
              } else if (exif_focal_plane_resolution_unit == "3") { //3 = Centimeter
                sensor_width_mm = pixel_x_dimension * 10 / focal_plane_x_resolution;
                focal_length = focal_mm / sensor_width_mm * max_size;
              }
            }
          }
        }

      }
    }
  }

  if (focal < 0.) {

    focal = 0.;
  }

  camera.setFocal(focal);
  if (sensor_width_mm > 0.) 
    camera.setSensorSize(sensor_width_mm);

  int camera_id = mCameras->size();
  mCameras->push_back(camera);

  return camera_id;
}

void graphos::LoadImagesProcess::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Images loaded");
    chrono.run();

    mCrsIn = std::make_shared<tl::geospatial::Crs>("EPSG:4326");
    std::shared_ptr<tl::geospatial::Crs> crs_out;
    if(!mEPSG.isEmpty()) {
      mCrsOut = std::make_shared<tl::geospatial::Crs>(mEPSG.toStdString());
    }

    for(size_t i = 0; i < mImages->size(); i++) {

      loadImage(i);

      //emit statusChangedNext();
      if(progressBar) (*progressBar)();

    }

    chrono.stop();

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Feature Extractor error");
  }

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

} // graphos
