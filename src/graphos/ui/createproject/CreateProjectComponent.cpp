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

#include "CreateProjectComponent.h"

#include "graphos/ui/createproject/impl/CreateProjectModel.h"
#include "graphos/ui/createproject/impl/CreateProjectView.h"
#include "graphos/ui/createproject/impl/CreateProjectPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


CreateProjectComponent::CreateProjectComponent(Project *project, 
                                               Application *application)
  : ComponentBase(application),
    mProject(project)
{
  this->setName("New Project");
  this->setMenu("file");
  this->setToolbar("file");
  QIcon iconNewProject;
  iconNewProject.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-empty-document.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(iconNewProject);
#ifndef QT_NO_SHORTCUT
  action()->setShortcut(tr("Ctrl+N"));
#endif // QT_NO_SHORTCUT
}

CreateProjectComponent::~CreateProjectComponent()
{
}

void CreateProjectComponent::createModel()
{
  setModel(new CreateProjectModelImp(mProject));
}

void CreateProjectComponent::createView()
{
  setView(new CreateProjectViewImp());
}

void CreateProjectComponent::createPresenter()
{
  setPresenter(new CreateProjectPresenterImp(dynamic_cast<CreateProjectView *>(view()),
                                             dynamic_cast<CreateProjectModel *>(model()),
                                             app()->status()));
  connect(dynamic_cast<CreateProjectPresenter *>(presenter()), &CreateProjectPresenter::projectCreate,
          this, &CreateProjectComponent::onProjectCreated);
}

void CreateProjectComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  action()->setEnabled(!bProcessing);
}

void CreateProjectComponent::onProjectCreated()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  app_status->activeFlag(AppStatus::Flag::project_exists, true);
  
  emit projectCreated();
}

} // namespace graphos
