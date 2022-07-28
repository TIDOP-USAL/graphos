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

#include "OrientationComponent.h"

#include "graphos/components/orientation/impl/OrientationModel.h"
#include "graphos/components/orientation/impl/OrientationView.h"
#include "graphos/components/orientation/impl/OrientationPresenter.h"
#include "graphos/components/orientation/impl/OrientationCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

OrientationComponent::OrientationComponent(Application *application)
  : ProcessComponent(application)
{
  init();
}

OrientationComponent::~OrientationComponent()
{
}

void OrientationComponent::init()
{
  this->setName("Orientation");
  this->setMenu("workflow");
  this->setToolbar("workflow");

  createCommand();

  action()->setIcon(QIcon::fromTheme("orientation"));
}

void OrientationComponent::createModel()
{
  setModel(new OrientationModelImp(app()->project()));
}

void OrientationComponent::createView()
{
  setView(new OrientationViewImp());
}

void OrientationComponent::createPresenter()
{
  setPresenter(new OrientationPresenterImp(dynamic_cast<OrientationView *>(view()), 
                                           dynamic_cast<OrientationModel *>(model())));

  connect(dynamic_cast<OrientationPresenter *>(presenter()), 
          &OrientationPresenter::orientation_deleted, 
          this, 
          &OrientationComponent::orientation_deleted);
}

void OrientationComponent::createCommand()
{
  setCommand(std::make_shared<OrientationCommand>());
}

void OrientationComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool oriented_active = app_status->isActive(AppStatus::Flag::project_exists) &&
                         app_status->isActive(AppStatus::Flag::feature_matching) &&
                         !app_status->isActive(AppStatus::Flag::processing);
  
  action()->setEnabled(oriented_active);
}

void OrientationComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void OrientationComponent::onFinished()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFinished();

  app_status->activeFlag(AppStatus::Flag::project_modified, true);
  app_status->activeFlag(AppStatus::Flag::oriented, true);
}

void OrientationComponent::onFailed()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFailed();

  app_status->activeFlag(AppStatus::Flag::oriented, false);
}

} // namespace graphos
