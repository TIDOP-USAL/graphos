#include "image.h"

#include <QFileInfo>

namespace graphos
{


Image::Image()
  : mFilePath(""),
    mName(""),
    mCameraId(0),
    mCameraPose()
{
}

Image::Image(const QString &file)
  : mFilePath(file),
    mCameraId(0),
    mCameraPose()
{
  update();
}

Image::Image(const Image &image)
  : mFilePath(image.mFilePath),
    mName(image.mName),
    mCameraId(image.mCameraId),
    mCameraPose(image.mCameraPose)
{

}

Image::Image(Image &&image) noexcept
  : mFilePath(std::move(image.mFilePath)),
    mName(std::move(image.mName)),
    mCameraId(std::exchange(image.mCameraId, 0)),
    mCameraPose(std::move(image.mCameraPose))
{
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

CameraPose Image::cameraPose() const
{
  return mCameraPose;
}

void Image::setCameraPose(const CameraPose &cameraPose)
{
  mCameraPose = cameraPose;
}

Image &Image::operator =(const Image &image)
{
  if (this != &image){
    this->mFilePath = image.mFilePath;
    this->mName = image.mName;
    this->mCameraId = image.mCameraId;
    this->mCameraPose = image.mCameraPose;
  }
  return *this;
}

Image &Image::operator =(Image &&image) noexcept
{
  if (this != &image){
    this->mFilePath = std::move(image.mFilePath);
    this->mName = std::move(image.mName);
    this->mCameraId = std::exchange(image.mCameraId, 0);
    this->mCameraPose = std::move(image.mCameraPose);
  }
  return *this;
}

void Image::update()
{
  QFileInfo file_info(mFilePath);
  mName = file_info.baseName();
}


} // namespace graphos
