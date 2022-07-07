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

#include "MainWindowModel.h"

#include "graphos/core/utils.h"
#include "graphos/core/sfm/orientationcolmap.h"

#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>


std::string removeParentheses(const std::string &value)
{
  size_t pos1 = value.find("(");
  size_t pos2 = value.find(")");

  std::string parse_value;

  if(pos1 != std::string::npos && pos2 != std::string::npos) {
    parse_value = value.substr(pos1 + 1, pos2 - (pos1+1));
  }

  return parse_value;
}


namespace graphos
{

MainWindowModel::MainWindowModel(Project *project)
  : mProject(project),
    bUnsavedChanges(false)
{
  this->init();
}

QString MainWindowModel::projectName() const
{
  return mProject->name();
}

QString MainWindowModel::projectPath() const
{
  return mProject->projectPath();
}

const std::unordered_map<size_t, Image> &MainWindowModel::images() const
{
  return mProject->images();
}

Image MainWindowModel::image(size_t imageId) const
{
  try {
    return mProject->findImageById(imageId);
  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
  }
}

void MainWindowModel::deleteImages(const std::vector<size_t> &imageIds)
{
  try {

    std::vector<std::string> images;

    for (auto imageId : imageIds) {
      images.push_back(mProject->findImageById(imageId).path().toStdString());
      mProject->removeImage(imageId);
    }

  QString reconstruction_path = mProject->reconstructionPath();
  if (!reconstruction_path.isEmpty())
    colmapRemoveOrientations(images, reconstruction_path.toStdString());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
  }
}

QImage MainWindowModel::readImage(size_t imageId)
{
  QImage image;

  try {

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(this->image(imageId).path().toStdWString());
    imageReader->open();
    if (imageReader->isOpen()) {

      /// Imagen georeferenciada.
      /// TODO: mostrar coordenadas en la barra de estado
      bool geo = imageReader->isGeoreferenced();

      cv::Mat bmp;

      tl::DataType data_type = imageReader->dataType();
      if (data_type == tl::DataType::TL_32F ||
          data_type == tl::DataType::TL_64F) {
        /// TODO: Aplicar paleta, mapa de sombras, etc, al DTM
      } else {
        bmp = imageReader->read();
      }

      image = cvMatToQImage(bmp);

      imageReader->close();
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
  }

  return image;
}

const std::unordered_map<size_t, QString> &MainWindowModel::features() const
{
  return mProject->features();
}

std::vector<size_t> MainWindowModel::imagePairs(size_t imageId) const
{
  std::vector<size_t> image_pairs = mProject->matchesPairs(imageId);
  return image_pairs;
}

QString MainWindowModel::sparseModel() const
{
  return mProject->sparseModel();
}

bool MainWindowModel::isAbsoluteOrientation() const
{
  return !mProject->offset().isEmpty();
}

std::unordered_map<QString, std::list<std::pair<QString, QString>>> MainWindowModel::exif(const QString &image) const
{

  std::unordered_map<QString, std::list<std::pair<QString, QString>>> exif;

  std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(image.toStdString());
  imageReader->open();
  if(imageReader->isOpen()) {

    std::shared_ptr<tl::ImageMetadata> image_metadata = imageReader->metadata();
    std::map<std::string, std::string> metadata = image_metadata->activeMetadata();
    std::string name;
    std::string value;

    std::map<std::string, std::string> values;

    tl::MessageManager::pause();

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
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Camera make"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_Model", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Camera model"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_FNumber", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("F Number"), QString::fromStdString("f/" + removeParentheses(value))));
    }

    value = image_metadata->metadata("EXIF_ExposureTime", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Exposure time"), QString::fromStdString(removeParentheses(value))));
    }
    
    //value = image_metadata->metadata("EXIF_ISOSpeed", active);
    //if(active) {
    //  exif["Camera"].push_back(std::make_pair(QString("ISO speed"), QString::fromStdString(value)));
    //}
    
