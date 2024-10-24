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

#include "graphos/core/project.h"
#include "graphos/components/properties/PropertiesParser.h"

#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>

#include "graphos/core/dense/CmvsPmvs.h"
#include "graphos/core/dense/mvs.h"
#include "graphos/core/dense/Smvs.h"


std::string removeParentheses(const std::string &value)
{
  size_t pos1 = value.find('(');
  size_t pos2 = value.find(')');

  std::string parse_value;

  if(pos1 != std::string::npos && pos2 != std::string::npos) {
    parse_value = value.substr(pos1 + 1, pos2 - (pos1 + 1));
  }

  return parse_value;
}



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

        Image image = mProject->findImageById(imageId);

        auto image_reader = tl::ImageReaderFactory::create(image.path().toStdString());
        image_reader->open();
        if (image_reader->isOpen()) {

            std::shared_ptr<tl::ImageMetadata> image_metadata = image_reader->metadata();
            std::map<std::string, std::string> metadata = image_metadata->activeMetadata();
            std::string name;
            std::string value;

            std::map<std::string, std::string> values;

            tl::Message::pauseMessages();

            bool active = false;

            //value = image_metadata->metadata("EXIF_Document_Name", active);
            //if(active) {
            //  exif["Image"][QString("Document Name")] = QString::fromStdString(value);
            //}
            //
            //value = image_metadata->metadata("EXIF_ImageDescription", active);
            //if(active) {
            //  exif["Description"][QString("Image description")] = QString::fromStdString(value);
            //}

            value = image_metadata->metadata("EXIF_Make", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Camera make"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_Model", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Camera model"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_FNumber", active);
            if (active) {
                exif["Camera"].emplace_back(QString("F Number"), QString::fromStdString("f/" + removeParentheses(value)));
            }

            value = image_metadata->metadata("EXIF_ExposureTime", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Exposure time"), QString::fromStdString(removeParentheses(value)));
            }

            //value = image_metadata->metadata("EXIF_ISOSpeed", active);
            //if(active) {
            //  exif["Camera"].push_back(std::make_pair(QString("ISO speed"), QString::fromStdString(value)));
            //}

            value = image_metadata->metadata("EXIF_ISOSpeedRatings", active);
            if (active) {
                exif["Camera"].emplace_back(QString("ISO Speed"), QString::fromStdString("ISO-" + value));
            }

            value = image_metadata->metadata("EXIF_ExposureTime", active);
            if (active) {
                //exif["Camera"].push_back(std::make_pair(QString("Exposure time"), QString::fromStdString(removeParentheses(value))));
                exif["Camera"].emplace_back(QString("Exposure time"), QString::fromStdString(removeParentheses(value)));
            }

            value = image_metadata->metadata("EXIF_ExposureBiasValue", active);
            if (active) {
                value = removeParentheses(value);
                double exposure_bias = std::stod(value);
                exif["Camera"].emplace_back(QString("Exposure bias"), QString::number(exposure_bias, 'f', 1) + " step");
            }

            value = image_metadata->metadata("EXIF_ExposureMode", active);
            if (active) {

                QString exposure_mode = 0;
                if (value == "0") {
                    exposure_mode = "Auto";
                } else if (value == "1") {
                    exposure_mode = "Manual";
                } else if (value == "2") {
                    exposure_mode = "Auto bracket";
                }

                if (exposure_mode != QString("0"))
                    exif["Camera"].emplace_back(QString("Exposure mode"), exposure_mode);
            }

            value = image_metadata->metadata("EXIF_ExposureProgram", active);
            if (active) {

                QString exposure_program = 0;
                if (value == "1") {
                    exposure_program = "Manual";
                } else if (value == "2") {
                    exposure_program = "Program AE";
                } else if (value == "3") {
                    exposure_program = "Aperture-priority AE";
                } else if (value == "4") {
                    exposure_program = "Shutter speed priority AE";
                } else if (value == "5") {
                    exposure_program = "Creative (Slow speed)";
                } else if (value == "6") {
                    exposure_program = "Action (High speed)";
                } else if (value == "7") {
                    exposure_program = "Portrait";
                } else if (value == "8") {
                    exposure_program = "Landscape";
                }

                if (exposure_program != QString("0"))
                    exif["Camera"].emplace_back(QString("Exposure program"), exposure_program);
            }

            value = image_metadata->metadata("EXIF_FocalLength", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Focal length"), QString::fromStdString(removeParentheses(value) + " mm"));
            }

            value = image_metadata->metadata("EXIF_ApertureValue", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Aperture value"), QString::fromStdString(removeParentheses(value)));
            }

            value = image_metadata->metadata("EXIF_MaxApertureValue", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Max aperture"), QString::fromStdString(removeParentheses(value)));
            }

            value = image_metadata->metadata("EXIF_FocalLengthIn35mmFilm", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Focal Length In 35 mm"), QString::fromStdString(value));
            }

            //value = image_metadata->metadata("EXIF_LensSpecification", active);
            //if(active) {
            //  exif["Camera"].push_back(std::make_pair(QString("Lens Specification"), QString::fromStdString(value)));
            //}

            value = image_metadata->metadata("EXIF_LensMake", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Lens make"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_LensModel", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Lens model"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_LensSerialNumber", active);
            if (active) {
                exif["Camera"].emplace_back(QString("Lens Serial number"), QString::fromStdString(value));
            }

            // Image

            {
                bool active1 = false;
                bool active2 = false;
                std::string x_dimension = image_metadata->metadata("EXIF_PixelXDimension", active1);
                std::string y_dimension = image_metadata->metadata("EXIF_PixelYDimension", active2);
                if (active1 && active2) {
                    exif["Image"].emplace_back(QString("Dimensions"), QString::fromStdString(x_dimension + " x " + y_dimension));
                    exif["Image"].emplace_back(QString("Width"), QString::fromStdString(x_dimension + " pixels"));
                    exif["Image"].emplace_back(QString("Height"), QString::fromStdString(y_dimension + " pixels"));
                }
            }

            {
                std::string resolution_unit;
                value = image_metadata->metadata("EXIF_ResolutionUnit", active);
                if (active) {
                    if (value == "2") {
                        resolution_unit = "dpi";
                    } else if (value == "3") {
                        resolution_unit = "dpc";
                    }
                    //exif.push_back(std::make_pair(QString("Resolution Unit"), QString::fromStdString(value)));
                }

                bool active1 = false;
                bool active2 = false;
                std::string x_resolution = image_metadata->metadata("EXIF_XResolution", active1);
                std::string y_resolution = image_metadata->metadata("EXIF_YResolution", active2);

                if (active1 && active2) {
                    exif["Image"].emplace_back(QString("Horizontal resolution"), QString::fromStdString(removeParentheses(x_resolution) + " " + resolution_unit));
                    exif["Image"].emplace_back(QString("Vertical resolution"), QString::fromStdString(removeParentheses(y_resolution) + " " + resolution_unit));
                }

            }

            value = image_metadata->metadata("EXIF_Orientation", active);
            if (active) {
                QString orientation;
                if (value == "1") {
                    orientation = "Horizontal";
                } else if (value == "2") {
                    orientation = "Mirror horizontal";
                } else if (value == "3") {
                    orientation = "Rotate 180";
                } else if (value == "4") {
                    orientation = "Mirror vertical";
                } else if (value == "5") {
                    orientation = "Mirror horizontal and rotate 270 CW";
                } else if (value == "6") {
                    orientation = "Rotate 90 CW";
                } else if (value == "7") {
                    orientation = "Mirror horizontal and rotate 90 CW";
                } else if (value == "8") {
                    orientation = "Rotate 270 CW";
                }

                exif["Image"].emplace_back(QString("Orientation"), orientation);
            }

            value = image_metadata->metadata("EXIF_CompressedBitsPerPixel", active);
            if (active) {
                exif["Image"].emplace_back(QString("Compressed bits per pixel"), QString::fromStdString(removeParentheses(value)));
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

            value = image_metadata->metadata("EXIF_DateTimeOriginal", active);
            if (active) {
                exif["Origin"].emplace_back(QString("Date Time Original"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_DateTimeDigitized", active);
            if (active) {
                exif["Origin"].emplace_back(QString("Date Time Digitized"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_OffsetTime", active);
            if (active) {
                exif["Origin"].emplace_back(QString("Offset Time"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_OffsetTimeOriginal", active);
            if (active) {
                exif["Origin"].emplace_back(QString("Offset Time Original"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_OffsetTimeDigitized", active);
            if (active) {
                exif["Origin"].emplace_back(QString("Offset Time Digitized"), QString::fromStdString(value));
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

            value = image_metadata->metadata("EXIF_GPSLatitude", active);
            if (active) {
                std::string ref = image_metadata->metadata("EXIF_GPSLatitudeRef", active);
                tl::Degrees<double> latitude = formatDegreesFromExif(value, ref);
                exif["GPS"].emplace_back(
                    QString("GPS Latitude"),
                    QString("%1º%2'%3'' %4").arg(std::abs(latitude.degrees()))
                    .arg(latitude.minutes())
                    .arg(latitude.seconds())
                    .arg(ref.c_str()));
            }

            value = image_metadata->metadata("EXIF_GPSLongitude", active);
            if (active) {
                std::string ref = image_metadata->metadata("EXIF_GPSLongitudeRef", active);
                tl::Degrees<double> longitude = formatDegreesFromExif(value, ref);
                exif["GPS"].emplace_back(
                    QString("GPS Longitude"),
                    QString("%1º%2'%3'' %4")
                    .arg(std::abs(longitude.degrees()))
                    .arg(longitude.minutes())
                    .arg(longitude.seconds())
                    .arg(ref.c_str()));
            }

            value = image_metadata->metadata("EXIF_GPSAltitude", active);
            if (active) {
                std::string altitude = removeParentheses(value);
                value = image_metadata->metadata("EXIF_GPSAltitudeRef", active);
                if ("0x00" == value)
                    altitude.append(" Above sea level");
                else
                    altitude.append(" Below sea level");
                value = image_metadata->metadata("EXIF_GPSAltitudeRef", active);
                exif["GPS"].emplace_back(QString("GPS Altitude"), QString::fromStdString(altitude));
            }

            value = image_metadata->metadata("EXIF_GPSTimeStamp", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Time Stamp"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSSatellites", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Satellites"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSStatus", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Status"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSMeasureMode", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Measure Mode"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDOP", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS DOP"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSSpeedRef", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Speed Ref"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSSpeed", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Speed"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("GPS Track Ref", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Track Ref"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSTrack", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Track"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSImgDirectionRef", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Img Direction Ref"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSImgDirection", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Img Direction"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSMapDatum", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Map Datum"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestLatitudeRef", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Latitude Ref"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestLatitude", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Latitude"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestLongitudeRef", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Longitude Ref"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestLongitude", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Longitude"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestBearingRef", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Bearing Ref"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestBearing", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Bearing"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestDistanceRef", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Distance Ref"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDestDistance", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Dest Distance"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSProcessingMethod", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Processing Method"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSAreaInformation", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Area Information"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDateStamp", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Date Stamp"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSDifferential", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Differential"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("EXIF_GPSHPositioningError", active);
            if (active) {
                exif["GPS"].emplace_back(QString("GPS Horizontal Positioning Error"), QString::fromStdString(value));
            }

            /// XMP

            value = image_metadata->metadata("XMP_AbsoluteAltitude", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Absolute Altitude"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_RelativeAltitude", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Relative Altitude"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_GimbalRollDegree", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Gimbal Roll Degree"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_GimbalYawDegree", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Gimbal Yaw Degree"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_GimbalPitchDegree", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Gimbal Pitch Degree"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_FlightRollDegree", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Flight Roll Degree"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_FlightYawDegree", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Flight Yaw Degree"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_FlightPitchDegree", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Flight Pitch Degree"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_CamReverse", active);
            if (active) {
                exif["XMP"].emplace_back(QString("CamReverse"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_GimbalReverse", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Gimbal Reverse"), QString::fromStdString(value));
            }

            value = image_metadata->metadata("XMP_RtkFlag", active);
            if (active) {
                exif["XMP"].emplace_back(QString("Rtk Flag"), QString::fromStdString(value));
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

            image_reader->close();
        }

        if (mProject->isPhotoOriented(imageId)) {
            auto orientation = mProject->photoOrientation(imageId);
            auto position = orientation.position();
            auto q = orientation.quaternion();

            exif["Orientation"].emplace_back(QString("X"), QString::number(position.x, 'g', 3));
            exif["Orientation"].emplace_back(QString("Y"), QString::number(position.y, 'g', 3));
            exif["Orientation"].emplace_back(QString("Z"), QString::number(position.z, 'g', 3));
            exif["Orientation"].emplace_back(QString("Qx"), QString::number(q.x, 'g', 3));
            exif["Orientation"].emplace_back(QString("Qy"), QString::number(q.y, 'g', 3));
            exif["Orientation"].emplace_back(QString("Qz"), QString::number(q.z, 'g', 3));
            exif["Orientation"].emplace_back(QString("Qw"), QString::number(q.w, 'g', 3));

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return exif;
}

auto PropertiesModelImp::parse(const QString& parser, const QString& file) const -> Properties
{
    try {
        auto properties_parser = PropertiesParserFactory::create(parser);
        return properties_parser->parse(file);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

auto PropertiesModelImp::parse(const QStringList& parsers,
                               const QStringList& files) const -> Properties
{
    Properties properties;

    try {

        TL_ASSERT(parsers.size() == files.size(), "The number of parsers and files must be the same.");

        for (size_t i = 0; i < parsers.size(); i++) {
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
    sparse_model_info["Orientation results"].emplace_back(QString("Oriented images"), QString::number(orientation_report.orientedImages).append(" of ").append(QString::number(mProject->imagesCount())));
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

    if (auto densification = mProject->densification()) {

        auto densification_method = densification->method();
        if (densification_method == Densification::Method::cmvs_pmvs) {

            auto cmvs_pmvs = std::dynamic_pointer_cast<CmvsPmvs>(densification);
            dense_model_info["Densification parameters (CMVS/PMVS)"].emplace_back(QString("Level"), QString::number(cmvs_pmvs->level()));
            dense_model_info["Densification parameters (CMVS/PMVS)"].emplace_back(QString("Cell size"), QString::number(cmvs_pmvs->cellSize()));
            dense_model_info["Densification parameters (CMVS/PMVS)"].emplace_back(QString("Threshold"), QString::number(cmvs_pmvs->threshold(), 'g', 5));
            dense_model_info["Densification parameters (CMVS/PMVS)"].emplace_back(QString("Window Size"), QString::number(cmvs_pmvs->windowSize()));
            dense_model_info["Densification parameters (CMVS/PMVS)"].emplace_back(QString("Images per cluster"), QString::number(cmvs_pmvs->imagesPerCluster()));
            dense_model_info["Densification parameters (CMVS/PMVS)"].emplace_back(QString("Minimun image number"), QString::number(cmvs_pmvs->minimunImageNumber()));
            dense_model_info["Densification parameters (CMVS/PMVS)"].emplace_back(QString("Use visibility information"), cmvs_pmvs->useVisibilityInformation() ? "True" : "False");

        } else if (densification_method == Densification::Method::smvs) {

            auto smvs = std::dynamic_pointer_cast<Smvs>(densification);
            dense_model_info["Densification parameters (SMVS)"].emplace_back(QString("Input image scale"), QString::number(smvs->inputImageScale()));
            dense_model_info["Densification parameters (SMVS)"].emplace_back(QString("Output depth scale"), QString::number(smvs->outputDepthScale()));
            dense_model_info["Densification parameters (SMVS)"].emplace_back(QString("Semi-global matching"), smvs->semiGlobalMatching() ? "True" : "False");
            dense_model_info["Densification parameters (SMVS)"].emplace_back(QString("Surface smoothing factor"), QString::number(smvs->surfaceSmoothingFactor(), 'g', 5));
            dense_model_info["Densification parameters (SMVS)"].emplace_back(QString("Shading based optimization"), smvs->shadingBasedOptimization() ? "True" : "False");

        } else if (densification_method == Densification::Method::mvs) {

            auto mvs = std::dynamic_pointer_cast<Mvs>(densification);
            dense_model_info["Densification parameters (MVS)"].emplace_back(QString("Resolution level"), QString::number(mvs->resolutionLevel()));
            dense_model_info["Densification parameters (MVS)"].emplace_back(QString("Min resolution"), QString::number(mvs->minResolution()));
            dense_model_info["Densification parameters (MVS)"].emplace_back(QString("Max resolution"), QString::number(mvs->maxResolution()));
            dense_model_info["Densification parameters (MVS)"].emplace_back(QString("Number views"), QString::number(mvs->numberViews()));
            dense_model_info["Densification parameters (MVS)"].emplace_back(QString("Number views fuse"), QString::number(mvs->numberViewsFuse()));

        }

        auto dense_report = mProject->denseReport();
        if (!dense_report.isEmpty()) {

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

    if (auto mesh_properties = mProject->meshProperties()) {

        mesh_info["Poisson reconstruction parameters"].emplace_back(QString("Depth"), QString::number(mesh_properties->depth()));
        //mesh_info["Poisson reconstruction parameters"].emplace_back(QString("Solve depth"), QString::number(mesh_properties->solveDepth()));
        mesh_info["Poisson reconstruction parameters"].emplace_back(QString("Boundary type"), mesh_properties->boundaryTypeAsText());

        auto mesh_report = mProject->meshReport();
        if (!mesh_report.isEmpty()) {
            mesh_info["Poisson reconstruction results"].emplace_back(QString("Processig time"), QString::number(mesh_report.time / 60., 'g', 2).append(" minutes"));
        }
    }

    return mesh_info;
}

} // namespace graphos
