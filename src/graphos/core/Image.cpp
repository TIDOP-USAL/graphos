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

#include "graphos/core/Image.h"


namespace graphos
{

Image::Image(tl::Path file)
  : mFilePath(std::move(file))
{
}

auto Image::path() const -> const tl::Path &
{
    return mFilePath;
}

void Image::setPath(tl::Path file)
{
    mFilePath = std::move(file);
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

auto Image::cameraPose() -> CameraPose &
{
    return mCameraPose;
}

auto Image::cameraPose() const -> const CameraPose &
{
    return mCameraPose;
}

//void Image::setCameraPose(const CameraPose &cameraPose)
//{
//    mCameraPose = cameraPose;
//}

void Image::addMetadata(std::string key, std::string value)
{
    mMetadata[std::move(key)] = std::move(value);
}

auto Image::hasMetadata(std::string_view key) const -> bool
{
    return mMetadata.contains(key);
}

auto Image::metadata(std::string_view key) const -> std::string
{
    if (auto it = mMetadata.find(key); it != mMetadata.end()) {
        return it->second;
    }
    return {};
}

auto Image::metadata() const -> const Metadata &
{
    return mMetadata;
}

auto Image::id(const Image &image) -> size_t
{
    return tl::Path::hash(image.path());
}

} // namespace graphos
