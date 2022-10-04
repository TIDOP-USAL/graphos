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

#include "DTMComponent.h"

#include "graphos/components/dtm/impl/DTMModel.h"
#include "graphos/components/dtm/impl/DTMView.h"
#include "graphos/components/dtm/impl/DTMPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

DTMComponent::DTMComponent(Application *application)
  : TaskComponent(application)
{
  init();
}

DTMComponent::~DTMComponent()
{
}

void DTMComponent::init()
{
  this->setName("DTM/DSM");
  this->setMenu("tools");

  createCommand();
}

void DTMComponent::createModel()
{
  setModel(new DtmModelImp(app()->project()));
}

void DTMComponent::createView()
{
  setView(new DtmViewImp());
}

void DTMComponent::createPresenter()
{
  setPresenter(new DtmPresenterImp(dynamic_cast<DtmView *>(view()), 
                                   dynamic_cast<DtmModel *>(model())));
}

void DTMComponent::createCommand()
{
}

void DTMComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bAbsoluteOriented = app_status->isActive(AppStatus::Flag::absolute_oriented);
  bool bDenseModel = app_status->isActive(AppStatus::Flag::dense_model);
  action()->setEnabled(bProjectExists && bAbsoluteOriented && bDenseModel && !bProcessing);
}

void DTMComponent::onRunning()
{
  TaskComponent::onRunning();
}

void DTMComponent::onFinished()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  TaskComponent::onFinished();

  app_status->activeFlag(AppStatus::Flag::project_modified, true);
  app_status->activeFlag(AppStatus::Flag::dtm, true);
}

void DTMComponent::onFailed()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  TaskComponent::onFailed();
  app_status->activeFlag(AppStatus::Flag::dtm, false);
}

} // namespace graphos
