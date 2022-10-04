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

#ifndef GRAPHOS_LOADER_MODEL_INTERFACE_H
#define GRAPHOS_LOADER_MODEL_INTERFACE_H

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"
#include "graphos/core/camera/Camera.h"

namespace graphos
{

class Image;

class ImageLoaderModel
  : public Model
{

  Q_OBJECT

public:

  ImageLoaderModel(QObject *parent = nullptr) : Model(parent) {}
  ~ImageLoaderModel() override = default;

  virtual QString projectCRS() const = 0;
  virtual void setProjectCRS(const QString &crs) = 0;
  virtual void addImage(const Image &image) = 0;
  virtual bool existImage(size_t imageId) const = 0;
  virtual tl::Path imagesDirectory() const = 0;
  virtual const std::map<int, Camera> &cameras() const = 0;
  virtual int addCamera(const Camera &camera) = 0;
  virtual int cameraID(const Camera &camera) const = 0;
  virtual int cameraID(const QString &make, 
                       const QString &model) const = 0;
};

} // namespace graphos

#endif // GRAPHOS_LOADER_MODEL_INTERFACE_H
