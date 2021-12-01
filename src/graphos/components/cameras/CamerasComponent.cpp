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

#include "CamerasComponent.h"

#include "graphos/components/cameras/impl/CamerasModel.h"
#include "graphos/components/cameras/impl/CamerasView.h"
#include "graphos/components/cameras/impl/CamerasPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

CamerasComponent::CamerasComponent(Project *project,
                                   Application *application)
  : ComponentBase(application),
    mProject(project)
{
  this->setName("Cameras");
  this->setMenu("tools");
  QIcon icon;
  icon.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-camera-outline-2.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(icon);
}

CamerasComponent::~CamerasComponent()
{
}

void CamerasComponent::onUpdateCameras()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  app_status->activeFlag(AppStatus::Flag::project_modified, true);
}

void CamerasComponent::createModel()
{
  setModel(new CamerasModelImp(mProject));
}

void CamerasComponent::createView()
{
  setView(new CamerasViewImp());
}

void CamerasComponent::createPresenter()
{
  setPresenter(new CamerasPresenterImp(dynamic_cast<CamerasView *>(view()), 
                                       dynamic_cast<CamerasModel *>(model())));
  connect(dynamic_cast<CamerasPresenter *>(presenter()), &CamerasPresenter::updateCameras,
          this, &CamerasComponent::onUpdateCameras); 
}

void CamerasComponent::createCommand()
{
}

void CamerasComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bImagesLoaded = app_status->isActive(AppStatus::Flag::images_added);
  action()->setEnabled(bProjectExists && bImagesLoaded);
}

} // namespace graphos
