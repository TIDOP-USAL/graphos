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

#include "graphos/core/image.h"


namespace graphos
{


Image::Image()
  : mFilePath(""),
    mId(0),
    mCameraId(0),
    mCameraPose()
{
}

Image::Image(const QString &file)
  : mFilePath(file.toStdWString()),
    mId(0),
    mCameraId(0),
    mCameraPose()
{
    update();
}

Image::Image(tl::Path file)
  : mFilePath(std::move(file)),
    mId(0),
    mCameraId(0),
    mCameraPose()
{
    update();
}

Image::Image(const Image &image)
  : mFilePath(image.mFilePath),
    mId(image.mId),
    mCameraId(image.mCameraId),
    mCameraPose(image.mCameraPose)
{

}

Image::Image(Image &&image) noexcept
  : mFilePath(std::move(image.mFilePath)),
    mId(std::exchange(image.mId, 0)),
    mCameraId(std::exchange(image.mCameraId, 0)),
    mCameraPose(std::move(image.mCameraPose))
{
}

auto Image::path() const -> QString
{
    return QString::fromStdWString(mFilePath.toWString());
}

void Image::setPath(const QString &file)
{
    mFilePath = file.toStdWString();
    update();
}

void Image::setPath(const tl::Path &file)
{
    mFilePath = file;
}

auto Image::name() const -> QString
{
    return QString::fromStdWString(mFilePath.fileName().toWString());
}

auto Image::id() const -> size_t
{
    return mId;
}

auto Image::cameraId() const -> int
{
    return mCameraId;
}

void Image::setCameraId(int cameraId)
{
    mCameraId = cameraId;
}

auto Image::cameraPose() const -> CameraPose
{
    return mCameraPose;
}

void Image::setCameraPose(const CameraPose &cameraPose)
{
    mCameraPose = cameraPose;
}

auto Image::operator =(const Image& image) -> Image&
{
    if (this != &image) {
        this->mFilePath = image.mFilePath;
        this->mId = image.mId;
        this->mCameraId = image.mCameraId;
        this->mCameraPose = image.mCameraPose;
    }
    return *this;
}

auto Image::operator =(Image&& image) noexcept -> Image&
{
    if (this != &image) {
        this->mFilePath = std::move(image.mFilePath);
        this->mId = std::exchange(image.mId, 0);
        this->mCameraId = std::exchange(image.mCameraId, 0);
        this->mCameraPose = std::move(image.mCameraPose);
    }
    return *this;
}

void Image::update()
{
    mId = tl::Path::hash(mFilePath);
}


} // namespace graphos
