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

#include "graphos/core/image.h"

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
