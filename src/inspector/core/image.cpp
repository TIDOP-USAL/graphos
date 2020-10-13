#include "image.h"

#include <QFileInfo>

namespace inspector
{

CameraPosition::CameraPosition()
  : mCrs(""),
    mX(TL_DOUBLE_MAX),
    mY(TL_DOUBLE_MAX),
    mZ(TL_DOUBLE_MAX),
    mSource("")
{}


QString CameraPosition::crs() const
{
  return mCrs;
}

void CameraPosition::setCrs(const QString &crs)
{
  mCrs = crs;
}

double CameraPosition::x() const
{
  return mX;
}

void CameraPosition::setX(double x)
{
  mX = x;
}

double CameraPosition::y() const
{
  return mY;
}

void CameraPosition::setY(double y)
{
  mY = y;
}

double CameraPosition::z() const
{
  return mZ;
}

void CameraPosition::setZ(double z)
{
  mZ = z;
}

QString CameraPosition::source() const
{
  return mSource;
}

void CameraPosition::setSource(const QString &source)
{
  mSource = source;
}

bool CameraPosition::isEmpty() const
{
  return mX == TL_DOUBLE_MAX ||
         mY == TL_DOUBLE_MAX ||
         mZ == TL_DOUBLE_MAX ||
         mCrs == "";
}

//std::shared_ptr<tl::math::Rotation> CameraPosition::rotation() const
//{
//  return mRotation;
//}

//void CameraPosition::setRotation(std::shared_ptr<tl::math::Rotation> &rotation)
//{
//  mRotation = rotation;
//}



Image::Image()
  : mFilePath(""),
    mName(""),
    mCameraId(0)
{
}

Image::Image(const QString &file)
  : mFilePath(file),
    mCameraId(0)
{
  update();
}

Image::Image(const Image &image)
  : mFilePath(image.mFilePath),
    mName(image.mName),
    mCameraId(image.mCameraId),
    mCameraPosition(image.mCameraPosition)/*,
    mLongitudeExif(image.mLongitudeExif),
    mLatitudeExif(image.mLatitudeExif),
    mAltitudeExif(image.mAltitudeExif)*/
{

}

Image::Image(Image &&image) noexcept
  : mFilePath(std::move(image.mFilePath)),
    mName(std::move(image.mName)),
    mCameraId(std::exchange(image.mCameraId, 0)),
    mCameraPosition(std::move(image.mCameraPosition))/*,
    mLongitudeExif(image.mLongitudeExif),
    mLatitudeExif(image.mLatitudeExif),
    mAltitudeExif(image.mAltitudeExif)*/
{
//  image.mLongitudeExif = 0.;
//  image.mLatitudeExif = 0.;
//  image.mAltitudeExif = 0.;
}

QString Image::path() const
{
  return mFilePath;
}

void Image::setPath(const QString &file)
{
  mFilePath = file;
  update();
}

QString Image::name() const
{
  return mName;
}

int Image::cameraId() const
{
  return mCameraId;
}

void Image::setCameraId(int cameraId)
{
  mCameraId = cameraId;
}

CameraPosition Image::cameraPosition() const
{
  return mCameraPosition;
}

void Image::setCameraPosition(const CameraPosition &cameraPosition)
{
  mCameraPosition = cameraPosition;
}

//double Image::longitudeExif() const
//{
//  return mLongitudeExif;
//}

//void Image::setLongitudeExif(double longitudeExif)
//{
//  mLongitudeExif = longitudeExif;
//}

//double Image::latitudeExif() const
//{
//  return mLatitudeExif;
//}

//void Image::setLatitudeExif(double latitudeExif)
//{
//  mLatitudeExif = latitudeExif;
//}

//double Image::altitudeExif() const
//{
//  return mAltitudeExif;
//}

//void Image::setAltitudeExif(double altitudeExif)
//{
//  mAltitudeExif = altitudeExif;
//}

Image &Image::operator =(const Image &image)
{
  if (this != &image){
    this->mFilePath = image.mFilePath;
    this->mName = image.mName;
    this->mCameraId = image.mCameraId;
    this->mCameraPosition = image.mCameraPosition;
//    this->mLongitudeExif = image.mLongitudeExif;
//    this->mLatitudeExif = image.mLatitudeExif;
//    this->mAltitudeExif = image.mAltitudeExif;
  }
  return *this;
}

Image &Image::operator =(Image &&image) noexcept
{
  if (this != &image){
    this->mFilePath = std::move(image.mFilePath);
    this->mName = std::move(image.mName);
    this->mCameraId = std::exchange(image.mCameraId, 0);
    this->mCameraPosition = std::move(image.mCameraPosition);
//    this->mLongitudeExif = image.mLongitudeExif;
//    this->mLatitudeExif = image.mLatitudeExif;
//    this->mAltitudeExif = image.mAltitudeExif;
  }
  return *this;
}

void Image::update()
{
  QFileInfo file_info(mFilePath);
  mName = file_info.baseName();
}


} // namespace inspector
