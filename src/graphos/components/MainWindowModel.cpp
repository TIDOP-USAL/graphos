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

#include "MainWindowModel.h"

#include "graphos/core/utils.h"
#include "graphos/core/orientation/orientationcolmap.h"

#include <tidop/core/messages.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>

#include <QStandardPaths>
#include <QDir>

namespace graphos
{

MainWindowModel::MainWindowModel(Project *project)
  : mPrjDefaultPath(""),
    mProject(project)
{
  this->init();
}

std::list<std::pair<QString, QString> > MainWindowModel::exif(const QString &image) const
{

  std::list<std::pair<QString, QString>> exif;
  
  std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.toStdString());
  imageReader->open();
  if (imageReader->isOpen()) {

    std::shared_ptr<tl::ImageMetadata> image_metadata = imageReader->metadata();
    std::map<std::string, std::string> metadata = image_metadata->activeMetadata();
    std::string name;
    std::string value;

    std::map<std::string, std::string> values;

    tl::MessageManager::pause();

    bool active = false;

    value = image_metadata->metadata("EXIF_Document_Name", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Document Name"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ImageDescription", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Image description"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Make", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Camera make"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Model", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Camera model"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Orientation", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Orientation"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_XResolution", active);
    if (active) {
      exif.push_back(std::make_pair(QString("X Resolution"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_YResolution", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Y Resolution"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ResolutionUnit", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Resolution Unit"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Software", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Software"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_DateTime", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Date Time"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Artist", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Artist"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_WhitePoint", active);
    if (active) {
      exif.push_back(std::make_pair(QString("White Point"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_PrimaryChromaticities", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Primary Chromaticities"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_YCbCrCoefficients", active);
    if (active) {
      exif.push_back(std::make_pair(QString("YCbCr Coefficients"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_YCbCrPositioning", active);
    if (active) {
      exif.push_back(std::make_pair(QString("YCbCr Positioning"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ReferenceBlackWhite", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Reference Black White"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Copyright", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Copyright"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ExposureTime", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Exposure Time"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FNumber", active);
    if (active) {
      exif.push_back(std::make_pair(QString("F Number"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ExposureProgram", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Exposure Program"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_SpectralSensitivity", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Spectral Sensitivity"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ISOSpeedRatings", active);
    if (active) {
      exif.push_back(std::make_pair(QString("ISO Speed Ratings"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_OECF", active);
    if (active) {
      exif.push_back(std::make_pair(QString("OECF"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_SensitivityType", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Sensitivity Type"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_StandardOutputSensitivity", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Standard Output Sensitivity"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_RecommendedExposureIndex", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Recommended Exposure Index"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ISOSpeed", active);
    if (active) {
      exif.push_back(std::make_pair(QString("ISO Speed"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ISOSpeedLatitudeyyy", active);
    if (active) {
      exif.push_back(std::make_pair(QString("ISO Speed Latitude yyy"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ISOSpeedLatitudezzz", active);
    if (active) {
      exif.push_back(std::make_pair(QString("ISO Speed Latitude zzz"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ExifVersion", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Exif Version"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_DateTimeOriginal", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Date Time Original"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_DateTimeDigitized", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Date Time Digitized"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_OffsetTime", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Offset Time"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_OffsetTimeOriginal", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Offset Time Original"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_OffsetTimeDigitized", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Offset Time Digitized"), QString::fromStdString(value)));
    }

    //value = image_metadata->metadata("EXIF_ComponentsConfiguration", active);
    //if (active) {
    //  exif.push_back(std::make_pair(QString("Components Configuration"), QString::fromStdString(value)));
    //}

    value = image_metadata->metadata("EXIF_CompressedBitsPerPixel", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Compressed Bits Per Pixel"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ShutterSpeedValue", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Shutter Speed Value"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ApertureValue", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Aperture Value"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_BrightnessValue", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Brightness Value"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ExposureBiasValue", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Exposure Bias Value"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_MaxApertureValue", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Max Aperture Value"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_SubjectDistance", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Subject Distance"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_MeteringMode", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Metering Mode"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_LightSource", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Light Source"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Flash", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Flash"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FocalLength", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Focal Length"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_SubjectArea", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Subject Area"), QString::fromStdString(value)));
    }

    //value = image_metadata->metadata("EXIF_MakerNote", active);
    //if (active) {
    //  exif.push_back(std::make_pair(QString("Maker Note"), QString::fromStdString(value)));
    //}

    value = image_metadata->metadata("EXIF_UserComment", active);
    if (active) {
      exif.push_back(std::make_pair(QString("User Comment"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("SubSec Time", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Focal Length"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_SubSecTime_Original", active);
    if (active) {
      exif.push_back(std::make_pair(QString("SubSecTimeOriginal"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_SubSecTime_Digitized", active);
    if (active) {
      exif.push_back(std::make_pair(QString("SubSecTimeDigitized"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FlashpixVersion", active);
    if (active) {
      exif.push_back(std::make_pair(QString("FlashpixVersion"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ColorSpace", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Color Space"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_PixelXDimension", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Pixel X Dimension"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_PixelYDimension", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Pixel Y Dimension"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FocalPlaneXResolution", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Focal Plane X Resolution"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FocalPlaneYResolution", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Focal Plane Y Resolution"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FocalPlaneResolutionUnit", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Focal Plane Resolution Unit"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ExposureMode", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Exposure Mode"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_WhiteBalance", active);
    if (active) {
      exif.push_back(std::make_pair(QString("White Balance"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("DigitalZoomRatio", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Digital Zoom Ratio"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FocalLengthIn35mmFilm", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Focal Length In 35 mm Film"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_SceneCaptureType", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Scene Capture Type"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GainControl", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Gain Control"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Contrast", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Contrast"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Saturation", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Saturation"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Sharpness", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Sharpness"), QString::fromStdString(value)));
    }

    //value = image_metadata->metadata("EXIF_DeviceSettingDescription", active);
    //if (active) {
    //  exif.push_back(std::make_pair(QString("Device Setting Description"), QString::fromStdString(value)));
    //}

    value = image_metadata->metadata("EXIF_SubjectDistanceRange", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Subject Distance Range"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_ImageUniqueID", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Image Unique ID"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_CameraOwnerName", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Camera Owner Name"), QString::fromStdString(value)));
    }

    //value = image_metadata->metadata("EXIF_BodySerialNumber", active);
    //if (active) {
    //  exif.push_back(std::make_pair(QString("Body Serial Number"), QString::fromStdString(value)));
    //}

    value = image_metadata->metadata("EXIF_LensSpecification", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Lens Specification"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_LensMake", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Lens Make"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_LensModel", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Lens Model"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_LensSerialNumber", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Lens Serial Number"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSLatitude", active);
    if (active) {
      std::string ref = image_metadata->metadata("EXIF_GPSLatitudeRef", active);
      tl::math::Degrees<double> latitude = formatDegreesFromExif(value, ref);
      exif.push_back(std::make_pair(QString("GPS Latitude"), QString("%1º%2'%3'' %4").arg(std::abs(latitude.degrees()))
                                                                                     .arg(latitude.minutes())
                                                                                     .arg(latitude.seconds())
                                                                                     .arg(ref.c_str())));
    }

    value = image_metadata->metadata("EXIF_GPSLongitude", active);
    if (active) {
      std::string ref = image_metadata->metadata("EXIF_GPSLongitudeRef", active);
      tl::math::Degrees<double> longitude = formatDegreesFromExif(value, ref);
      exif.push_back(std::make_pair(QString("GPS Longitude"), QString("%1º%2'%3'' %4").arg(std::abs(longitude.degrees()))
                                                                                      .arg(longitude.minutes())
                                                                                      .arg(longitude.seconds())
                                                                                      .arg(ref.c_str())));
    }

    value = image_metadata->metadata("EXIF_GPSAltitude", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Altitude"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSAltitudeRef", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Altitude Ref"), value.compare("0") == 0 ? QString("Above sea level") : QString("Below sea level")));
    }



    value = image_metadata->metadata("EXIF_GPSTimeStamp", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Time Stamp"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSSatellites", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Satellites"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSStatus", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Status"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSMeasureMode", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Measure Mode"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDOP", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS DOP"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSSpeedRef", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Speed Ref"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSSpeed", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Speed"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("GPS Track Ref", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Track Ref"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSTrack", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Track"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSImgDirectionRef", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Img Direction Ref"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSImgDirection", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Img Direction"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSMapDatum", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Map Datum"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestLatitudeRef", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Latitude Ref"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestLatitude", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Latitude"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestLongitudeRef", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Longitude Ref"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestLongitude", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Longitude"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestBearingRef", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Bearing Ref"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestBearing", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Bearing"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestDistanceRef", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Distance Ref"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDestDistance", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Dest Distance"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSProcessingMethod", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Processing Method"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSAreaInformation", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Area Information"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDateStamp", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Date Stamp"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSDifferential", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Differential"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSHPositioningError", active);
    if (active) {
      exif.push_back(std::make_pair(QString("GPS Horizontal Positioning Error"), QString::fromStdString(value)));
    }

    /// XMP

    value = image_metadata->metadata("XMP_AbsoluteAltitude", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Absolute Altitude"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_RelativeAltitude", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Relative Altitude"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_GimbalRollDegree", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Gimbal Roll Degree"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_GimbalYawDegree", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Gimbal Yaw Degree"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_GimbalPitchDegree", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Gimbal Pitch Degree"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_FlightRollDegree", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Flight Roll Degree"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_FlightYawDegree", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Flight Yaw Degree"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_FlightPitchDegree", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Flight Pitch Degree"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_CamReverse", active);
    if (active) {
      exif.push_back(std::make_pair(QString("CamReverse"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_GimbalReverse", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Gimbal Reverse"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("XMP_RtkFlag", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Rtk Flag"), QString::fromStdString(value)));
    }

    /// TIFF

    value = image_metadata->metadata("TIFFTAG_DOCUMENTNAME", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Document Name"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("TIFFTAG_IMAGEDESCRIPTION", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Image Description"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("TIFFTAG_SOFTWARE", active);
    if (active) {
      exif.push_back(std::make_pair(QString("Software"), QString::fromStdString(value)));
    }

    /// ...
    tl::MessageManager::resume();

    imageReader->close();
  }

  return exif;
}

bool MainWindowModel::isPhotoOriented(const QString &imgName) const
{
  return mProject->isPhotoOriented(imgName);
}

CameraPose MainWindowModel::cameraPose(const QString &imgName) const
{
  return mProject->photoOrientation(imgName);
}

Image MainWindowModel::findImageByName(const QString &imageName) const
{
  return mProject->findImageByName(imageName);
}

size_t MainWindowModel::imageID(const QString &imageName) const
{
  return mProject->imageId(imageName);
}

bool MainWindowModel::removeImage(const QString &imageName)
{
  Image img = mProject->findImageByName(imageName);
  std::vector<std::string> _images;
  _images.push_back(img.path().toStdString());
  colmapRemoveOrientations(_images, mProject->reconstructionPath().toStdString());
  return mProject->removeImage(imageID(imageName));
}

void MainWindowModel::removeImages(const QStringList &images)
{
  std::vector<std::string> _images(images.size());
  for (size_t i = 0; i < images.size(); i++) {
    _images[i] = mProject->findImageByName(images[i]).path().toStdString();
    mProject->removeImage(imageID(images[i]));
  }
  QString reconstruction_path = mProject->reconstructionPath();
  if (!reconstruction_path.isEmpty())
    colmapRemoveOrientations(_images, reconstruction_path.toStdString());
}

MainWindowModel::image_iterator MainWindowModel::imageBegin()
{
  return mProject->imageBegin();
}

MainWindowModel::image_const_iterator MainWindowModel::imageBegin() const
{
  return mProject->imageBegin();
}

MainWindowModel::image_iterator MainWindowModel::imageEnd()
{
  return mProject->imageEnd();
}

MainWindowModel::image_const_iterator MainWindowModel::imageEnd() const
{
  return mProject->imageEnd();
}

void MainWindowModel::init()
{
//  mPrjDefaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//  mPrjDefaultPath.append("/graphos/Projects");

//  QDir dir(mPrjDefaultPath);
//  if (!dir.exists()) {
//    dir.mkpath(".");
//  }
}

void MainWindowModel::clear()
{
}

} // namespace graphos



