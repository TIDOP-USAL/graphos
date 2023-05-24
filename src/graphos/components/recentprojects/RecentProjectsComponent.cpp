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

#include "RecentProjectsComponent.h"

#include "graphos/components/recentprojects/impl/RecentProjectsModel.h"
#include "graphos/components/recentprojects/impl/RecentProjectsView.h"
#include "graphos/components/recentprojects/impl/RecentProjectsPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


  RecentProjectsComponent::RecentProjectsComponent(Application *application)
  : MultiComponentBase(application)
{
  init();
}

  RecentProjectsComponent::~RecentProjectsComponent()
{
}

void RecentProjectsComponent::init()
{
  this->setName("Recent Projects");
  this->setMenu("file");
  //this->setToolbar("file");

  //action()->setIcon(QIcon(":/ico/24/img/material/24/icons8-open.png"));
//#ifndef QT_NO_SHORTCUT
//  action()->setShortcut(tr("Ctrl+O"));
//#endif // QT_NO_SHORTCUT

  this->createModel();
  this->createView();
  this->createPresenter();
}

void RecentProjectsComponent::createModel()
{
  setModel(new RecentProjectsModelImp());
}

void RecentProjectsComponent::createView()
{
  setView(new RecentProjectsViewImp());
}

void RecentProjectsComponent::createPresenter()
{
  setPresenter(new RecentProjectsPresenterImp(dynamic_cast<RecentProjectsView *>(view()),
               dynamic_cast<RecentProjectsModel *>(model()),
               app()->status()));

  connect(dynamic_cast<RecentProjectsPresenter *>(presenter()), &RecentProjectsPresenter::open_project,
        this, &RecentProjectsComponent::open_project);
}

void RecentProjectsComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProcessing = app_status->isEnabled(AppStatus::Flag::processing);
  bool bLoadingImages = app_status->isEnabled(AppStatus::Flag::loading_images);
  //action()->setEnabled(!bLoadingImages && !bProcessing);
}

} // namespace graphos
