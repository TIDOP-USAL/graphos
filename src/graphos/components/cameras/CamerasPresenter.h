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

#ifndef GRAPHOS_CAMERAS_PRESENTER_INTERFACE_H
#define GRAPHOS_CAMERAS_PRESENTER_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class CamerasPresenter
  : public Presenter
{

  Q_OBJECT

public:

  CamerasPresenter(){}
  virtual ~CamerasPresenter() override = default;

public slots:

  virtual void activeCamera(int id) = 0;
  virtual void fixCalibration(bool fix) = 0;

protected slots:

  //virtual void onTypeChange(const QString &type) = 0;
  //virtual void updateCurrentCameraMake(const QString &make) = 0;
  //virtual void updateCurrentCameraModel(const QString &model) = 0;
  //virtual void updateCurrentCameraWidth(int width) = 0;
  //virtual void updateCurrentCameraHeight(int height) = 0;
  //virtual void updateCurrentCameraSensorSize(const QString &sensorSize) = 0;
  //virtual void updateCurrentCameraFocal(const QString &focal) = 0;
  //virtual void updateCurrentCameraType(const QString &type) = 0;

  virtual void save() = 0;
  virtual void discart() = 0;

signals:

  void updateCameras();

};

} // namespace graphos

#endif // GRAPHOS_CAMERAS_PRESENTER_INTERFACE_H
