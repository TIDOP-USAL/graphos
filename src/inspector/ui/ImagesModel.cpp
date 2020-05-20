#include "ImagesModel.h"

#include <tidop/core/messages.h>

#include <colmap/util/bitmap.h>


//#include <QSqlQuery>
//#include <QSqlError>
//#include <QApplication>
//#include <QFileInfo>

namespace inspector
{

namespace ui
{

ImagesModelImp::ImagesModelImp(Project *project,
                               QObject *parent)
  : ImagesModel(parent),
    mProject(project)
{
  init();
}

void ImagesModelImp::addImage(const QString &imagePath, size_t cameraId)
{
  colmap::Bitmap bitmap;

  Image img(imagePath);

  if (!bitmap.Read(imagePath.toStdString(), false)) {
    TL_TODO("Devolver una excepción")
    msgError("  Failed to read image file");
    return;
  } else {
    msgInfo("Read image: %s", imagePath.toStdString().c_str());
  }

  double lon;
  if (bitmap.ExifLongitude(&lon)){
    img.setLongitudeExif(lon);
  }
  double lat;
  if (bitmap.ExifLatitude(&lat)){
    img.setLatitudeExif(lat);
  }
  double altitude;
  if (bitmap.ExifAltitude(&altitude)){
    img.setAltitudeExif(altitude);
  }

  msgInfo(" - Coordinates: (%.4lf, %.4lf, %.2lf)", img.longitudeExif(), img.latitudeExif(), img.altitudeExif());

  img.setCameraId(cameraId);

  addImage(img);
}

void ImagesModelImp::addImage(Image &image)
{
  mProject->addImage(image);
}

size_t ImagesModelImp::imageID(const QString &imageName) const
{
  return mProject->imageId(imageName);
}

Image ImagesModelImp::findImageByName(const QString &imageName) const
{
  return mProject->findImageByName(imageName);
}

Image ImagesModelImp::findImageById(size_t id) const
{
  return mProject->findImageById(id);
}

bool ImagesModelImp::updateImage(size_t id, const Image &image)
{
  return mProject->updateImage(id, image);
}

bool ImagesModelImp::removeImage(size_t id)
{
  return mProject->removeImage(id);
}

bool ImagesModelImp::removeImage(const Image &image)
{
  size_t id_image = imageID(image.name());
  return mProject->removeImage(id_image);
}

ImagesModel::image_iterator ImagesModelImp::begin()
{
  return mProject->imageBegin();
}

ImagesModel::image_const_iterator ImagesModelImp::begin() const
{
  return mProject->imageBegin();
}

ImagesModel::image_iterator ImagesModelImp::end()
{
  return mProject->imageEnd();
}

ImagesModel::image_const_iterator ImagesModelImp::end() const
{
  return mProject->imageEnd();
}

void ImagesModelImp::init()
{
}

void ImagesModelImp::clear()
{
  mProject->imagesCount();
  for (auto it = begin(); it != end();){
    Image image = *it;
    removeImage(image);
    it = begin();
  }
}

} // namespace ui

} // namespace inspector
