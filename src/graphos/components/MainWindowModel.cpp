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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "MainWindowModel.h"

#include "graphos/core/orientation/orientationcolmap.h"

#include <tidop/core/messages.h>

#include <easyexif/exif.h>

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
  tl::MessageManager::pause();

  std::list<std::pair<QString, QString>> exif;

  QByteArray ba = image.toLocal8Bit();
  const char *img_file = ba.constData();

  /// TODO: leer con GDAL
  QFileInfo file_info(image);
  if (file_info.suffix().compare("tif", Qt::CaseInsensitive) == 0){
    QImage qImage(image);
    if (qImage.isNull()) return exif;
    exif.push_back(std::make_pair(QString("Bits per sample"), QString::number(qImage.depth())));
    exif.push_back(std::make_pair(QString("Image width"), QString::number(qImage.width())));
    exif.push_back(std::make_pair(QString("Image height"), QString::number(qImage.height())));
    //exif[QString("Bits per sample")] = QString::number(qImage.depth());
    //exif[QString("Image width")] = QString::number(qImage.width());
    //exif[QString("Image height")] = QString::number(qImage.height());
  } else {
    // Read the JPEG file into a buffer
    FILE *fp = fopen(img_file, "rb");
    if (!fp) {
      msgError("Can't open file", img_file);
      return exif;
    }
    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp);
    rewind(fp);
    unsigned char *buf = new unsigned char[fsize];
    if (fread(buf, 1, fsize, fp) != fsize) {
      msgError("Can't read file", img_file);
      delete[] buf;
      return exif;
    }
    fclose(fp);

    // Parse EXIF
    easyexif::EXIFInfo result;
    int code = result.parseFrom(buf, fsize);
    delete[] buf;
    if (code) {
      msgWarning("Error parsing EXIF: code %d\n", code);
      return exif;
    }
    exif.push_back(std::make_pair(QString("Camera make"), QString(result.Make.c_str())));
    exif.push_back(std::make_pair(QString("Camera model"), QString(result.Model.c_str())));
    exif.push_back(std::make_pair(QString("Software"), QString(result.Software.c_str())));
    exif.push_back(std::make_pair(QString("Bits per sample"), QString::number(result.BitsPerSample)));
    exif.push_back(std::make_pair(QString("Image width"), QString::number(result.ImageWidth)));
    exif.push_back(std::make_pair(QString("Image height"), QString::number(result.ImageHeight)));
    exif.push_back(std::make_pair(QString("Image description"), QString(result.ImageDescription.c_str())));
    exif.push_back(std::make_pair(QString("Image orientation"), QString::number(result.Orientation)));
    exif.push_back(std::make_pair(QString("Image copyright"), QString(result.Copyright.c_str())));
    exif.push_back(std::make_pair(QString("Image date/time"), QString(result.DateTime.c_str())));
    exif.push_back(std::make_pair(QString("Original date/time"), QString(result.DateTimeOriginal.c_str())));
    exif.push_back(std::make_pair(QString("Digitize date/time"), QString("1/%1").arg(static_cast<int>(1.0 / result.ExposureTime))));
    exif.push_back(std::make_pair(QString("Exposure time"), QString(result.DateTimeDigitized.c_str())));
    exif.push_back(std::make_pair(QString("F-stop"), QString("f/%1").arg(result.FNumber, 0, 'g', 1)));
    exif.push_back(std::make_pair(QString("ISO speed"), QString::number(result.ISOSpeedRatings)));
    exif.push_back(std::make_pair(QString("Subject distance"), QString::number(result.SubjectDistance).append(" m")));
    exif.push_back(std::make_pair(QString("Exposure bias"), QString::number(result.ExposureBiasValue).append(" EV")));
    exif.push_back(std::make_pair(QString("Lens focal length"), QString::number(result.FocalLength).append(" mm")));
    exif.push_back(std::make_pair(QString("35mm focal length"), QString::number(result.FocalLengthIn35mm).append(" mm")));
    exif.push_back(std::make_pair(QString("GPS Latitude"), QString("%1º%2'%3'' %4").arg(result.GeoLocation.LatComponents.degrees)
                                                            .arg(result.GeoLocation.LatComponents.minutes)
                                                            .arg(result.GeoLocation.LatComponents.seconds)
                                                            .arg(result.GeoLocation.LatComponents.direction)));
    exif.push_back(std::make_pair(QString("GPS Longitude"), QString("%1º%2'%3'' %4").arg(result.GeoLocation.LonComponents.degrees)
                                                             .arg(result.GeoLocation.LonComponents.minutes)
                                                             .arg(result.GeoLocation.LonComponents.seconds)
                                                             .arg(result.GeoLocation.LonComponents.direction)));
    exif.push_back(std::make_pair(QString("GPS Altitude"), QString::number(result.GeoLocation.Altitude).append(" m")));
    exif.push_back(std::make_pair(QString("GPS Precision (DOP)"), QString::number(result.GeoLocation.DOP)));
    exif.push_back(std::make_pair(QString("Lens min focal length"), QString::number(result.LensInfo.FocalLengthMin).append(" mm")));
    exif.push_back(std::make_pair(QString("Lens max focal length"), QString::number(result.LensInfo.FocalLengthMax).append(" mm")));
    exif.push_back(std::make_pair(QString("Lens f-stop min"), QString("f/%1").arg(result.LensInfo.FStopMin, 0, 'g', 1)));
    exif.push_back(std::make_pair(QString("Lens f-stop max"), QString("f/%1").arg(result.LensInfo.FStopMax, 0, 'g', 1)));
    exif.push_back(std::make_pair(QString("Lens make"), QString(result.LensInfo.Make.c_str())));
    exif.push_back(std::make_pair(QString("Lens model"), QString(result.LensInfo.Model.c_str())));
    exif.push_back(std::make_pair(QString("Focal plane XRes"), QString::number(result.LensInfo.FocalPlaneXResolution)));
    exif.push_back(std::make_pair(QString("Focal plane YRes"), QString::number(result.LensInfo.FocalPlaneYResolution)));
  }

  tl::MessageManager::resume();

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

//bool MainWindowModel::removeImage(size_t id)
//{
//  ///TODO: Esto esta fatal...
//  Image img = mProject->findImageById(id);
//  colmapRemoveOrientation(img.path().toStdString(), mProject->reconstructionPath().toStdString());
//  return mProject->removeImage(id);
//}

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


