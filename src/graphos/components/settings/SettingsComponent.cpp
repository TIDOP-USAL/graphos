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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "SettingsComponent.h"

#include "graphos/components/settings/impl/SettingsModel.h"
#include "graphos/components/settings/impl/SettingsView.h"
#include "graphos/components/settings/impl/SettingsPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


SettingsComponent::SettingsComponent(Application *application)
  : ComponentBase(application)
{
  this->setName("Settings");
  this->setMenu("tools");
  this->setToolbar("tools");
  action()->setIcon(QIcon(":/ico/24/img/material/24/icons8-settings.png"));
}

SettingsComponent::~SettingsComponent()
{
}

void SettingsComponent::createModel()
{
  setModel(new SettingsModelImp(app()->settings()));
}

void SettingsComponent::createView()
{
  setView(new SettingsViewImp());
}

void SettingsComponent::createPresenter()
{
  setPresenter(new SettingsPresenterImp(dynamic_cast<SettingsView *>(view()),
                                        dynamic_cast<SettingsModel *>(model())));
}

void SettingsComponent::createCommand()
{
}

void SettingsComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  action()->setEnabled(true);
}

} // namespace graphos
