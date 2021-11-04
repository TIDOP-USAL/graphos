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

#include "ExportPointCloudComponent.h"

#include "graphos/components/export/densemodel/impl/ExportPointCloudModel.h"
#include "graphos/components/export/densemodel/impl/ExportPointCloudView.h"
#include "graphos/components/export/densemodel/impl/ExportPointCloudPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


ExportPointCloudComponent::ExportPointCloudComponent(Project *project,
                                                     Application *application)
  : ComponentBase(application),
    mProject(project)
{
  this->setName("Export Point Cloud");
  this->setMenu("file_export");
}

ExportPointCloudComponent::~ExportPointCloudComponent()
{
}

void ExportPointCloudComponent::createModel()
{
  setModel(new ExportPointCloudModelImp(mProject));
}

void ExportPointCloudComponent::createView()
{
  setView(new ExportPointCloudViewImp());
}

void ExportPointCloudComponent::createPresenter()
{
  setPresenter(new ExportPointCloudPresenterImp(dynamic_cast<ExportPointCloudView *>(view()), 
                                                dynamic_cast<ExportPointCloudModel *>(model())));
}

void ExportPointCloudComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bImagesLoaded = app_status->isActive(AppStatus::Flag::oriented) || 
                       app_status->isActive(AppStatus::Flag::absolute_oriented);
  action()->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
}

} // namespace graphos
