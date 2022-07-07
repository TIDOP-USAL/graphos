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

#include "CloseProjectComponent.h"

#include "graphos/components/closeproject/impl/CloseProjectModel.h"
#include "graphos/components/closeproject/impl/CloseProjectView.h"
#include "graphos/components/closeproject/impl/CloseProjectPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


CloseProjectComponent::CloseProjectComponent(Application *application)
  : ComponentBase(application)
{
  init();
}

CloseProjectComponent::~CloseProjectComponent()
{
}

void CloseProjectComponent::init()
{
  this->setName("Close Project");
  this->setMenu("file");
  //this->setToolbar("file");

  action()->setIcon(QIcon(":/ico/24/img/material/24/icons8-close-button.png"));
#ifndef QT_NO_SHORTCUT
  action()->setShortcut(tr("Ctrl+O"));
#endif // QT_NO_SHORTCUT
}

void CloseProjectComponent::createModel()
{
  setModel(new CloseProjectModelImp(app()->project()));
}

void CloseProjectComponent::createView()
{
  setView(new CloseProjectViewImp());
}

void CloseProjectComponent::createPresenter()
{
  setPresenter(new CloseProjectPresenterImp(dynamic_cast<CloseProjectView *>(view()),
                                            dynamic_cast<CloseProjectModel *>(model()),
                                            app()->status()));

  connect(dynamic_cast<CloseProjectPresenter *>(presenter()),
          &CloseProjectPresenter::projectClosed,
          this,
          &CloseProjectComponent::projectClosed);
}

void CloseProjectComponent::createCommand()
{
}

void CloseProjectComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool processing = app_status->isActive(AppStatus::Flag::processing);
  bool loading_images = app_status->isActive(AppStatus::Flag::loading_images);
  bool project_exists = app_status->isActive(AppStatus::Flag::project_exists);
  action()->setEnabled(!loading_images && !processing && project_exists);
}

} // namespace graphos
