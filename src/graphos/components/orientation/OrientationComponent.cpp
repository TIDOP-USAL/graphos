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

#include "OrientationComponent.h"

#include "graphos/components/orientation/impl/OrientationModel.h"
#include "graphos/components/orientation/impl/OrientationView.h"
#include "graphos/components/orientation/impl/OrientationPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

OrientationComponent::OrientationComponent(Project *project,
                                           Application *application)
  : ProcessComponent(application),
    mProject(project)
{
  this->setName("Orientation");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  action()->setIcon(QIcon(":/ico/24/img/material/24/icons8-coordinate-system.png"));
}

OrientationComponent::~OrientationComponent()
{
}

void OrientationComponent::createModel()
{
  setModel(new OrientationModelImp(mProject));
}

void OrientationComponent::createView()
{
  setView(new OrientationViewImp());
}

void OrientationComponent::createPresenter()
{
  setPresenter(new OrientationPresenterImp(dynamic_cast<OrientationView *>(view()), 
                                           dynamic_cast<OrientationModel *>(model())));

  connect(dynamic_cast<OrientationPresenter *>(presenter()), &OrientationPresenter::orientationFinished, 
          this, &OrientationComponent::orientationFinished);
  connect(dynamic_cast<OrientationPresenter *>(presenter()), &OrientationPresenter::orientationDeleted, 
          this, &OrientationComponent::orientationDeleted);
}

void OrientationComponent::createCommand()
{
}

void OrientationComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bFeatureMatching = app_status->isActive(AppStatus::Flag::feature_matching);
  action()->setEnabled(bProjectExists && bFeatureMatching && !bProcessing);
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
