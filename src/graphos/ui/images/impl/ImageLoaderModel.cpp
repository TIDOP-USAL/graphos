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

#include "ImageLoaderModel.h"

#include <tidop/core/messages.h>
#include <tidop/core/path.h>

#include "graphos/core/project.h"

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

void ImageLoaderModelImp::addImage(Image &image)
{
  mProject->addImage(image);
}

size_t ImageLoaderModelImp::imageID(const QString &imageName) const
{
  return mProject->imageId(imageName);
}

Image ImageLoaderModelImp::findImageByName(const QString &imageName) const
{
  return mProject->findImageByName(imageName);
}

Image ImageLoaderModelImp::findImageById(size_t id) const
{
  return mProject->findImageById(id);
}

bool ImageLoaderModelImp::updateImage(size_t id, const Image &image)
{
  return mProject->updateImage(id, image);
}

bool ImageLoaderModelImp::removeImage(size_t id)
{
  return mProject->removeImage(id);
}

bool ImageLoaderModelImp::removeImage(const Image &image)
{
  size_t id_image = imageID(image.name());
  return mProject->removeImage(id_image);
}

QString ImageLoaderModelImp::imagesDirectory() const
{
  QString image_directory = mProject->projectFolder();
  tl::Path path(mProject->projectFolder().toStdString());
  path.append("images");
  if (path.exists()) {
    image_directory = path.toString().c_str();
  }
  return image_directory;
}

ImageLoaderModel::image_iterator ImageLoaderModelImp::begin()
{
  return mProject->imageBegin();
}

ImageLoaderModel::image_const_iterator ImageLoaderModelImp::begin() const
{
  return mProject->imageBegin();
}

ImageLoaderModel::image_iterator ImageLoaderModelImp::end()
{
  return mProject->imageEnd();
}

ImageLoaderModel::image_const_iterator ImageLoaderModelImp::end() const
{
  return mProject->imageEnd();
}

int ImageLoaderModelImp::addCamera(const Camera &camera)
{
  return mProject->addCamera(camera);
}

int ImageLoaderModelImp::cameraID(const Camera &camera) const
{
  return cameraID(camera.make(), camera.model());
}

int ImageLoaderModelImp::cameraID(const QString &make, 
                                  const QString &model) const
{
  int id_camera = 0;
  for (auto it = cameraBegin(); it != cameraEnd(); it++){
    QString camera_make = it->second.make();
    QString camera_model = it->second.model();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      id_camera = it->first;
      break;
    }
  }
  return id_camera;
}

ImageLoaderModel::camera_iterator ImageLoaderModelImp::cameraBegin()
{
  return mProject->cameraBegin();
}

ImageLoaderModel::camera_const_iterator ImageLoaderModelImp::cameraBegin() const
{
  return mProject->cameraBegin();
}

ImageLoaderModel::camera_iterator ImageLoaderModelImp::cameraEnd()
{
  return mProject->cameraEnd();
}

ImageLoaderModel::camera_const_iterator ImageLoaderModelImp::cameraEnd() const
{
  return mProject->cameraEnd();
}

void ImageLoaderModelImp::init()
{
}

void ImageLoaderModelImp::clear()
{
  mProject->imagesCount();
  for (auto it = begin(); it != end();){
    Image image = *it;
    removeImage(image);
    it = begin();
  }
}

} // namespace graphos
