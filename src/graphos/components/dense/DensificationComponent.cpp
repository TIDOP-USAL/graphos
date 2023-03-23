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

#include "DensificationComponent.h"

#include "graphos/components/dense/impl/DensificationModel.h"
#include "graphos/components/dense/impl/DensificationView.h"
#include "graphos/components/dense/impl/DensificationPresenter.h"
#include "graphos/components/dense/impl/DensificationCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

DensificationComponent::DensificationComponent(Application *application)
  : TaskComponent(application)
{
  init();
}

DensificationComponent::~DensificationComponent()
{
}

void DensificationComponent::init()
{
  this->setName("Densification");
  this->setMenu("workflow");
  this->setToolbar("workflow");

  createCommand();

  action()->setIcon(QIcon::fromTheme("dense"));
}

void DensificationComponent::createModel()
{
  setModel(new DensificationModelImp(app()->project()));
}

void DensificationComponent::createView()
{
  setView(new DensificationViewImp());
}

void DensificationComponent::createPresenter()
{
  setPresenter(new DensificationPresenterImp(dynamic_cast<DensificationView *>(view()),
                                             dynamic_cast<DensificationModel *>(model())));
}

void DensificationComponent::createCommand()
{
  setCommand(std::make_shared<DensificationCommand>());
}

void DensificationComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool dense_model_active = app_status->isEnabled(AppStatus::Flag::project_exists) && 
                            !app_status->isEnabled(AppStatus::Flag::processing) &&
                            app_status->isEnabled(AppStatus::Flag::oriented);

  action()->setEnabled(dense_model_active);
}

void DensificationComponent::onRunning()
{
  TaskComponent::onRunning();
}

void DensificationComponent::onFinished()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  TaskComponent::onFinished();
  app_status->activeFlag(AppStatus::Flag::project_modified, true);
  app_status->activeFlag(AppStatus::Flag::dense_model, true);
}

void DensificationComponent::onFailed()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  TaskComponent::onFailed();
  app_status->activeFlag(AppStatus::Flag::dense_model, false);
}

} // namespace graphos
