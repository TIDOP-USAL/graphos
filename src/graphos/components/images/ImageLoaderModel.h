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

#ifndef GRAPHOS_LOADER_MODEL_INTERFACE_H
#define GRAPHOS_LOADER_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace tl
{
class Camera;
}

namespace graphos
{

class Image;

class ImageLoaderModel
  : public Model
{

  Q_OBJECT

public:

  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;
  typedef std::map<int, tl::Camera>::iterator camera_iterator;
  typedef std::map<int, tl::Camera>::const_iterator camera_const_iterator;
  
public:

  ImageLoaderModel(QObject *parent = nullptr) : Model(parent) {}
  ~ImageLoaderModel() override = default;

  virtual QString projectCRS() const = 0;
  virtual void setProjectCRS(const QString &crs) = 0;

  virtual void addImage(Image &image) = 0;
  virtual size_t imageID(const QString &imageName) const = 0;
  virtual Image findImageByName(const QString &imageName) const = 0;
  virtual Image findImageById(size_t id) const = 0;
  virtual bool updateImage(size_t id, const Image &image) = 0;
  virtual bool removeImage(size_t id) = 0;
  virtual bool removeImage(const Image &image) = 0;
  virtual QString imagesDirectory() const = 0;

  virtual image_iterator begin() = 0;
  virtual image_const_iterator begin() const = 0;
  virtual image_iterator end() = 0;
  virtual image_const_iterator end() const = 0;
  
  virtual int addCamera(const tl::Camera &camera) = 0;
  virtual int cameraID(const tl::Camera &camera) const = 0;
  virtual int cameraID(const QString &make, 
                       const QString &model) const = 0;
  virtual camera_iterator cameraBegin() = 0;
  virtual camera_const_iterator cameraBegin() const = 0;
  virtual camera_iterator cameraEnd() = 0;
  virtual camera_const_iterator cameraEnd() const = 0;
};

} // namespace graphos

#endif // GRAPHOS_LOADER_MODEL_INTERFACE_H
