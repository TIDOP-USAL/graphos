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

#include "PropertiesModel.h"

#include "graphos/core/project/Project.h"
#include "graphos/core/utils.h"
#include "graphos/components/properties/PropertiesParser.h"

#include <tidop/rastertools/io/Reader.h>


namespace graphos
{


PropertiesModelImp::PropertiesModelImp(Project *project,
    QObject *parent)
    : PropertiesModel(parent),
    mProject(project)
{
    PropertiesModelImp::init();
}

PropertiesModelImp::~PropertiesModelImp()
{

}

auto PropertiesModelImp::exif(size_t imageId) const -> Properties
{
    Properties exif;

    try {

        auto &image_repo = mProject->images();
        if (Image *image = image_repo.find(imageId)) {

            exif = this->image(image->path());
        }

        auto &camera_poses_repo = mProject->cameraPoses();
        if (camera_poses_repo.empty())
            return exif;

        if (auto camera_pose = camera_poses_repo.find(imageId)) {

            auto position = camera_pose->position();
            auto quaternion = camera_pose->quaternion();

            exif["Orientation"].emplace_back(QString("X"), QString::number(position.x(), 'g', 3));
            exif["Orientation"].emplace_back(QString("Y"), QString::number(position.y(), 'g', 3));
            exif["Orientation"].emplace_back(QString("Z"), QString::number(position.z(), 'g', 3));
            exif["Orientation"].emplace_back(QString("Qx"), QString::number(quaternion.x(), 'g', 3));
            exif["Orientation"].emplace_back(QString("Qy"), QString::number(quaternion.y(), 'g', 3));
            exif["Orientation"].emplace_back(QString("Qz"), QString::number(quaternion.z(), 'g', 3));
            exif["Orientation"].emplace_back(QString("Qw"), QString::number(quaternion.w(), 'g', 3));

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return exif;
}

auto PropertiesModelImp::image(const tl::Path &image) const -> Properties
{
    Properties exif;

    try {

        tl::RasterReader image_reader(image);
        if (image_reader.isOpen()) {

            auto image_metadata = image_reader.metadata();

            tl::Message::pauseMessages();

            if (const auto make = image_metadata.value("EXIF_Make")) {
                exif["Camera"].emplace_back(QString("Camera make"), QString::fromStdString(*make));
            }

            if (const auto model = image_metadata.value("EXIF_Model")) {
                exif["Camera"].emplace_back(QString("Camera model"), QString::fromStdString(*model));
            }

            if (const auto fnumber = image_metadata.value("EXIF_FNumber")) {
                exif["Camera"].emplace_back(QString("F Number"), QString::fromStdString("f/" + *fnumber));
            }

            if (const auto exposure_time = image_metadata.value("EXIF_ExposureTime")) {
                exif["Camera"].emplace_back(QString("Exposure time"), QString::fromStdString(*exposure_time));
            }

            //value = image_metadata->metadata("EXIF_ISOSpeed", active);
            //if(active) {
            //  exif["Camera"].push_back(std::make_pair(QString("ISO speed"), QString::fromStdString(value)));
            //}

            if (const auto iso_speed = image_metadata.value("EXIF_ISOSpeedRatings")) {
                exif["Camera"].emplace_back(QString("ISO Speed"), QString::fromStdString("ISO-" + *iso_speed));
            }

            if (const auto exposure_time = image_metadata.value("EXIF_ExposureTime")) {
                exif["Camera"].emplace_back(QString("Exposure time"), QString::fromStdString(*exposure_time));
            }

            if (const auto exposure_bias = image_metadata.value("EXIF_ExposureBiasValue")) {
                double exposure_bias_value = std::stod(*exposure_bias);
                exif["Camera"].emplace_back(QString("Exposure bias"), QString::number(exposure_bias_value, 'f', 1) + " step");
            }

            if (const auto exposure_mode = image_metadata.value("EXIF_ExposureMode")) {
                QString exposure_mode_str;
                if (*exposure_mode == "0") {
                    exposure_mode_str = "Auto";
                } else if (*exposure_mode == "1") {
                    exposure_mode_str = "Manual";
                } else if (*exposure_mode == "2") {
                    exposure_mode_str = "Auto bracket";
                }

                if (!exposure_mode_str.isEmpty())
                    exif["Camera"].emplace_back(QString("Exposure mode"), exposure_mode_str);
            }

            if (const auto exposure_program = image_metadata.value("EXIF_ExposureProgram")) {

                QString exposure_program_str;
                if (*exposure_program == "1") {
                    exposure_program_str = "Manual";
                } else if (*exposure_program == "2") {
                    exposure_program_str = "Program AE";
                } else if (*exposure_program == "3") {
                    exposure_program_str = "Aperture-priority AE";
                } else if (*exposure_program == "4") {
                    exposure_program_str = "Shutter speed priority AE";
                } else if (*exposure_program == "5") {
                    exposure_program_str = "Creative (Slow speed)";
                } else if (*exposure_program == "6") {
                    exposure_program_str = "Action (High speed)";
                } else if (*exposure_program == "7") {
                    exposure_program_str = "Portrait";
                } else if (*exposure_program == "8") {
                    exposure_program_str = "Landscape";
                }

                if (!exposure_program_str.isEmpty())
                    exif["Camera"].emplace_back(QString("Exposure program"), exposure_program_str);
            }

            if (const auto focal_length = image_metadata.value("EXIF_FocalLength")) {
                exif["Camera"].emplace_back(QString("Focal length"), QString::fromStdString(*focal_length + " mm"));
            }

            if (const auto aperture_value = image_metadata.value("EXIF_ApertureValue")) {
                exif["Camera"].emplace_back(QString("Aperture value"), QString::fromStdString(*aperture_value));
            }

            if (const auto max_aperture_value = image_metadata.value("EXIF_MaxApertureValue")) {
                exif["Camera"].emplace_back(QString("Max aperture"), QString::fromStdString(*max_aperture_value));
            }

            if (const auto focal_length_in_35mm = image_metadata.value("EXIF_FocalLengthIn35mmFilm")) {
                exif["Camera"].emplace_back(QString("Focal Length In 35 mm"), QString::fromStdString(*focal_length_in_35mm));
            }

            //value = image_metadata->metadata("EXIF_LensSpecification", active);
            //if(active) {
            //  exif["Camera"].push_back(std::make_pair(QString("Lens Specification"), QString::fromStdString(value)));
            //}

            if (const auto lens_make = image_metadata.value("EXIF_LensMake")) {
                exif["Camera"].emplace_back(QString("Lens make"), QString::fromStdString(*lens_make));
            }

            if (const auto lens_model = image_metadata.value("EXIF_LensModel")) {
                exif["Camera"].emplace_back(QString("Lens model"), QString::fromStdString(*lens_model));
            }

            if (const auto lens_serial_number = image_metadata.value("EXIF_LensSerialNumber")) {
                exif["Camera"].emplace_back(QString("Lens Serial number"), QString::fromStdString(*lens_serial_number));
            }

            // Image

            
            const auto x_dimension = image_metadata.value("EXIF_PixelXDimension");
            const auto y_dimension = image_metadata.value("EXIF_PixelYDimension");
            if (x_dimension && y_dimension) {
                exif["Image"].emplace_back(QString("Dimensions"), QString::fromStdString(*x_dimension + " x " + *y_dimension));
                exif["Image"].emplace_back(QString("Width"), QString::fromStdString(*x_dimension + " pixels"));
                exif["Image"].emplace_back(QString("Height"), QString::fromStdString(*y_dimension + " pixels"));
            }
            
            std::string resolution_unit;
            if (const auto resolution_unit_value = image_metadata.value("EXIF_ResolutionUnit")) {
                if (*resolution_unit_value == "2") {
                    resolution_unit = "dpi";
                } else if (*resolution_unit_value == "3") {
                    resolution_unit = "dpc";
                }
            }

            const auto x_resolution = image_metadata.value("EXIF_XResolution");
            const auto y_resolution = image_metadata.value("EXIF_YResolution");

            if (x_resolution && y_resolution) {
                exif["Image"].emplace_back(QString("Horizontal resolution"), QString::fromStdString(*x_resolution + " " + resolution_unit));
                exif["Image"].emplace_back(QString("Vertical resolution"), QString::fromStdString(*y_resolution + " " + resolution_unit));
            }


            if (const auto orientation = image_metadata.value("EXIF_Orientation")){
                QString orientation_str;
                if (*orientation == "1") {
                    orientation_str = "Horizontal";
                } else if (*orientation == "2") {
                    orientation_str = "Mirror horizontal";
                } else if (*orientation == "3") {
                    orientation_str = "Rotate 180";
                } else if (*orientation == "4") {
                    orientation_str = "Mirror vertical";
                } else if (*orientation == "5") {
                    orientation_str = "Mirror horizontal and rotate 270 CW";
                } else if (*orientation == "6") {
                    orientation_str = "Rotate 90 CW";
                } else if (*orientation == "7") {
                    orientation_str = "Mirror horizontal and rotate 90 CW";
                } else if (*orientation == "8") {
                    orientation_str = "Rotate 270 CW";
                }

                exif["Image"].emplace_back(QString("Orientation"), orientation_str);
            }

            if (const auto compressed_bits_per_pixel = image_metadata.value("EXIF_CompressedBitsPerPixel")) {
                exif["Image"].emplace_back(QString("Compressed bits per pixel"), QString::fromStdString(*compressed_bits_per_pixel));
            }

            //value = image_metadata->metadata("EXIF_Software", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Software"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_DateTime", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Date Time"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_Artist", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Artist"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_WhitePoint", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("White Point"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_PrimaryChromaticities", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Primary Chromaticities"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_YCbCrCoefficients", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("YCbCr Coefficients"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_YCbCrPositioning", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("YCbCr Positioning"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_ReferenceBlackWhite", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Reference Black White"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_Copyright", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Copyright"), QString::fromStdString(value)));
            //}

            //
            //
            //
            //value = image_metadata->metadata("EXIF_SpectralSensitivity", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Spectral Sensitivity"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_ISOSpeedRatings", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("ISO Speed Ratings"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_OECF", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("OECF"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_SensitivityType", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Sensitivity Type"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_StandardOutputSensitivity", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Standard Output Sensitivity"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_RecommendedExposureIndex", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Recommended Exposure Index"), QString::fromStdString(value)));
            //}

            //value = image_metadata->metadata("EXIF_ISOSpeedLatitudeyyy", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("ISO Speed Latitude yyy"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_ISOSpeedLatitudezzz", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("ISO Speed Latitude zzz"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_ExifVersion", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Exif Version"), QString::fromStdString(value)));
            //}

            if (const auto date_time_original = image_metadata.value("EXIF_DateTimeOriginal")) {
                exif["Origin"].emplace_back(QString("Date Time Original"), QString::fromStdString(*date_time_original));
            }

            if (const auto date_time_digitized = image_metadata.value("EXIF_DateTimeDigitized")) {
                exif["Origin"].emplace_back(QString("Date Time Digitized"), QString::fromStdString(*date_time_digitized));
            }

            if (const auto offset_time = image_metadata.value("EXIF_OffsetTime")) {
                exif["Origin"].emplace_back(QString("Offset Time"), QString::fromStdString(*offset_time));
            }

            if (const auto offset_time_original = image_metadata.value("EXIF_OffsetTimeOriginal")) {
                exif["Origin"].emplace_back(QString("Offset Time Original"), QString::fromStdString(*offset_time_original));
            }

            if (const auto offset_time_digitized = image_metadata.value("EXIF_OffsetTimeDigitized")) {
                exif["Origin"].emplace_back(QString("Offset Time Digitized"), QString::fromStdString(*offset_time_digitized));
            }

            ////value = image_metadata->metadata("EXIF_ComponentsConfiguration", active);
            ////if (active) {
            ////  exif.push_back(std::make_pair(QString("Components Configuration"), QString::fromStdString(value)));
            ////}

            //value = image_metadata->metadata("EXIF_ShutterSpeedValue", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Shutter Speed Value"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_BrightnessValue", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Brightness Value"), QString::fromStdString(value)));
            //}
            //

            //value = image_metadata->metadata("EXIF_SubjectDistance", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Subject Distance"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_MeteringMode", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Metering Mode"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_LightSource", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Light Source"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_Flash", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Flash"), QString::fromStdString(value)));
            //}

            //value = image_metadata->metadata("EXIF_SubjectArea", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Subject Area"), QString::fromStdString(value)));
            //}
            //
            ////value = image_metadata->metadata("EXIF_MakerNote", active);
            ////if (active) {
            ////  exif.push_back(std::make_pair(QString("Maker Note"), QString::fromStdString(value)));
            ////}
            //
            //value = image_metadata->metadata("EXIF_UserComment", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("User Comment"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("SubSec Time", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Focal Length"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_SubSecTime_Original", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("SubSecTimeOriginal"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_SubSecTime_Digitized", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("SubSecTimeDigitized"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_FlashpixVersion", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("FlashpixVersion"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_ColorSpace", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Color Space"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_FocalPlaneXResolution", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Focal Plane X Resolution"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_FocalPlaneYResolution", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Focal Plane Y Resolution"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_FocalPlaneResolutionUnit", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Focal Plane Resolution Unit"), QString::fromStdString(value)));
            //}
            //
            //
            //value = image_metadata->metadata("EXIF_WhiteBalance", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("White Balance"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("DigitalZoomRatio", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Digital Zoom Ratio"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_SceneCaptureType", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Scene Capture Type"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_GainControl", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Gain Control"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_Contrast", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Contrast"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_Saturation", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Saturation"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_Sharpness", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Sharpness"), QString::fromStdString(value)));
            //}
            //
            ////value = image_metadata->metadata("EXIF_DeviceSettingDescription", active);
            ////if (active) {
            ////  exif.push_back(std::make_pair(QString("Device Setting Description"), QString::fromStdString(value)));
            ////}
            //
            //value = image_metadata->metadata("EXIF_SubjectDistanceRange", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Subject Distance Range"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_ImageUniqueID", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Image Unique ID"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("EXIF_CameraOwnerName", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Camera Owner Name"), QString::fromStdString(value)));
            //}
            //
            ////value = image_metadata->metadata("EXIF_BodySerialNumber", active);
            ////if (active) {
            ////  exif.push_back(std::make_pair(QString("Body Serial Number"), QString::fromStdString(value)));
            ////}
            //


            // GPS

            const auto gps_latitude = image_metadata.value("EXIF_GPSLatitude");
            const auto gps_latitude_ref = image_metadata.value("EXIF_GPSLatitudeRef");
            if (gps_latitude && gps_latitude_ref) {

                tl::Degrees<double> latitude = formatDegreesFromExif(*gps_latitude, *gps_latitude_ref);
                exif["GPS"].emplace_back(
                    QString("GPS Latitude"),
                    QString("%1º%2'%3'' %4").arg(std::abs(latitude.degrees()))
                    .arg(latitude.minutes())
                    .arg(latitude.seconds())
                    .arg(gps_latitude_ref->c_str()));
            }

            const auto gps_longitude = image_metadata.value("EXIF_GPSLongitude");
            const auto gps_longitude_ref = image_metadata.value("EXIF_GPSLongitudeRef");
            if (gps_longitude && gps_longitude_ref) {
                tl::Degrees<double> longitude = formatDegreesFromExif(*gps_longitude, *gps_longitude_ref);
                exif["GPS"].emplace_back(
                    QString("GPS Longitude"),
                    QString("%1º%2'%3'' %4")
                    .arg(std::abs(longitude.degrees()))
                    .arg(longitude.minutes())
                    .arg(longitude.seconds())
                    .arg(gps_longitude_ref->c_str()));
            }

            const auto gps_altitude = image_metadata.value("EXIF_GPSAltitude");
            const auto gps_altitude_ref = image_metadata.value("EXIF_GPSAltitudeRef");
            if (gps_altitude && gps_altitude_ref) {
                std::string altitude = *gps_altitude;
                if ("0x00" == *gps_altitude_ref)
                    altitude.append(" Above sea level");
                else
                    altitude.append(" Below sea level");
                exif["GPS"].emplace_back(QString("GPS Altitude"), QString::fromStdString(altitude));
            }

            if (const auto gps_time_stamp = image_metadata.value("EXIF_GPSTimeStamp")) {
                exif["GPS"].emplace_back(QString("GPS Time Stamp"), QString::fromStdString(*gps_time_stamp));
            }

            if (const auto gps_satellites = image_metadata.value("EXIF_GPSSatellites")) {
                exif["GPS"].emplace_back(QString("GPS Satellites"), QString::fromStdString(*gps_satellites));
            }

            if (const auto gps_status = image_metadata.value("EXIF_GPSStatus")) {
                exif["GPS"].emplace_back(QString("GPS Status"), QString::fromStdString(*gps_status));
            }

            if (const auto gps_measure_mode = image_metadata.value("EXIF_GPSMeasureMode")) {
                exif["GPS"].emplace_back(QString("GPS Measure Mode"), QString::fromStdString(*gps_measure_mode));
            }

            if (const auto gps_dop = image_metadata.value("EXIF_GPSDOP")) {
                exif["GPS"].emplace_back(QString("GPS DOP"), QString::fromStdString(*gps_dop));
            }

            if (const auto gps_speed_ref = image_metadata.value("EXIF_GPSSpeedRef")) {
                exif["GPS"].emplace_back(QString("GPS Speed Ref"), QString::fromStdString(*gps_speed_ref));
            }

            if (const auto gps_speed = image_metadata.value("EXIF_GPSSpeed")) {
                exif["GPS"].emplace_back(QString("GPS Speed"), QString::fromStdString(*gps_speed));
            }

            if (const auto gps_track_ref = image_metadata.value("GPS Track Ref")) {
                exif["GPS"].emplace_back(QString("GPS Track Ref"), QString::fromStdString(*gps_track_ref));
            }

            if (const auto gps_track = image_metadata.value("EXIF_GPSTrack")) {
                exif["GPS"].emplace_back(QString("GPS Track"), QString::fromStdString(*gps_track));
            }

            if (const auto gps_img_direction_ref = image_metadata.value("EXIF_GPSImgDirectionRef")) {
                exif["GPS"].emplace_back(QString("GPS Img Direction Ref"), QString::fromStdString(*gps_img_direction_ref));
            }

            if (const auto gps_img_direction = image_metadata.value("EXIF_GPSImgDirection")) {
                exif["GPS"].emplace_back(QString("GPS Img Direction"), QString::fromStdString(*gps_img_direction));
            }

            if (const auto gps_map_datum = image_metadata.value("EXIF_GPSMapDatum")) {
                exif["GPS"].emplace_back(QString("GPS Map Datum"), QString::fromStdString(*gps_map_datum));
            }

            if (const auto gps_dest_latitude_ref = image_metadata.value("EXIF_GPSDestLatitudeRef")) {
                exif["GPS"].emplace_back(QString("GPS Dest Latitude Ref"), QString::fromStdString(*gps_dest_latitude_ref));
            }

            if (const auto gps_dest_latitude = image_metadata.value("EXIF_GPSDestLatitude")) {
                exif["GPS"].emplace_back(QString("GPS Dest Latitude"), QString::fromStdString(*gps_dest_latitude));
            }

            if (const auto gps_dest_longitude_ref = image_metadata.value("EXIF_GPSDestLongitudeRef")) {
                exif["GPS"].emplace_back(QString("GPS Dest Longitude Ref"), QString::fromStdString(*gps_dest_longitude_ref));
            }

            if (const auto gps_dest_longitude = image_metadata.value("EXIF_GPSDestLongitude")) {
                exif["GPS"].emplace_back(QString("GPS Dest Longitude"), QString::fromStdString(*gps_dest_longitude));   
            }

            if (const auto gps_dest_bearing_ref = image_metadata.value("EXIF_GPSDestBearingRef")) {
                exif["GPS"].emplace_back(QString("GPS Dest Bearing Ref"), QString::fromStdString(*gps_dest_bearing_ref));
            }

            if (const auto gps_dest_bearing = image_metadata.value("EXIF_GPSDestBearing")) {
                exif["GPS"].emplace_back(QString("GPS Dest Bearing"), QString::fromStdString(*gps_dest_bearing));
            }

            if (const auto gps_dest_distance_ref = image_metadata.value("EXIF_GPSDestDistanceRef")) {
                exif["GPS"].emplace_back(QString("GPS Dest Distance Ref"), QString::fromStdString(*gps_dest_distance_ref));
            }

            if (const auto gps_dest_distance = image_metadata.value("EXIF_GPSDestDistance")) {
                exif["GPS"].emplace_back(QString("GPS Dest Distance"), QString::fromStdString(*gps_dest_distance));
            }

            if (const auto gps_processing_method = image_metadata.value("EXIF_GPSProcessingMethod")) {
                exif["GPS"].emplace_back(QString("GPS Processing Method"), QString::fromStdString(*gps_processing_method));
            }

            if (const auto gps_area_information = image_metadata.value("EXIF_GPSAreaInformation")) {
                exif["GPS"].emplace_back(QString("GPS Area Information"), QString::fromStdString(*gps_area_information));
            }

            if (const auto gps_date_stamp = image_metadata.value("EXIF_GPSDateStamp")) {    
                exif["GPS"].emplace_back(QString("GPS Date Stamp"), QString::fromStdString(*gps_date_stamp));
            }

            if (const auto gps_differential = image_metadata.value("EXIF_GPSDifferential")) {
                exif["GPS"].emplace_back(QString("GPS Differential"), QString::fromStdString(*gps_differential));
            }

            if (const auto gps_h_positioning_error = image_metadata.value("EXIF_GPSHPositioningError")) {
                exif["GPS"].emplace_back(QString("GPS Horizontal Positioning Error"), QString::fromStdString(*gps_h_positioning_error));
            }

            /// XMP

            if (const auto absolute_altitude = image_metadata.value("XMP_DJI_AbsoluteAltitude")) {
                exif["XMP"].emplace_back(QString("Absolute Altitude"), QString::fromStdString(*absolute_altitude));
            }

            if (const auto relative_altitude = image_metadata.value("XMP_RelativeAltitude")) {
                exif["XMP"].emplace_back(QString("Relative Altitude"), QString::fromStdString(*relative_altitude));
            }

            if (const auto gimbal_roll_degree = image_metadata.value("XMP_DJI_GimbalRollDegree")) {
                exif["XMP"].emplace_back(QString("Gimbal Roll Degree"), QString::fromStdString(*gimbal_roll_degree));
            }

            if (const auto gimbal_yaw_degree = image_metadata.value("XMP_DJI_GimbalYawDegree")) {
                exif["XMP"].emplace_back(QString("Gimbal Yaw Degree"), QString::fromStdString(*gimbal_yaw_degree));
            }

            if (const auto gimbal_pitch_degree = image_metadata.value("XMP_DJI_GimbalPitchDegree")) {
                exif["XMP"].emplace_back(QString("Gimbal Pitch Degree"), QString::fromStdString(*gimbal_pitch_degree));
            }

            if (const auto flight_roll_degree = image_metadata.value("XMP_DJI_FlightRollDegree")) {
                exif["XMP"].emplace_back(QString("Flight Roll Degree"), QString::fromStdString(*flight_roll_degree));
            }

            if (const auto flight_yaw_degree = image_metadata.value("XMP_DJI_FlightYawDegree")) {
                exif["XMP"].emplace_back(QString("Flight Yaw Degree"), QString::fromStdString(*flight_yaw_degree));
            }

            if (const auto flight_pitch_degree = image_metadata.value("XMP_DJI_FlightPitchDegree")) {
                exif["XMP"].emplace_back(QString("Flight Pitch Degree"), QString::fromStdString(*flight_pitch_degree));
            }

            if (const auto cam_reverse = image_metadata.value("XMP_DJI_CamReverse")) {
                exif["XMP"].emplace_back(QString("CamReverse"), QString::fromStdString(*cam_reverse));
            }

            if (const auto gimbal_reverse = image_metadata.value("XMP_DJI_GimbalReverse")) {
                exif["XMP"].emplace_back(QString("Gimbal Reverse"), QString::fromStdString(*gimbal_reverse));
            }

            if (const auto rtk_flag = image_metadata.value("XMP_DJI_RtkFlag")) {
                exif["XMP"].emplace_back(QString("Rtk Flag"), QString::fromStdString(*rtk_flag));
            }

            if (const auto rtk_std_lon = image_metadata.value("XMP_DJI_RtkStdLon")) {
                exif["XMP"].emplace_back(QString("Longitude Accuracy"), QString::fromStdString(*rtk_std_lon));
            }

            if (const auto rtk_std_lat = image_metadata.value("XMP_DJI_RtkStdLat")) {
                exif["XMP"].emplace_back(QString("Latitude Accuracy"), QString::fromStdString(*rtk_std_lat));
            }

            if (const auto rtk_std_hgt = image_metadata.value("XMP_DJI_RtkStdHgt")) {
                exif["XMP"].emplace_back(QString("Altitude Accuracy"), QString::fromStdString(*rtk_std_hgt));
            }

            if (const auto rtk_std_lat = image_metadata.value("XMP_DJI_RtkStdLat")) {
                exif["XMP"].emplace_back(QString("Latitude Accuracy"), QString::fromStdString(*rtk_std_lat));
            }

            if (const auto rtk_std_hgt = image_metadata.value("XMP_DJI_RtkStdHgt")) {
                exif["XMP"].emplace_back(QString("Altitude Accuracy"), QString::fromStdString(*rtk_std_hgt));
            }

            ///// TIFF
            //
            //value = image_metadata->metadata("TIFFTAG_DOCUMENTNAME", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Document Name"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("TIFFTAG_IMAGEDESCRIPTION", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Image Description"), QString::fromStdString(value)));
            //}
            //
            //value = image_metadata->metadata("TIFFTAG_SOFTWARE", active);
            //if(active) {
            //  exif.push_back(std::make_pair(QString("Software"), QString::fromStdString(value)));
            //}

            /// ...
            tl::Message::resumeMessages();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return exif;
}

auto PropertiesModelImp::parse(const QString &parser, const QString &file) const -> Properties
{
    try {
        auto properties_parser = PropertiesParserFactory::create(parser);
        return properties_parser->parse(file);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

auto PropertiesModelImp::parse(const QStringList &parsers,
    const QStringList &files) const -> Properties
{
    Properties properties;

    try {

        TL_ASSERT(parsers.size() == files.size(), "The number of parsers and files must be the same.");

        for (int i = 0; i < parsers.size(); i++) {
            auto _properties = parse(parsers[i], files[i]);
            properties.insert(_properties.begin(), _properties.end());
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return properties;
}

void PropertiesModelImp::init()
{

}

void PropertiesModelImp::clear()
{
    mProject->clear();
}

auto PropertiesModelImp::sparseModel() const -> Properties
{
    Properties sparse_model_info;

    auto orientation_report = mProject->orientationReport();

    sparse_model_info["Orientation parameters"].emplace_back(QString("Orientation type"), QString::fromStdString(orientation_report.type));
    sparse_model_info["Orientation results"].emplace_back(QString("Oriented images"), QString::number(orientation_report.orientedImages).append(" of ").append(QString::number(mProject->images().size())));
    sparse_model_info["Orientation results"].emplace_back(QString("BA iterations"), QString::number(orientation_report.iterations));
    sparse_model_info["Orientation results"].emplace_back(QString("BA initial cost"), QString::number(orientation_report.initialCost, 'f', 10));
    sparse_model_info["Orientation results"].emplace_back(QString("BA final cost"), QString::number(orientation_report.finalCost, 'f', 10));
    if (orientation_report.type == "Absolute") {
        sparse_model_info["Orientation results"].emplace_back(QString("Absolute orientation error (mean)"), QString::number(orientation_report.alignmentErrorMean, 'f', 10));
        sparse_model_info["Orientation results"].emplace_back(QString("Absolute orientation error (median)"), QString::number(orientation_report.alignmentErrorMedian, 'f', 10));
    }

    sparse_model_info["Orientation results"].emplace_back(QString("Processing time"), QString::number(orientation_report.time / 60., 'g', 2).append(" minutes"));

    return sparse_model_info;
}

auto PropertiesModelImp::denseModel() const -> Properties
{
    Properties dense_model_info;

    if (auto densification = mProject->densificationConfig()) {

        auto densification_method = densification->name();

        QString method = QString("Densification parameters (%1)").arg(densification_method);

        for (auto &[key, value] : *densification) {
            dense_model_info[method].emplace_back(QString::fromStdString(key), QString::fromStdString(value->toString()));
        }
        
        if (auto dense_report = mProject->densificationReport(); !dense_report.isEmpty()) {

            dense_model_info["Densification parameters"].emplace_back(QString("Cuda"), dense_report.time ? "True" : "False");

            dense_model_info["Densification results"].emplace_back(QString("Point cloud size"), QString::number(dense_report.points));
            dense_model_info["Densification results"].emplace_back(QString("Processig time"), QString::number(dense_report.time / 60., 'g', 2).append(" minutes"));

        }
    }

    return dense_model_info;
}

auto PropertiesModelImp::meshModel() const -> Properties
{
    Properties mesh_info;

    if (auto mesh_properties = mProject->meshConfig()) {

        for (auto &[key, value] : *mesh_properties) {
            mesh_info["Poisson reconstruction parameters"].emplace_back(QString::fromStdString(key), QString::fromStdString(value->toString()));
        }

        if (auto mesh_report = mProject->meshReport(); !mesh_report.isEmpty()) {
            mesh_info["Poisson reconstruction results"].emplace_back(QString("Processig time"), QString::number(mesh_report.time / 60., 'g', 2).append(" minutes"));
        }
    }

    return mesh_info;
}

auto PropertiesModelImp::dem() const -> Properties
{
    Properties dem_info;

    if (auto dem_properties = mProject->demConfig()) {

        for (auto &[key, value] : *dem_properties) {
            dem_info["DEM parameters"].emplace_back(QString::fromStdString(key), QString::fromStdString(value->toString()));
        }
    }

    if (auto dem_report = mProject->demReport(); !dem_report.isEmpty()) {
        dem_info["DEM results"].emplace_back(QString("Processig time"), QString::number(dem_report.time / 60., 'g', 2).append(" minutes"));
        if (dem_report.cols != 0 && dem_report.rows != 0)
            dem_info["DEM results"].emplace_back(QString("Dimensions"), QString::number(dem_report.cols).append(" x ").append(QString::number(dem_report.rows)));
    }

    return dem_info;
}

auto PropertiesModelImp::orthophoto(size_t orthoId) const -> Properties
{
    Properties orthophoto_info;

    if (auto ortho_properties = mProject->orthoConfig()) {

        for (auto &[key, value] : *ortho_properties) {
            orthophoto_info["Orthophoto parameters"].emplace_back(QString::fromStdString(key), QString::fromStdString(value->toString()));
        }
    }

    if (auto orthophoto_report = mProject->orthoReport(); !orthophoto_report.isEmpty()) {
        orthophoto_info["Orthophoto results"].emplace_back(QString("Processig time"), QString::number(orthophoto_report.time / 60., 'g', 2).append(" minutes"));
        // Tamaño, nº bits, bandas
        if (orthophoto_report.cols != 0 && orthophoto_report.rows != 0)
            orthophoto_info["Orthophoto results"].emplace_back(QString("Dimensions"), QString::number(orthophoto_report.cols).append(" x ").append(QString::number(orthophoto_report.rows)));
        if (orthophoto_report.channels != 0)
            orthophoto_info["Orthophoto results"].emplace_back(QString("Channels"), QString::number(orthophoto_report.channels));
    }

    return orthophoto_info;
}

} // namespace graphos
