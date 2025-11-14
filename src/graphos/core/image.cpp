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
    mCameraId(0),
    mCameraPose()
{
}

Image::Image(const QString &file)
  : mFilePath(file.toStdString()),
    mCameraId(0),
    mCameraPose()
{
}

Image::Image(tl::Path file)
  : mFilePath(std::move(file)),
    mCameraId(0),
    mCameraPose()
{
}

Image::Image(const Image &image)
  : mFilePath(image.mFilePath),
    mCameraId(image.mCameraId),
    mCameraPose(image.mCameraPose),
    mMetadata(image.mMetadata)
{

}

Image::Image(Image &&image) noexcept
  : mFilePath(std::move(image.mFilePath)),
    mCameraId(std::exchange(image.mCameraId, 0)),
    mCameraPose(std::move(image.mCameraPose)),
    mMetadata(std::move(image.mMetadata))
{
}

auto Image::path() const -> QString
{
    return QString::fromStdString(mFilePath.toUtf8());
}

void Image::setPath(const QString &file)
{
    mFilePath = file.toStdString();
}

void Image::setPath(const tl::Path &file)
{
    mFilePath = file;
}

auto Image::name() const -> QString
{
    return QString::fromStdString(mFilePath.fileName().toUtf8());
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

void Image::addMetadata(const std::string &key, const std::string &value)
{
    mMetadata[key] = value;
}

auto Image::hasMetadata(const std::string &key) const -> bool
{
    return mMetadata.find(key) != mMetadata.end();
}

auto Image::metadata(const std::string &key) const -> std::string
{
    auto it = mMetadata.find(key);
    if (it != mMetadata.end()) {
        return it->second;
    }
    return std::string();
}

auto Image::metadata() const -> const std::map<std::string, std::string> &
{
    return mMetadata;
}

auto Image::operator =(const Image& image) -> Image&
{
    if (this != &image) {
        this->mFilePath = image.mFilePath;
        this->mCameraId = image.mCameraId;
        this->mCameraPose = image.mCameraPose;
        this->mMetadata = image.mMetadata;
    }
    return *this;
}

auto Image::operator =(Image&& image) noexcept -> Image&
{
    if (this != &image) {
        this->mFilePath = std::move(image.mFilePath);
        this->mCameraId = std::exchange(image.mCameraId, 0);
        this->mCameraPose = std::move(image.mCameraPose);
        this->mMetadata = std::move(image.mMetadata);
    }
    return *this;
}

auto Image::id(const Image &image) -> size_t
{
    return tl::Path::hash(image.path().toStdString());
}


} // namespace graphos
