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


OpenProjectComponent::OpenProjectComponent(Application *application)
  : ComponentBase(application)
{
  init();
}

OpenProjectComponent::~OpenProjectComponent()
{
}

void OpenProjectComponent::init()
{
  setName("Open Project");
  setMenu("file");
  setToolbar("file");

  action()->setIcon(QIcon::fromTheme("open-project"));
#ifndef QT_NO_SHORTCUT
  action()->setShortcut(tr("Ctrl+O"));
#endif // QT_NO_SHORTCUT
}

void OpenProjectComponent::createModel()
{
  setModel(new OpenProjectModelImp(app()->project()));
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

  connect(dynamic_cast<OpenProjectPresenter *>(presenter()),
          &OpenProjectPresenter::project_loaded, 
          this,
          &OpenProjectComponent::project_loaded);
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
