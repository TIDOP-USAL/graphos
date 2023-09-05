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

#include <tidop/core/msg/message.h>
#include <tidop/core/path.h>

namespace graphos
{

ImageLoaderModelImp::ImageLoaderModelImp(Project *project,
                                         QObject *parent)
  : ImageLoaderModel(parent),
    mProject(project)
{
    init();
}

QString ImageLoaderModelImp::projectCRS() const
{
    return mProject->crs();
}

void ImageLoaderModelImp::setProjectCRS(const QString &crs)
{
    mProject->setCrs(crs);
}

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

    tl::Path path(mProject->projectFolder());
    path.append("images");
    if (path.exists()) {
        image_directory = path;
    }

    return image_directory;
}

const std::map<int, Camera> &ImageLoaderModelImp::cameras() const
{
    return mProject->cameras();
}

int ImageLoaderModelImp::addCamera(const Camera &camera)
{
    return mProject->addCamera(camera);
}

int ImageLoaderModelImp::cameraID(const Camera &camera) const
{
    return cameraID(camera.make().c_str(), camera.model().c_str());
}

int ImageLoaderModelImp::cameraID(const QString &make,
                                  const QString &model) const
{
    int id_camera = 0;
    for (const auto &camera : mProject->cameras()) {

        QString camera_make = camera.second.make().c_str();
        QString camera_model = camera.second.model().c_str();
        if (make.compare(camera_make) == 0 &&
            model.compare(camera_model) == 0) {
            id_camera = camera.first;
            break;
        }
    }
    return id_camera;
}

void ImageLoaderModelImp::init()
{
}

void ImageLoaderModelImp::clear()
{
}

} // namespace graphos
