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

#include "ImageLoaderModel.h"

#include "graphos/core/project.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Path.h>

namespace graphos
{

ImageLoaderModelImp::ImageLoaderModelImp(Project *project,
                                         QObject *parent)
  : ImageLoaderModel(parent),
    mProject(project)
{
    ImageLoaderModelImp::init();
}

//QString ImageLoaderModelImp::projectCRS() const
//{
//    return mProject->crs();
//}

//void ImageLoaderModelImp::setProjectCRS(const QString &crs)
//{
//    mProject->setCrs(crs);
//}

void ImageLoaderModelImp::addImage(const Image &image)
{
    mProject->addImage(image);
}

bool ImageLoaderModelImp::existImage(size_t imageId) const
{
    return mProject->existImage(imageId);
}

tl::Path ImageLoaderModelImp::imagesDirectory() const
{
    tl::Path image_directory = mProject->projectFolder();

    tl::Path path(image_directory);
    path.append("images");
    if (path.exists()) {
        image_directory = path;
    }

    return image_directory;
}

auto ImageLoaderModelImp::cameras() const -> const std::map<int, Camera>&
{
    return mProject->cameras();
}

auto ImageLoaderModelImp::addCamera(const Camera &camera) -> int
{
    return mProject->addCamera(camera);
}

auto ImageLoaderModelImp::cameraID(const Camera &camera) const -> int
{
    return mProject->cameraId(QString::fromStdString(camera.make()),
                              QString::fromStdString(camera.model()),
                              QString::fromStdString(camera.serialNumber()),
                              QString::fromStdString(camera.bandName()));
}

void ImageLoaderModelImp::init()
{
}

void ImageLoaderModelImp::clear()
{
}

} // namespace graphos
