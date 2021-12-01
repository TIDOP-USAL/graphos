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

#include "OpenProjectComponent.h"

#include "graphos/components/openproject/impl/OpenProjectModel.h"
#include "graphos/components/openproject/impl/OpenProjectView.h"
#include "graphos/components/openproject/impl/OpenProjectPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


OpenProjectComponent::OpenProjectComponent(Project *project,
                                           Application *application)
  : ComponentBase(application),
    mProject(project)
{
  this->setName("Open Project");
  this->setMenu("file");
  this->setToolbar("file");
  QIcon iconOpenProject;
  iconOpenProject.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-open.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(iconOpenProject);
#ifndef QT_NO_SHORTCUT
  action()->setShortcut(tr("Ctrl+O"));
#endif // QT_NO_SHORTCUT
}

OpenProjectComponent::~OpenProjectComponent()
{
}

void OpenProjectComponent::createModel()
{
  setModel(new OpenProjectModelImp(mProject));
}

void OpenProjectComponent::createView()
{
  setView(new OpenProjectViewImp());
}

void OpenProjectComponent::createPresenter()
{
  setPresenter(new OpenProjectPresenterImp(dynamic_cast<OpenProjectView *>(view()),
                                           dynamic_cast<OpenProjectModel *>(model()),
                                           app()->status()));
  connect(dynamic_cast<OpenProjectPresenter *>(presenter()), &OpenProjectPresenter::projectLoaded, 
          this, &OpenProjectComponent::projectLoaded);
}

void OpenProjectComponent::createCommand()
{
}

void OpenProjectComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bLoadingImages = app_status->isActive(AppStatus::Flag::loading_images);
  action()->setEnabled(!bLoadingImages && !bProcessing);
}

} // namespace graphos
