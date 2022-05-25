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

#ifndef GRAPHOS_ORIENTATION_MODEL_INTERFACE_H
#define GRAPHOS_ORIENTATION_MODEL_INTERFACE_H

#include <unordered_map>

#include "graphos/interfaces/mvp.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/poses.h"

namespace graphos
{

class Camera;

class OrientationModel
  : public Model
{

public:

  OrientationModel(QObject *parent = nullptr) : Model(parent) {}
  ~OrientationModel() override = default;

  virtual bool calibratedCamera() const = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual void setOffset(const QString &offset) = 0;
  virtual bool isPhotoOriented(size_t imageId) const = 0;
  virtual CameraPose photoOrientation(size_t imageId) const = 0;
  virtual void addPhotoOrientation(size_t imageId,
                                   const CameraPose &orientation) = 0;
  virtual QString database() const = 0;
  virtual QString projectPath() const = 0;
  virtual bool gpsPositions() const = 0;
  virtual bool rtkOrientations() const = 0;
  virtual QString reconstructionPath() const = 0;
  virtual void setReconstructionPath(const QString &reconstructionPath) = 0;
  //virtual std::map<QString, std::array<double, 3>> cameraPositions() const = 0;
  virtual void clearProject() = 0;

  virtual const std::map<int, Camera> &cameras() const = 0;
  virtual bool updateCamera(int id, const Camera &camera) = 0;
  
  virtual const std::unordered_map<size_t, Image> &images() const = 0;
};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_MODEL_INTERFACE_H
