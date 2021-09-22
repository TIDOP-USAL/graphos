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

#ifndef GRAPHOS_ORIENTATION_MODEL_INTERFACE_H
#define GRAPHOS_ORIENTATION_MODEL_INTERFACE_H

#include <tidop/geospatial/camera.h>

#include "graphos/interfaces/mvp.h"
#include "graphos/core/image.h"
#include "graphos/core/orientation/photoorientation.h"

namespace graphos
{

class OrientationModel
  : public Model
{

public:

  typedef std::map<int, tl::Camera>::iterator camera_iterator;
  typedef std::map<int, tl::Camera>::const_iterator camera_const_iterator;
  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;

public:

  OrientationModel(QObject *parent = nullptr) : Model(parent) {}
  ~OrientationModel() override = default;

  virtual bool calibratedCamera() const = 0;
  virtual bool refinePrincipalPoint() const = 0;
  virtual void setRefinePrincipalPoint(bool refine) = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual void setOffset(const QString &offset) = 0;
  virtual bool isPhotoOriented(const QString &imgName) const = 0;
  virtual CameraPose photoOrientation(const QString &imgName) const = 0;
  virtual void addPhotoOrientation(const QString &imgName, 
                                   const CameraPose &orientation) = 0;
  virtual QString database() const = 0;
  virtual QString projectPath() const = 0;
  virtual bool gpsPositions() const = 0;
  virtual bool rtkOrientations() const = 0;
  virtual QString reconstructionPath() const = 0;
  virtual void setReconstructionPath(const QString &reconstructionPath) = 0;
  virtual std::map<QString, std::array<double, 3>> cameraPositions() const = 0;
  virtual void clearProject() = 0;

  virtual std::map<int, tl::Camera> cameras() const = 0;
  virtual bool updateCamera(int id, const tl::Camera &camera) = 0;
  virtual camera_iterator cameraBegin() = 0;
  virtual camera_const_iterator cameraBegin() const = 0;
  virtual camera_iterator cameraEnd() = 0;
  virtual camera_const_iterator cameraEnd() const = 0;
  
  virtual std::vector<Image> images() const = 0;
  virtual image_iterator imageBegin() = 0;
  virtual image_const_iterator imageBegin() const = 0;
  virtual image_iterator imageEnd() = 0;
  virtual image_const_iterator imageEnd() const = 0;
};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_MODEL_INTERFACE_H
