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

#ifndef GRAPHOS_CAMERAS_PRESENTER_H
#define GRAPHOS_CAMERAS_PRESENTER_H

#include "graphos/components/cameras/CamerasPresenter.h"

namespace graphos
{

class Camera;
class CamerasView;
class CamerasModel;

class CamerasPresenterImp
  : public CamerasPresenter
{

  Q_OBJECT

public:

  CamerasPresenterImp(CamerasView *view,
                      CamerasModel *model);
  ~CamerasPresenterImp() override = default;

private:

  void loadCameras();
  void clear();

private slots:

  void calibrationImport(const QString &file,
                         const QString &format);

// Presenter interface

public slots:

  void open() override;

private:

  void init() override;
  void initSignalAndSlots() override;

// CamerasPresenter interface

public slots:

  void activeCamera(int id) override;

protected slots:

  void save() override;
  void discart() override;

private:

  CamerasView *mView;
  CamerasModel *mModel;

};

} // namespace graphos

#endif // GRAPHOS_CAMERAS_PRESENTER_H
