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

#ifndef GRAPHOS_ORIENTATION_MODEL_H
#define GRAPHOS_ORIENTATION_MODEL_H

#include "graphos/ui/orientation/OrientationModel.h"

namespace graphos
{

class Project;

class OrientationModelImp
  : public OrientationModel
{

public:

  OrientationModelImp(Project *project,
                      QObject *parent = nullptr);
  ~OrientationModelImp() override = default;

// OrientationModel interface

public:

  bool calibratedCamera() const override;
  bool refinePrincipalPoint() const override;
  void setRefinePrincipalPoint(bool refine) override;
  void setSparseModel(const QString &sparseModel) override;
  void setOffset(const QString &offset) override;
  bool isPhotoOriented(const QString &imgName) const override;
  CameraPose photoOrientation(const QString &imgName) const override;
  void addPhotoOrientation(const QString &imgName, const CameraPose &orientation) override;
  QString database() const override;
  QString projectPath() const override;
  bool gpsPositions() const override;
  bool rtkOrientations() const override;
  QString reconstructionPath() const override;
  void setReconstructionPath(const QString &reconstructionPath) override;
  std::map<QString, std::array<double, 3>> cameraPositions() const override;
  void clearProject() override;

  std::map<int, tl::Camera> cameras() const override;
  bool updateCamera(int id, const tl::Camera &camera) override;
  camera_iterator cameraBegin() override;
  camera_const_iterator cameraBegin() const override;
  camera_iterator cameraEnd() override;
  camera_const_iterator cameraEnd() const override;

  std::vector<Image> images() const override;
  image_iterator imageBegin() override;
  image_const_iterator imageBegin() const override;
  image_iterator imageEnd() override;
  image_const_iterator imageEnd() const override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_MODEL_H
