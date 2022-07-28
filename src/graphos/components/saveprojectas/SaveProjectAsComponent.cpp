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

#include "SaveProjectAsComponent.h"

#include "graphos/components/saveprojectas/impl/SaveProjectAsModel.h"
#include "graphos/components/saveprojectas/impl/SaveProjectAsView.h"
#include "graphos/components/saveprojectas/impl/SaveProjectAsPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


SaveProjectAsComponent::SaveProjectAsComponent(Application *application)
  : ComponentBase(application)
{
  init();
}

SaveProjectAsComponent::~SaveProjectAsComponent()
{
}

void SaveProjectAsComponent::init()
{
  this->setName("Save Project As...");
  this->setMenu("file");
  this->setToolbar("file");

  action()->setIcon(QIcon::fromTheme("save-as"));
#ifndef QT_NO_SHORTCUT
  action()->setShortcut(tr("Ctrl+Shift+S"));
#endif // QT_NO_SHORTCUT
}

void SaveProjectAsComponent::createModel()
{
  setModel(new SaveProjectAsModelImp(app()->project()));
}

void SaveProjectAsComponent::createView()
{
  setView(new SaveProjectAsViewImp());
}

void SaveProjectAsComponent::createPresenter()
{
  setPresenter(new SaveProjectAsPresenterImp(dynamic_cast<SaveProjectAsView *>(view()),
                                             dynamic_cast<SaveProjectAsModel *>(model()),
                                             app()->status()));
}

void SaveProjectAsComponent::createCommand()
{
}

void SaveProjectAsComponent::update()
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