    value = image_metadata->metadata("EXIF_ISOSpeedRatings", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("ISO Speed"), QString::fromStdString("ISO-"+value)));
    }

    value = image_metadata->metadata("EXIF_ExposureBiasValue", active);
    if(active) {
      value = removeParentheses(value);
      double exposure_bias = std::stod(value);
      exif["Camera"].push_back(std::make_pair(QString("Exposure bias"), QString::number(exposure_bias, 'f', 1) + " step"));
    }

    value = image_metadata->metadata("EXIF_FocalLength", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Focal length"), QString::fromStdString(removeParentheses(value) + " mm")));
    }

    value = image_metadata->metadata("EXIF_ApertureValue", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Aperture value"), QString::fromStdString(removeParentheses(value))));
    }
    
    value = image_metadata->metadata("EXIF_MaxApertureValue", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Max aperture"), QString::fromStdString(removeParentheses(value))));
    }
    
    value = image_metadata->metadata("EXIF_FocalLengthIn35mmFilm", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Focal Length In 35 mm"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_LensSpecification", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Lens Specification"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_LensMake", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Lens Make"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_LensModel", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Lens Model"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_LensSerialNumber", active);
    if(active) {
      exif["Camera"].push_back(std::make_pair(QString("Lens Serial Number"), QString::fromStdString(value)));
    }

    // Image

    {
      bool active1 = false;
      bool active2 = false;
      std::string x_dimension = image_metadata->metadata("EXIF_PixelXDimension", active1);
      std::string y_dimension = image_metadata->metadata("EXIF_PixelYDimension", active2);
      if(active1 && active2) {
        exif["Image"].push_back(std::make_pair(QString("Dimensions"), QString::fromStdString(x_dimension + " x " + y_dimension)));
        exif["Image"].push_back(std::make_pair(QString("Width"), QString::fromStdString(x_dimension + " pixels")));
        exif["Image"].push_back(std::make_pair(QString("Height"), QString::fromStdString(y_dimension + " pixels")));
      }
    }

    {
      std::string resolution_unit;
      value = image_metadata->metadata("EXIF_ResolutionUnit", active);
      if(active) {
        if(value == "2") {
          resolution_unit = "dpi";
        } else if(value == "3") {
          resolution_unit = "dpc";
        }
        //exif.push_back(std::make_pair(QString("Resolution Unit"), QString::fromStdString(value)));
      }

      bool active1 = false;
      bool active2 = false;
      std::string x_resolution = image_metadata->metadata("EXIF_XResolution", active1);
      std::string y_resolution = image_metadata->metadata("EXIF_YResolution", active2);

      if(active1 && active2) {
        exif["Image"].push_back(std::make_pair(QString("Horizontal resolution"), QString::fromStdString(removeParentheses(x_resolution) + " " + resolution_unit)));
        exif["Image"].push_back(std::make_pair(QString("Vertical resolution"), QString::fromStdString(removeParentheses(y_resolution) + " " + resolution_unit)));
      }

    }

    value = image_metadata->metadata("EXIF_Orientation", active);
    if(active) {
      QString orientation;
      if(value == "1") {
        orientation = "Horizontal";
      } else if(value == "2") {
        orientation = "Mirror horizontal";
      } else if(value == "3") {
        orientation = "Rotate 180";
      } else if(value == "4") {
        orientation = "Mirror vertical";
      } else if(value == "5") {
        orientation = "Mirror horizontal and rotate 270 CW";
      } else if(value == "6") {
        orientation = "Rotate 90 CW";
      } else if(value == "7") {
        orientation = "Mirror horizontal and rotate 90 CW";
      } else if(value == "8") {
        orientation = "Rotate 270 CW";
      }

      exif["Image"].push_back(std::make_pair(QString("Orientation"), orientation));
    }

    value = image_metadata->metadata("EXIF_CompressedBitsPerPixel", active);
    if(active) {
      exif["Image"].push_back(std::make_pair(QString("Compressed bits per pixel"), QString::fromStdString(removeParentheses(value))));
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
    //value = image_metadata->metadata("EXIF_ExposureProgram", active);
    //if(active) {
    //  exif.push_back(std::make_pair(QString("Exposure Program"), QString::fromStdString(value)));
    //}
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
    if(active) {
      exif["Origin"].push_back(std::make_pair(QString("Date Time Original"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_DateTimeDigitized", active);
    if(active) {
      exif["Origin"].push_back(std::make_pair(QString("Date Time Digitized"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_OffsetTime", active);
    if(active) {
      exif["Origin"].push_back(std::make_pair(QString("Offset Time"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_OffsetTimeOriginal", active);
    if(active) {
      exif["Origin"].push_back(std::make_pair(QString("Offset Time Original"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_OffsetTimeDigitized", active);
    if(active) {
      exif["Origin"].push_back(std::make_pair(QString("Offset Time Digitized"), QString::fromStdString(value)));
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
    //value = image_metadata->metadata("EXIF_ExposureMode", active);
    //if(active) {
    //  exif.push_back(std::make_pair(QString("Exposure Mode"), QString::fromStdString(value)));
    //}
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
    //value = image_metadata->metadata("EXIF_FocalLengthIn35mmFilm", active);
    //if(active) {
    //  exif.push_back(std::make_pair(QString("Focal Length In 35 mm Film"), QString::fromStdString(value)));
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
    if(active) {
      std::string ref = image_metadata->metadata("EXIF_GPSLatitudeRef", active);
      tl::math::Degrees<double> latitude = formatDegreesFromExif(value, ref);
      exif["GPS"].push_back(std::make_pair(
                            QString("GPS Latitude"), 
                            QString("%1º%2'%3'' %4").arg(std::abs(latitude.degrees()))
                            .arg(latitude.minutes())
                            .arg(latitude.seconds())
                            .arg(ref.c_str())));
    }
    
    value = image_metadata->metadata("EXIF_GPSLongitude", active);
    if(active) {
      std::string ref = image_metadata->metadata("EXIF_GPSLongitudeRef", active);
      tl::math::Degrees<double> longitude = formatDegreesFromExif(value, ref);
      exif["GPS"].push_back(std::make_pair(
                            QString("GPS Longitude"), 
                            QString("%1º%2'%3'' %4")
                            .arg(std::abs(longitude.degrees()))
                            .arg(longitude.minutes())
                            .arg(longitude.seconds())
                            .arg(ref.c_str())));
    }
    
    value = image_metadata->metadata("EXIF_GPSAltitude", active);
    if(active) {
      std::string altitude = removeParentheses(value);
      value = image_metadata->metadata("EXIF_GPSAltitudeRef", active);
      if("0x00" == 0)
        altitude.append(" Above sea level");
      else
        altitude.append(" Below sea level");
      value = image_metadata->metadata("EXIF_GPSAltitudeRef", active);
      exif["GPS"].push_back(std::make_pair(QString("GPS Altitude"), QString::fromStdString(altitude)));
    }
   
    value = image_metadata->metadata("EXIF_GPSTimeStamp", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Time Stamp"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSSatellites", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Satellites"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSStatus", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Status"), QString::fromStdString(value)));
    }

    value = image_metadata->metadata("EXIF_GPSMeasureMode", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Measure Mode"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDOP", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS DOP"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSSpeedRef", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Speed Ref"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSSpeed", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Speed"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("GPS Track Ref", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Track Ref"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSTrack", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Track"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSImgDirectionRef", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Img Direction Ref"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSImgDirection", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Img Direction"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSMapDatum", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Map Datum"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestLatitudeRef", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Latitude Ref"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestLatitude", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Latitude"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestLongitudeRef", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Longitude Ref"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestLongitude", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Longitude"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestBearingRef", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Bearing Ref"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestBearing", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Bearing"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestDistanceRef", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Distance Ref"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDestDistance", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Dest Distance"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSProcessingMethod", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Processing Method"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSAreaInformation", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Area Information"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDateStamp", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Date Stamp"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSDifferential", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Differential"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("EXIF_GPSHPositioningError", active);
    if(active) {
      exif["GPS"].push_back(std::make_pair(QString("GPS Horizontal Positioning Error"), QString::fromStdString(value)));
    }
    
    /// XMP
    
    value = image_metadata->metadata("XMP_AbsoluteAltitude", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Absolute Altitude"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_RelativeAltitude", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Relative Altitude"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_GimbalRollDegree", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Gimbal Roll Degree"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_GimbalYawDegree", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Gimbal Yaw Degree"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_GimbalPitchDegree", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Gimbal Pitch Degree"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_FlightRollDegree", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Flight Roll Degree"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_FlightYawDegree", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Flight Yaw Degree"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_FlightPitchDegree", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Flight Pitch Degree"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_CamReverse", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("CamReverse"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_GimbalReverse", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Gimbal Reverse"), QString::fromStdString(value)));
    }
    
    value = image_metadata->metadata("XMP_RtkFlag", active);
    if(active) {
      exif["XMP"].push_back(std::make_pair(QString("Rtk Flag"), QString::fromStdString(value)));
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
    tl::MessageManager::resume();

    imageReader->close();
  }

  return exif;
}

const std::unordered_map<size_t, CameraPose> &MainWindowModel::poses() const
{
  return mProject->poses();
}

QString MainWindowModel::denseModel() const
{
  return mProject->denseModel();
}

bool MainWindowModel::checkUnsavedChanges() const
{
  return bUnsavedChanges;
}

bool MainWindowModel::checkOldVersion(const QString &file) const
{
  return mProject->checkOldVersion(file);
}

void MainWindowModel::oldVersionBackup(const QString &file) const
{
  mProject->oldVersionBak(file);
}

void MainWindowModel::load(const QString &file)
{
  mProject->load(file);
  bUnsavedChanges = false;
}

void MainWindowModel::save()
{
  try {

    saveAs(mProject->projectPath());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
  }
}

void MainWindowModel::saveAs(const QString &file)
{
  mProject->save(file);

  bUnsavedChanges = false;
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
  mProject->clear();
}


} // namespace graphos



