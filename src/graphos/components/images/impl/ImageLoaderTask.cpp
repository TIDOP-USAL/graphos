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

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Chrono.h>
#include <tidop/core/task/Progress.h>
#include <tidop/core/base/Split.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/rastertools/io/Metadata.h>
#include <tidop/math/algebra/matrix/Matrix.h>
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
                               std::string cameraType)
  : tl::Task(),
    mImages(images),
    mCameras(cameras),
    mCameraType(std::move(cameraType))
{
    mDatabaseCamerasPath = qApp->applicationDirPath();
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

        tl::RasterReader image_reader(image.toStdString());
        if (!image_reader.isOpen()) throw std::runtime_error("  Failed to read image file");

        int camera_position = loadCamera(image_reader);

        auto image_metadata = image_reader.metadata();

        tl::Message::pauseMessages();

        tl::Degrees<double> latitude{};
        tl::Degrees<double> longitude{};
        double altitude{};

        auto exif_latitude = image_metadata.value("EXIF_GPSLatitude");
        if (exif_latitude) {
            auto exif_latitude_ref = image_metadata.value("EXIF_GPSLatitudeRef");
            latitude = formatDegreesFromExif(exif_latitude.value(), exif_latitude_ref.value_or(""));
        }

        auto exif_longitude = image_metadata.value("EXIF_GPSLongitude");
        if (exif_longitude) {
            auto exif_longitude_ref = image_metadata.value("EXIF_GPSLongitudeRef");
            longitude = formatDegreesFromExif(exif_longitude.value(), exif_longitude_ref.value_or(""));
        }

        auto exif_altitude = image_metadata.value("EXIF_GPSAltitude");
        if (exif_altitude.has_value()) {

            altitude = tl::convertStringTo<double>(exif_altitude.value());

            if (auto exif_altitude_ref = image_metadata.value("EXIF_GPSAltitudeRef")) {
                if ("0x01" == exif_altitude_ref) altitude = -altitude;
            }
        }

        if (exif_latitude.has_value() && exif_longitude.has_value() && exif_altitude.has_value()) {

            tl::Point3d pt(longitude.value(), latitude.value(), altitude);

            CameraPose camera_pose;
            camera_pose.setPosition(pt);
            camera_pose.setCrs("EPSG:4326");
            camera_pose.setSource("EXIF");

            if (auto rtk_flag = image_metadata.value("XMP_DJI_RtkFlag")) {
                camera_pose.setRtkFlag(tl::convertStringTo<int>(rtk_flag.value()));
            }

            tl::Vector3d accuracy;

            auto rtk_std_lon = image_metadata.value("XMP_DJI_RtkStdLon");
            if (rtk_std_lon.has_value()) {
                accuracy[0] = tl::convertStringTo<double>(rtk_std_lon.value());
            }

            auto rtk_std_lat = image_metadata.value("XMP_DJI_RtkStdLat");
            if (rtk_std_lat.has_value()) {
                accuracy[1] = tl::convertStringTo<double>(rtk_std_lat.value());
            }

            auto rtk_std_hgt = image_metadata.value("XMP_DJI_RtkStdHgt");
            if (rtk_std_hgt.has_value()) {
                accuracy[2] = tl::convertStringTo<double>(rtk_std_hgt.value());
            }

            if (!(rtk_std_lon.has_value() && rtk_std_lat.has_value() && rtk_std_hgt.has_value())) {

                // No viene RtkFlag, intentamos estimarlo a partir de GPSXY/Z Accuracy
                auto gps_xy_str = image_metadata.value("XMP_CAMERA_GPSXYAccuracy");
                auto gps_z_str = image_metadata.value("XMP_CAMERA_GPSZAccuracy");

                double gps_xy = tl::convertStringTo<double>(gps_xy_str.value_or("-1.0"));// ? tl::convertStringTo<double>(gps_xy_str) : -1.0;
                double gps_z = tl::convertStringTo<double>(gps_z_str.value_or("-1.0")); // active_z ? tl::convertStringTo<double>(gps_z_str) : -1.0;

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

        // DJI M3M -> XMP_DJI_CaptureUUID
        // Parrot Sequoia -> XMP_CAMERA_CaptureUUID
        std::vector<std::string> capture_uuid_keys = {"XMP_DJI_CaptureUUID", "XMP_CAMERA_CaptureUUID"};

        if (auto capture_uuid = image_metadata.value(capture_uuid_keys))
            mImages->at(imagePosition).addMetadata("CaptureUUID", capture_uuid.value());

        // Radiometric Calibration
        if (auto radiometric_calibration = image_metadata.value("XMP_RadiometricCalibration"))
            mImages->at(imagePosition).addMetadata("RadiometricCalibration", radiometric_calibration.value());

        // Exposure Time
        if (auto exposure_time = image_metadata.value("EXIF_ExposureTime"))
            mImages->at(imagePosition).addMetadata("ExposureTime", exposure_time.value());

        if (auto sensor_gain = image_metadata.value("XMP_DJI_SensorGain"))
            mImages->at(imagePosition).addMetadata("SensorGain", sensor_gain.value());

        if (auto sensor_gain_adjustment = image_metadata.value("XMP_DJI_SensorGainAdjustment"))
            mImages->at(imagePosition).addMetadata("SensorGainAdjustment", sensor_gain_adjustment.value());

        // Sun sensor
        if (auto camera_sun_sensor = image_metadata.value("XMP_CAMERA_SunSensor"))
            mImages->at(imagePosition).addMetadata("SunSensor", camera_sun_sensor.value());

        if (auto camera_sun_sensor_exposure_time = image_metadata.value("XMP_CAMERA_SunSensorExposureTime"))
            mImages->at(imagePosition).addMetadata("SunSensorExposureTime", camera_sun_sensor_exposure_time.value());

        if (auto camera_sun_sensor_pitch = image_metadata.value("XMP_CAMERA_SunSensorPitch"))
            mImages->at(imagePosition).addMetadata("SunSensorPitch", camera_sun_sensor_pitch.value());

        if (auto camera_sun_sensor_roll = image_metadata.value("XMP_CAMERA_SunSensorRoll"))
            mImages->at(imagePosition).addMetadata("SunSensorRoll", camera_sun_sensor_roll.value());

        if (auto camera_sun_sensor_yaw = image_metadata.value("XMP_CAMERA_SunSensorYaw"))
            mImages->at(imagePosition).addMetadata("SunSensorYaw", camera_sun_sensor_yaw.value());

        // Irradiance
        
        if (auto camera_irradiance = image_metadata.value("XMP_CAMERA_Irradiance"))
            mImages->at(imagePosition).addMetadata("Irradiance", camera_irradiance.value());

        if (auto camera_irradiance_exposure_time = image_metadata.value("XMP_CAMERA_IrradianceExposureTime"))
            mImages->at(imagePosition).addMetadata("IrradianceExposureTime", camera_irradiance_exposure_time.value());

        if (auto camera_irradiance_gain = image_metadata.value("XMP_CAMERA_IrradianceGain"))
            mImages->at(imagePosition).addMetadata("IrradianceGain", camera_irradiance_gain.value());

        tl::Message::resumeMessages();

        emit imageAdded(static_cast<int>(imagePosition), camera_position);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

int LoadImagesTask::loadCamera(const tl::RasterReader &imageReader)
{
    int camera_position;

    try {

        tl::Message::pauseMessages();

        auto image_metadata = imageReader.metadata();

        std::vector<std::string> make_strings = {"EXIF_Make", "XMP_TIFF_Make"};
        auto exif_camera_make = image_metadata.value(make_strings);
        std::string camera_make = exif_camera_make.value_or("Unknown camera");

        std::vector<std::string> model_strings = {"EXIF_Model", "XMP_CAMERA_RigName", "XMP_TIFF_Model"};
        auto exif_camera_model = image_metadata.value(model_strings);
        if (!exif_camera_model.has_value()) {
            exif_camera_model = image_metadata.value("XMP_CAMERA_RigName");
        }
        std::string camera_model = exif_camera_model.value_or("");

        std::vector<std::string> serial_number_keys = {"XMP_DJI_CameraSerialNumber", "EXIF_SerialNumber", "EXIF_BodySerialNumber"};
        auto exif_serial_number = image_metadata.value(serial_number_keys);
        std::string serial_number = exif_serial_number.value_or("");

        std::string band_name;
        auto exif_band_name = image_metadata.value("XMP_CAMERA_BandName");
        if (!exif_band_name.has_value()) {
            if (camera_make == "DJI" && camera_model == "M3M")
                band_name = "RGB";
            if (camera_make == "DJI" && camera_model == "M4T")
                band_name = "THERMAL";
            else if (imageReader.channels() >= 3)
                band_name = "RGB";
            else
                band_name = "Gray";
        } else {
            if (camera_make == "Parrot" && camera_model == "Sequoia" && exif_band_name == "Red, Green, Blue") {
                band_name = "RGB";
            } else {
                band_name = exif_band_name.value();
            }
        }

        tl::Message::resumeMessages();

        // No es suficiente porque en el caso de Parrot Sequoia todas las cámaras tienen el mismo número de serie
        // Hay que comprobar también el RigCameraIndex y el RigName para multiespectral
        camera_position = findCamera(QString::fromStdString(camera_make),
                                     QString::fromStdString(camera_model),
                                     QString::fromStdString(serial_number),
                                     QString::fromStdString(band_name));
        if (camera_position != -1) return camera_position;

        int width = imageReader.cols();
        int height = imageReader.rows();

        if (!exif_camera_make.has_value() && !exif_camera_model.has_value()) {

            int counter = 0;
            for (auto &camera : *mCameras) {
                if (camera.make() == "Unknown camera") {
                    if (camera.width() == width && camera.height() == height) {
                        return counter;
                    }
                    counter++;
                }
            }

            //camera_make = "Unknown camera";
            camera_model = std::to_string(counter);
        }

        tl::Message::info("New camera detected: {} {}", camera_make, camera_model);

        Camera camera(camera_make, camera_model, serial_number);
        camera.setWidth(width);
        camera.setHeight(height);
        camera.setBitsPerPixel(imageReader.depth());
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
        } else {
            tl::Message::warning("The camera database does not exist {}", mDatabaseCamerasPath.toStdString());
        }

        /// Extract focal

        double focal = -1.;
        int max_size = std::max(width, height);

        if (auto exif_focal_length_in_35_mm_film = image_metadata.value("EXIF_FocalLengthIn35mmFilm")) {

            double focal_35mm = parseFocal(exif_focal_length_in_35_mm_film.value(), focal);
            focal = focal_35mm / 35.0 * max_size;

        }

        if (focal < 0.) {

            if (auto focal_length = image_metadata.value("EXIF_FocalLength")) {

                double focal_mm = parseFocal(focal_length.value(), focal);

                if (sensor_width_mm > 0.) {

                    focal = focal_mm / sensor_width_mm * max_size;

                } else {

                    if (auto exif_pixel_x_dimension = image_metadata.value("EXIF_PixelXDimension")) {

                        double pixel_x_dimension = tl::convertStringTo<double>(exif_pixel_x_dimension.value());

                        double focal_plane_x_resolution = 0.;
                        if (auto exif_focal_plane_x_resolution = image_metadata.value("EXIF_FocalPlaneXResolution")) {
                            focal_plane_x_resolution = tl::convertStringTo<double>(exif_focal_plane_x_resolution.value());
                        }

                        if (auto exif_focal_plane_resolution_unit = image_metadata.value("EXIF_FocalPlaneResolutionUnit")) {

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

        if (auto dewarp_flag = image_metadata.value("XMP_DJI_DewarpFlag")) {

            if (auto dewarp_data = image_metadata.value("XMP_DJI_DewarpData")) {

                auto v = tl::split<std::string>(dewarp_data.value(), ';');
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
        if (auto black_level = image_metadata.value(std::vector<std::string>{"XMP_CAMERA_BlackCurrent", "XMP_DJI_BlackLevel"}))
            camera.setBlackLevel(tl::convertStringTo<uint16_t>(black_level.value()));

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
        
        if (auto vignetting_flag = image_metadata.value("XMP_DJI_VignettingFlag"); vignetting_flag == "0") {
            
            if (auto vignetting_str = image_metadata.value(std::vector<std::string>{"XMP_DJI_VignettingData", "XMP_CAMERA_VignettingPolynomial"})) {

                //bool active_cx;
                //bool active_cy;
                //std::string cx = image_metadata.value("XMP_DJI_CalibratedOpticalCenterX", active_cx);
                //std::string cy = image_metadata.value("XMP_DJI_CalibratedOpticalCenterY", active_cy);
                //if (active_cx && active_cy) {
                //    auto vignetin_data = tl::split<float>(vignetting_str);
                //    tl::Point2d calibrated_optical_center(tl::stringToNumber<float>(cx), tl::stringToNumber<float>(cy));
                //    camera.setVignettingCenter(calibrated_optical_center);
                //    camera.setVignettingPolynomial(vignetin_data);
                //}

                if (auto vignetting_center = image_metadata.value("XMP_CAMERA_VignettingCenter")) {
                    auto vignetting_data = tl::split<float>(vignetting_str.value());
                    auto vignetting_center_values = tl::split<float>(vignetting_center.value());
                    if (vignetting_center_values.size() == 2) {
                        tl::Point2f vignetting_center_point(vignetting_center_values[0], vignetting_center_values[1]);
                        //camera.setVignettingCenter(vignetting_center_point);
                        //camera.setVignettingPolynomial(vignetting_data);
                        auto model = std::make_shared<VignettingRadial>(vignetting_center_point, vignetting_data);
                        camera.setVignettingModel(model);
                    }
                }
            }
        }

        // XMP_CAMERA_VignettingPolynomial2D
        
        if (auto vignetting_polynomial2d = image_metadata.value("XMP_CAMERA_VignettingPolynomial2D")) {

            if (auto vignetting_polynomial2d_name = image_metadata.value("XMP_CAMERA_VignettingPolynomial2DName")) {
                
                auto coeffs = tl::split<float>(vignetting_polynomial2d.value());
                auto powers = tl::split<float>(vignetting_polynomial2d_name.value());
                std::vector<std::pair<int, int>> power_pairs(powers.size()/2);
                for (size_t i = 0; i < powers.size(); i += 2) {
                    power_pairs[i / 2] = std::make_pair(static_cast<int>(powers[i]), static_cast<int>(powers[i + 1]));
                }
                auto model = std::make_shared<VignettingPolynomial2D>(coeffs, power_pairs);
                camera.setVignettingModel(model);
            }
        }

        // XMP_DJI_CalibratedHMatrix
        
        if (auto calibrated_hmatrix = image_metadata.value("XMP_DJI_CalibratedHMatrix")) {
            auto hmatrix_values = tl::split<float>(calibrated_hmatrix.value());
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

void LoadImagesTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{
    try {

        //tl::Chrono chrono;
        //chrono.run();

        //mCrsIn = std::make_shared<tl::Crs>("EPSG:4326");
        //std::shared_ptr<tl::Crs> crs_out;
        //if (!mEPSG.isEmpty()) {
        //    mCrsOut = std::make_shared<tl::Crs>(mEPSG.toStdString());
        //}

        for (size_t i = 0; i < mImages->size(); i++) {

            //if (status() == Status::stopping)  break;
            interruptionPoint(stopToken);
            pausePoint(stopToken);

            loadImage(i);

            if (progressBar) (*progressBar)();

        }

        if (status() == Status::stopping) {
            //chrono.reset();
        } else {
            
            tl::Message::success("{} images loaded in {:.2} minutes", mImages->size(), this->time() /60.);
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
