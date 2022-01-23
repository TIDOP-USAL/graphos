#include "LoadImagesProcess.h"

#include "graphos/core/camera/database.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>
#include <tidop/geospatial/camera.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

namespace graphos
{

LoadImagesProcess::LoadImagesProcess(std::vector<Image> *images, 
                                     std::vector<tl::Camera> *cameras,
                                     const QString &epsg)
  : mImages(images),
    mCameras(cameras),
    mEPSG(epsg),
    mDatabaseCameras(nullptr)
{
  QString database_cameras_path;
#ifdef _DEBUG
  database_cameras_path = QString(GRAPHOS_SOURCE_PATH).append("/res");
#else
  database_cameras_path = qApp->applicationDirPath();
#endif
  database_cameras_path.append("/cameras.db");

  mDatabaseCameras = new DatabaseCameras(database_cameras_path);
}

LoadImagesProcess::~LoadImagesProcess()
{
  if (mDatabaseCameras) {
    delete mDatabaseCameras;
    mDatabaseCameras = nullptr;
  }
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

    double latitudeDecimalDegrees = TL_DOUBLE_MIN;
    double longitudeDecimalDegrees = TL_DOUBLE_MIN;
    double altitude = TL_DOUBLE_MIN;

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

      if (degrees != TL_DOUBLE_MIN && 
          minutes != TL_DOUBLE_MIN && 
          seconds != TL_DOUBLE_MIN) {

        latitudeDecimalDegrees = tl::math::degreesToDecimalDegrees(degrees, minutes, seconds);
        if (latitude_ref.compare("S") == 0) latitudeDecimalDegrees = -latitudeDecimalDegrees;

      }
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

      if (degrees != TL_DOUBLE_MIN &&
          minutes != TL_DOUBLE_MIN &&
          seconds != TL_DOUBLE_MIN) {

        longitudeDecimalDegrees = tl::math::degreesToDecimalDegrees(degrees, minutes, seconds);
        if (longitude_ref.compare("W") == 0) longitudeDecimalDegrees = -longitudeDecimalDegrees;

      }

    }

    std::string gps_altitude = image_metadata->metadata("EXIF_GPSAltitude", bActive);

    if (bActive) {

      size_t pos1 = gps_altitude.find("(");
      size_t pos2 = gps_altitude.find(")");

      if (pos1 != std::string::npos && pos2 != std::string::npos) {
        altitude = std::stod(gps_altitude.substr(pos1 + 1, pos2 - pos1 + 1));
      }

    }

    if (latitudeDecimalDegrees != TL_DOUBLE_MIN &&
        longitudeDecimalDegrees != TL_DOUBLE_MIN && 
        altitude != TL_DOUBLE_MIN) {

      std::string epsg_out;
      if (!mCrsOut) {
        int zone = tl::geospatial::utmZoneFromLongitude(longitudeDecimalDegrees * tl::math::consts::deg_to_grad<double>);
        epsg_out = "EPSG:326"; 
        epsg_out.append(std::to_string(zone));
        mCrsOut = std::make_shared<tl::geospatial::Crs>(epsg_out);
      } else {
        epsg_out = mEPSG.toStdString();
      }

      bool bTrfCrs = mCrsIn->isValid() && mCrsOut->isValid();
      tl::geospatial::CrsTransform crs_trf(mCrsIn, mCrsOut);
      tl::Point3D pt_in(latitudeDecimalDegrees, longitudeDecimalDegrees, altitude);
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
    tl::Camera camera2;
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

  tl::Camera camera(camera_make, camera_model);
  camera.setWidth(width);
  camera.setHeight(height);

  /// Extract sensor size
  double sensor_width_mm = -1.;

  if (mDatabaseCameras->isOpen() &&
      mDatabaseCameras->existCameraMakeId(camera_make.c_str())) {
    int camera_make_id = mDatabaseCameras->cameraMakeId(camera_make.c_str());

    if (mDatabaseCameras->existCameraModel(camera_make_id, camera_model.c_str())) {
      sensor_width_mm = mDatabaseCameras->cameraSensorSize(camera_make_id, camera_model.c_str());
    }
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

void graphos::LoadImagesProcess::run()
{
  tl::Chrono chrono("Images loaded");
  chrono.run();


  mDatabaseCameras->open();

  mCrsIn = std::make_shared<tl::geospatial::Crs>("EPSG:4326");
  std::shared_ptr<tl::geospatial::Crs> crs_out;
  if (!mEPSG.isEmpty()) {
    mCrsOut = std::make_shared<tl::geospatial::Crs>(mEPSG.toStdString());
  }

  for (size_t i = 0; i < mImages->size(); i++){

    loadImage(i);

    emit statusChangedNext();

  }

  mDatabaseCameras->close();

  chrono.stop();

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
