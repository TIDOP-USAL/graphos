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

#include "PropertiesComponent.h"

#include "graphos/components/properties/impl/PropertiesModel.h"
#include "graphos/components/properties/impl/PropertiesView.h"
#include "graphos/components/properties/impl/PropertiesPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


PropertiesComponent::PropertiesComponent(Application *application)
  : ComponentBase(application)
{
  init();
}

PropertiesComponent::~PropertiesComponent()
{
}

void PropertiesComponent::init()
{
  this->setName("Properties");

  //createCommand();
}

void PropertiesComponent::createModel()
{
  setModel(new PropertiesModelImp(app()->project()));
}

void PropertiesComponent::createView()
{
  setView(new PropertiesViewImp());
  setViewType(ViewType::dock_widget);
}

void PropertiesComponent::createPresenter()
{
  setPresenter(new PropertiesPresenterImp(dynamic_cast<PropertiesView *>(view()),
                                          dynamic_cast<PropertiesModel *>(model()),
                                          app()->status()));

  //connect(dynamic_cast<PropertiesPresenter *>(presenter()), &PropertiesPresenter::project_created,
  //        this, &PropertiesComponent::onProjectCreated);
}

void PropertiesComponent::createCommand()
{
}

void PropertiesComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  //bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  //action()->setEnabled(!bProcessing);
}



} // namespace graphos
