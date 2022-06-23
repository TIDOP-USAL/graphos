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

#include "TabComponent.h"

#include "graphos/components/Tab/impl/TabModel.h"
#include "graphos/components/Tab/impl/TabView.h"
#include "graphos/components/Tab/impl/TabPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{



TabComponent::TabComponent(Application *application)
  : ComponentBase(application)
{
  init();
}

TabComponent::~TabComponent()
{
}

void TabComponent::init()
{

}

void TabComponent::createModel()
{
  setModel(new TabModelImp(app()->project()));
}

void TabComponent::createView()
{
  setView(new TabViewImp());
}

void TabComponent::createPresenter()
{
  setPresenter(new TabPresenterImp(dynamic_cast<TabView *>(view()),
                                             dynamic_cast<TabModel *>(model()),
                                             app()->status()));

  //connect(dynamic_cast<TabPresenter *>(presenter()), &TabPresenter::project_created,
  //        this, &TabComponent::onProjectCreated);
}

void TabComponent::createCommand()
{
}

void TabComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  //bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  //action()->setEnabled(!bProcessing);
}



} // namespace graphos
