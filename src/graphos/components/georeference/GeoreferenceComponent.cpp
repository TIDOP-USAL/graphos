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

#include "GeoreferenceComponent.h"

#include "graphos/components/georeference/impl/GeoreferenceModel.h"
#include "graphos/components/georeference/impl/GeoreferenceView.h"
#include "graphos/components/georeference/impl/GeoreferencePresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

GeoreferenceComponent::GeoreferenceComponent(Project *project,
                                             Application *application)
  : ProcessComponent(application),
    mProject(project)
{
  this->setName("Georeference");
  this->setMenu("tools");
  this->setToolbar("tools");
  QIcon iconGeoreference;
  iconGeoreference.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-coordinate-system.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(iconGeoreference);
}

GeoreferenceComponent::~GeoreferenceComponent()
{
}

void GeoreferenceComponent::createModel()
{
  setModel(new GeoreferenceModelImp(mProject));
}

void GeoreferenceComponent::createView()
{
  Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  setView(new GeoreferenceViewImp(nullptr, f));
}

void GeoreferenceComponent::createPresenter()
{
  setPresenter(new GeoreferencePresenterImp(dynamic_cast<GeoreferenceView *>(view()), 
                                            dynamic_cast<GeoreferenceModel *>(model())));
  connect(dynamic_cast<GeoreferencePresenterImp *>(presenter()), &GeoreferencePresenter::georeferenceFinished,
          this, &GeoreferenceComponent::georeferenceFinished);
}

void GeoreferenceComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bOriented = app_status->isActive(AppStatus::Flag::oriented);
  action()->setEnabled(bProjectExists && bOriented && !bProcessing);
}

void GeoreferenceComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void GeoreferenceComponent::onFinished()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFinished();
  app_status->activeFlag(AppStatus::Flag::absolute_oriented, true);
}

void GeoreferenceComponent::onFailed()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFailed();
  app_status->activeFlag(AppStatus::Flag::absolute_oriented, false);
}

} // namespace graphos
