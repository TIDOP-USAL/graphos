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

#include "ImageLoaderTask.h"

#include "graphos/core/utils.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Database.h"
#include "graphos/core/multispectral/Vignetting.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/chrono.h>
#include <tidop/core/progress.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>
#include <tidop/math/algebra/matrix.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>

#include <QFileInfo>

namespace graphos
{

static bool isLikelyFisheye(double focal_mm, double sensor_width_mm)
{
    if (focal_mm <= 0.0 || sensor_width_mm <= 0.0)
        return false;

    double fov_rad = 2.0 * std::atan(sensor_width_mm / (2.0 * focal_mm));
    double fov_deg = fov_rad * tl::consts::rad_to_deg<double>;

    return fov_deg > 150.0; // Umbral aproximado para fisheye
}

LoadImagesTask::LoadImagesTask(std::vector<Image> *images,
                               std::vector<Camera> *cameras,
                               std::string cameraType/*,
                               QString epsg*/)
  : tl::TaskBase(),
    mImages(images),
    mCameras(cameras),
    //mEPSG(std::move(epsg)),
    mCameraType(std::move(cameraType))
{
//#ifdef _DEBUG
//    mDatabaseCamerasPath = QString(GRAPHOS_SOURCE_PATH).append("/res");
//#else
    mDatabaseCamerasPath = qApp->applicationDirPath();
//#endif
    mDatabaseCamerasPath.append("/cameras.db");
}

LoadImagesTask::~LoadImagesTask()
{

}

auto LoadImagesTask::existCamera(const QString &make,
                                 const QString &model, 
                                 const QString &serialNumber,
                                 const QString &bandName) const -> bool
{
    bool camera_exist = false;


    for (const auto &camera : *mCameras) {

        if (make.toStdString() == camera.make() &&
            model.toStdString() == camera.model() &&
            serialNumber.toStdString() == camera.serialNumber() &&
            bandName.toStdString() == camera.bandName()) {
            camera_exist = true;
            break;
        }

    }

    return camera_exist;
}

auto LoadImagesTask::findCamera(const QString &make, 
                                const QString &model, 
                                const QString &serialNumber, 
                                const QString &bandName) const -> int
{
    int camera_id = -1;

    for (size_t i = 0; i < mCameras->size(); i++) {

        if (make.toStdString() == (*mCameras)[i].make() &&
            model.toStdString() == (*mCameras)[i].model() &&
            serialNumber.toStdString() == (*mCameras)[i].serialNumber() &&
            bandName.toStdString() == (*mCameras)[i].bandName()) {
            camera_id = static_cast<int>(i);
            break;
        }

    }

    return camera_id;
}

void LoadImagesTask::loadImage(size_t imagePosition)
{
    try {

        QString image = mImages->at(imagePosition).path();

        auto image_reader = tl::ImageReaderFactory::create(image.toStdString());
        image_reader->open();
        if (!image_reader->isOpen()) throw std::runtime_error("  Failed to read image file");

        int camera_position = -1;

        camera_position = loadCamera(image_reader.get());

        //tl::Message::pauseMessages();
        auto image_metadata = image_reader->metadata();
        //bool bActiveCameraName = false;
        //bool bActiveCameraModel = false;
        //std::string camera_make = image_metadata->metadata("EXIF_Make", bActiveCameraName);
        //std::string camera_model = image_metadata->metadata("EXIF_Model", bActiveCameraModel);
        //tl::Message::resumeMessages();

        //camera_id = findCamera(camera_make.c_str(), camera_model.c_str());
        //if (camera_id == -1) {
        //    camera_id = loadCamera(image_reader.get());
        //}

        tl::Message::pauseMessages();

        tl::Degrees<double> latitude_degrees{};
        tl::Degrees<double> longitude_degrees{};
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

            //size_t pos1 = gps_altitude.find('(');
            //size_t pos2 = gps_altitude.find(')');

            //if (pos1 != std::string::npos && pos2 != std::string::npos) {
            //    altitude = tl::stringToNumber<double>(gps_altitude.substr(pos1 + 1, pos2 - pos1 + 1));

            //    bool active;
            //    auto value = image_metadata->metadata("EXIF_GPSAltitudeRef", active);
            //    if (active) {
            //        if ("0x01" == value) altitude = -altitude;
            //    }
            //}
            altitude = tl::stringToNumber<double>(gps_altitude);

            bool active;
            auto value = image_metadata->metadata("EXIF_GPSAltitudeRef", active);
            if (active) {
                if ("0x01" == value) altitude = -altitude;
            }
        }

        if (latitude_active && longitude_active && altitude_active) {

            tl::Point3<double> pt(longitude_degrees.value(), latitude_degrees.value(), altitude);

            CameraPose camera_pose;
            camera_pose.setPosition(pt);
            camera_pose.setCrs("EPSG:4326");
            camera_pose.setSource("EXIF");

            bool active = false;
            std::string rtk_flag = image_metadata->metadata("XMP_DJI_RtkFlag", active);
            if (active) {
                camera_pose.setRtkFlag(tl::stringToNumber<int>(rtk_flag));
            }

            tl::Vector3d accuracy;

            bool active_std_lon = false;
            std::string rtk_std_lon = image_metadata->metadata("XMP_DJI_RtkStdLon", active_std_lon);
            if (active_std_lon) {
                accuracy[0] = tl::stringToNumber<double>(rtk_std_lon);
            }

            bool active_std_lat = false;
            std::string rtk_std_lat = image_metadata->metadata("XMP_DJI_RtkStdLat", active_std_lat);
            if (active_std_lat) {
                accuracy[1] = tl::stringToNumber<double>(rtk_std_lat);
            }

            bool active_std_hgt = false;
            std::string rtk_std_hgt = image_metadata->metadata("XMP_DJI_RtkStdHgt", active_std_hgt);
            if (active_std_hgt) {
                accuracy[2] = tl::stringToNumber<double>(rtk_std_hgt);
            }

            if (!(active_std_lon && active_std_lat && active_std_hgt)) {

                // No viene RtkFlag, intentamos estimarlo a partir de GPSXY/Z Accuracy
                bool active_xy = false, active_z = false;
                std::string gps_xy_str = image_metadata->metadata("XMP_CAMERA_GPSXYAccuracy", active_xy);
                std::string gps_z_str = image_metadata->metadata("XMP_CAMERA_GPSZAccuracy", active_z);

                double gps_xy = active_xy ? tl::stringToNumber<double>(gps_xy_str) : -1.0;
                double gps_z = active_z ? tl::stringToNumber<double>(gps_z_str) : -1.0;

                if (gps_xy > 0.0 && gps_z > 0.0) {
                    accuracy[0] = gps_xy;
                    accuracy[1] = gps_xy;
                    accuracy[2] = gps_z;

                    if (gps_xy < 0.10 && gps_z < 0.15) {
                        camera_pose.setRtkFlag(50);
                    } else if (gps_xy < 0.50 && gps_z < 1.00) {
                        camera_pose.setRtkFlag(34);
                    } else {
                        camera_pose.setRtkFlag(16);
                    }
                }
                
            }

            if (accuracy[0] > 0.0 || accuracy[1] > 0.0 || accuracy[2] > 0.0) {
                camera_pose.setAccuracy(accuracy);
            }

            mImages->at(imagePosition).setCameraPose(camera_pose);

        }

        // Image metadata
        bool active_metadata = false;

        // DJI M3M -> XMP_DJI_CaptureUUID
        // Parrot Sequoia -> XMP_CAMERA_CaptureUUID
        std::vector<std::string> capture_uuid = {"XMP_DJI_CaptureUUID", "XMP_CAMERA_CaptureUUID"};

        std::string value = image_metadata->metadata(capture_uuid, active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("CaptureUUID", value);

        // Radiometric Calibration

        value = image_metadata->metadata("XMP_RadiometricCalibration", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("RadiometricCalibration", value);

        // Exposure Time
        value = image_metadata->metadata("EXIF_ExposureTime", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("ExposureTime", value);

        value = image_metadata->metadata("XMP_DJI_SensorGain", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("SensorGain", value);

        value = image_metadata->metadata("XMP_DJI_SensorGainAdjustment", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("SensorGainAdjustment", value);

        // Sun sensor

        value = image_metadata->metadata("XMP_CAMERA_SunSensor", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("SunSensor", value);

        value = image_metadata->metadata("XMP_CAMERA_SunSensorExposureTime", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("SunSensorExposureTime", value);

        value = image_metadata->metadata("XMP_CAMERA_SunSensorPitch", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("SunSensorPitch", value);

        value = image_metadata->metadata("XMP_CAMERA_SunSensorRoll", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("SunSensorRoll", value);

        value = image_metadata->metadata("XMP_CAMERA_SunSensorYaw", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("SunSensorYaw", value);

        // Irradiance
        
        value = image_metadata->metadata("XMP_CAMERA_Irradiance", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("Irradiance", value);

        value = image_metadata->metadata("XMP_CAMERA_IrradianceExposureTime", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("IrradianceExposureTime", value);

        value = image_metadata->metadata("XMP_CAMERA_IrradianceGain", active_metadata);
        if (active_metadata)
            mImages->at(imagePosition).addMetadata("IrradianceGain", value);

        tl::Message::resumeMessages();

        emit imageAdded(static_cast<int>(imagePosition), camera_position);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

int LoadImagesTask::loadCamera(tl::ImageReader *imageReader)
{
    int camera_position;

    try {

        tl::Message::pauseMessages();

        auto image_metadata = imageReader->metadata();
        bool active_camera_name = false;
        bool active_camera_model = false;
        std::vector<std::string> make_strings = {"EXIF_Make", "XMP_TIFF_Make"};
        std::string camera_make = image_metadata->metadata(make_strings, active_camera_name);
        std::vector<std::string> model_strings = {"EXIF_Model", "XMP_CAMERA_RigName", "XMP_TIFF_Model"};
        std::string camera_model = image_metadata->metadata(model_strings, active_camera_model);
        if (!active_camera_model) {
            camera_model = image_metadata->metadata("XMP_CAMERA_RigName", active_camera_model);
        }
        bool active_serial_number = false;
        std::vector<std::string> serial_number_strings = {"XMP_DJI_CameraSerialNumber", "EXIF_SerialNumber", "EXIF_BodySerialNumber"};
        std::string serial_number = image_metadata->metadata(serial_number_strings, active_serial_number);

        bool active_band_name = false;
        std::string band_name = image_metadata->metadata("XMP_CAMERA_BandName", active_band_name);
        if (!active_band_name) {
            if (camera_make == "DJI" && camera_model == "M3M")
                band_name = "RGB";
            if (camera_make == "DJI" && camera_model == "M4T")
                band_name = "THERMAL";
            else if (imageReader->channels() >= 3)
                band_name = "RGB";
            else
                band_name = "Gray";
        } else {
            if (camera_make == "Parrot" && camera_model == "Sequoia" && band_name == "Red, Green, Blue")
                band_name = "RGB";
        }

        tl::Message::resumeMessages();

        // No es suficiente porque en el caso de Parrot Sequoia todas las cámaras tienen el mismo número de serie
        // Hay que comprobar también el RigCameraIndex y el RigName para multiespectral
        camera_position = findCamera(QString::fromStdString(camera_make),
                                     QString::fromStdString(camera_model),
                                     QString::fromStdString(serial_number),
                                     QString::fromStdString(band_name));
        if (camera_position != -1) return camera_position;

        int width = imageReader->cols();
        int height = imageReader->rows();

        if (!active_camera_name && !active_camera_model) {


            int counter = 0;
            for (auto &camera : *mCameras) {
                if (camera.make() == "Unknown camera") {
                    if (camera.width() == width && camera.height() == height) {
                        return counter;
                    }
                    counter++;
                }
            }

            camera_make = "Unknown camera";
            camera_model = std::to_string(counter);
        }

        tl::Message::info("New camera detected: {} {}", camera_make, camera_model);

        Camera camera(camera_make, camera_model, serial_number);
        camera.setWidth(width);
        camera.setHeight(height);
        camera.setBitsPerPixel(imageReader->depth());
        camera.setType(mCameraType);
        camera.setBandName(band_name);
        /// Extract sensor size
        double sensor_width_mm = -1.;
        DatabaseCameras database_cameras(mDatabaseCamerasPath);
        database_cameras.open();

        if (database_cameras.isOpen()) {

            if (database_cameras.existCameraMakeId(QString::fromStdString(camera_make))) {
                int camera_make_id = database_cameras.cameraMakeId(QString::fromStdString(camera_make));

                if (database_cameras.existCameraModel(camera_make_id, QString::fromStdString(camera_model))) {
                    sensor_width_mm = database_cameras.cameraSensorSize(camera_make_id, QString::fromStdString(camera_model));
                }
            }

            database_cameras.close();
        }

        /// Extract focal

        double focal = -1.;

        bool bActive = false;
        int max_size = std::max(width, height);

        std::string focal_length_in_35_mm_film = image_metadata->metadata("EXIF_FocalLengthIn35mmFilm", bActive);

        if (bActive) {

            double focal_35mm = parseFocal(focal_length_in_35_mm_film, focal);
            focal = focal_35mm / 35.0 * max_size;

        }

        if (!bActive || focal < 0.) {

            std::string focal_length = image_metadata->metadata("EXIF_FocalLength", bActive);
            if (bActive) {

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
                            focal_plane_x_resolution = tl::convertStringTo<double>(exif_focal_plane_x_resolution);
                        }

                        std::string exif_focal_plane_resolution_unit = image_metadata->metadata("EXIF_FocalPlaneResolutionUnit", bActive);
                        if (bActive) {

                            if (exif_focal_plane_resolution_unit == "2") { // 2 = Inch.
                                sensor_width_mm = pixel_x_dimension * 25.4 / focal_plane_x_resolution;
                                focal = focal_mm / sensor_width_mm * max_size;
                            } else if (exif_focal_plane_resolution_unit == "3") { //3 = Centimeter
                                sensor_width_mm = pixel_x_dimension * 10 / focal_plane_x_resolution;
                                focal = focal_mm / sensor_width_mm * max_size;
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

        if (sensor_width_mm > 0.) {
            double focal_mm_estimate = focal * sensor_width_mm / std::max(width, height);
            if (isLikelyFisheye(focal_mm_estimate, sensor_width_mm)) {
                camera.setType("OpenCV Fisheye");
            }
        }

        // Calibration

        bool active_dewarped;
        std::string dewarp_flag = image_metadata->metadata("XMP_DJI_DewarpFlag", active_dewarped);
        if (active_dewarped) {

            bool active_dewarp_data;
            std::string dewarp_data = image_metadata->metadata("XMP_DJI_DewarpData", active_dewarp_data);
            if (active_dewarp_data) {

                auto v = tl::split<std::string>(dewarp_data, ';');
                if (v.size() == 2) {
                    //Fecha -> v[0];
                    auto params = tl::split<double>(v[1], ',');
                    if (params.size() == 9 && params[0] > 0.) {
                        std::shared_ptr<Calibration> calibration;
                        //if (params[8] == 0)
                        if (dewarp_flag == "0") {
                            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
                            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
                            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
                            calibration->setParameter(Calibration::Parameters::focal, (params[0] + params[1]) / 2.0);
                            calibration->setParameter(Calibration::Parameters::cx, params[2] + static_cast<double>(camera.width()) / 2.);
                            calibration->setParameter(Calibration::Parameters::cy, params[3] + static_cast<double>(camera.height()) / 2.);
                            calibration->setParameter(Calibration::Parameters::k1, params[4]);
                            calibration->setParameter(Calibration::Parameters::k2, params[5]);
                            calibration->setParameter(Calibration::Parameters::p1, params[6]);
                            calibration->setParameter(Calibration::Parameters::p2, params[7]);
                            camera.setType("OpenCV 1");
                        } else if (dewarp_flag == "1") {
                            calibration = CalibrationFactory::create(Calibration::CameraModel::pinhole);
                            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
                            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
                            calibration->setParameter(Calibration::Parameters::focal, (params[0] + params[1]) / 2.0);
                            calibration->setParameter(Calibration::Parameters::cx, params[2] + static_cast<double>(camera.width()) / 2.);
                            calibration->setParameter(Calibration::Parameters::cy, params[3] + static_cast<double>(camera.height()) / 2.);
                            camera.setType("Pinhole 2");
                        }

                        if (calibration) camera.setPriorCalibration(calibration);
                    }
                }
            }
        }

        // Black Level

        bool active_metadata = false;
        std::string black_level = image_metadata->metadata(std::vector<std::string>{"XMP_CAMERA_BlackCurrent", "XMP_DJI_BlackLevel"}, active_metadata);
        if (active_metadata)
            camera.setBlackLevel(tl::stringToNumber<uint16_t>(black_level));

        // Coeficientes de Viñeteo
        //XMP_DJI_CalibratedOpticalCenterX: 1296.000000
        //XMP_DJI_CalibratedOpticalCenterY : 972.000000
        //XMP_DJI_VignettingData: -0.000105175, 1.530238e-06, -4.671235e-09, 7.899883e-12, -5.874507e-15, 1.637752e-18
        //XMP_DJI_VignettingFlag : 0
        // Según la documentación de DJI hay que usar Calibrated Optical CenterX y Calibrated Optical Center Y pero 
        // los valores que dan son la mitad de las dimensiones
        //XMP_CAMERA_VignettingCenter: 1215.959106, 946.467468
        // Igual que XMP_DJI_VignettingData
        //XMP_CAMERA_VignettingPolynomial: -1.051753e-04, 1.530238e-06, -4.671235e-09, 7.899883e-12, -5.874507e-15, 1.637752e-18
        std::string vignetting_flag = image_metadata->metadata("XMP_DJI_VignettingFlag", active_metadata);
        if (active_metadata && vignetting_flag == "0") {
            std::string vignetting_str = image_metadata->metadata(std::vector<std::string>{"XMP_DJI_VignettingData", "XMP_CAMERA_VignettingPolynomial"}, active_metadata);
            if (active_metadata) {

                //bool active_cx;
                //bool active_cy;
                //std::string cx = image_metadata->metadata("XMP_DJI_CalibratedOpticalCenterX", active_cx);
                //std::string cy = image_metadata->metadata("XMP_DJI_CalibratedOpticalCenterY", active_cy);
                //if (active_cx && active_cy) {
                //    auto vignetin_data = tl::split<float>(vignetting_str);
                //    tl::Point2d calibrated_optical_center(tl::stringToNumber<float>(cx), tl::stringToNumber<float>(cy));
                //    camera.setVignettingCenter(calibrated_optical_center);
                //    camera.setVignettingPolynomial(vignetin_data);
                //}

                std::string vignetting_center = image_metadata->metadata("XMP_CAMERA_VignettingCenter", active_metadata);
                if (active_metadata) {
                    auto vignetting_data = tl::split<float>(vignetting_str);
                    auto vignetting_center_values = tl::split<float>(vignetting_center);
                    if (vignetting_center_values.size() == 2) {
                        tl::Point2d vignetting_center_point(vignetting_center_values[0], vignetting_center_values[1]);
                        //camera.setVignettingCenter(vignetting_center_point);
                        //camera.setVignettingPolynomial(vignetting_data);
                        auto model = std::make_shared<VignettingRadial>(vignetting_center_point, vignetting_data);
                        camera.setVignettingModel(model);
                    }
                }
            }
        }

        // XMP_CAMERA_VignettingPolynomial2D
        std::string vignetting_polynomial2d = image_metadata->metadata("XMP_CAMERA_VignettingPolynomial2D", active_metadata);
        if (active_metadata) {
            std::string vignetting_polynomial2d_name = image_metadata->metadata("XMP_CAMERA_VignettingPolynomial2DName", active_metadata);
            if (active_metadata) {
                
                auto coeffs = tl::split<float>(vignetting_polynomial2d);
                auto powers = tl::split<float>(vignetting_polynomial2d_name);
                std::vector<std::pair<int, int>> power_pairs(powers.size()/2);
                for (size_t i = 0; i < powers.size(); i += 2) {
                    power_pairs[i / 2] = std::make_pair(static_cast<int>(powers[i]), static_cast<int>(powers[i + 1]));
                }
                auto model = std::make_shared<VignettingPolynomial2D>(coeffs, power_pairs);
                camera.setVignettingModel(model);
            }
        }

        // XMP_DJI_CalibratedHMatrix
        std::string calibrated_hmatrix = image_metadata->metadata("XMP_DJI_CalibratedHMatrix", active_metadata);
        if (active_metadata) {
            auto hmatrix_values = tl::split<float>(calibrated_hmatrix);
            if (hmatrix_values.size() == 9) {
                tl::Matrix3x3f hmatrix{hmatrix_values[0],
                                       hmatrix_values[1],
                                       hmatrix_values[2],
                                       hmatrix_values[3],
                                       hmatrix_values[4],
                                       hmatrix_values[5],
                                       hmatrix_values[6],
                                       hmatrix_values[7],
                                       hmatrix_values[8]};
                camera.setCalibratedHMatrix(hmatrix);
            }
        }

        // GainAdjustment
        // Gain
        // SunSensor
        // 


        camera_position = static_cast<int>(mCameras->size());
        mCameras->push_back(camera);

    } catch (...){
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return camera_position;
}

void LoadImagesTask::execute(tl::Progress *progressBar)
{
    try {

        tl::Chrono chrono;
        chrono.run();

        //mCrsIn = std::make_shared<tl::Crs>("EPSG:4326");
        //std::shared_ptr<tl::Crs> crs_out;
        //if (!mEPSG.isEmpty()) {
        //    mCrsOut = std::make_shared<tl::Crs>(mEPSG.toStdString());
        //}

        for (size_t i = 0; i < mImages->size(); i++) {

            if (status() == Status::stopping)  break;

            loadImage(i);

            if (progressBar) (*progressBar)();

        }

        if (status() == Status::stopping) {
            chrono.reset();
        } else {
            
            tl::Message::success("{} images loaded in {:.2} minutes", mImages->size(), chrono.stop()/60.);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Load images error");
    }

}

double LoadImagesTask::parseFocal(const std::string &focal, double def)
{
    double r_focal;

    try {

        std::string f = focal;

        size_t pos = f.find('(');
        if (pos != std::string::npos) {
            f.erase(pos, 1);
        }

        pos = f.find(')');
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
