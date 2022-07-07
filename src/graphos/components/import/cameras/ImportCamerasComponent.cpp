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

#include "ImportCamerasComponent.h"

#include "graphos/components/import/cameras/impl/ImportCamerasModel.h"
#include "graphos/components/import/cameras/impl/ImportCamerasView.h"
#include "graphos/components/import/cameras/impl/ImportCamerasPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

ImportCamerasComponent::ImportCamerasComponent(Application *application)
  : ComponentBase(application)
{
  this->setName("Import Cameras");
  this->setMenu("file_import");
  //this->setToolbar("file");
  //mAction->setIcon(QIcon(":/ico/24/img/material/24/features.png"));
}

ImportCamerasComponent::~ImportCamerasComponent()
{
}

void ImportCamerasComponent::onImportedCameras()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  app_status->activeFlag(AppStatus::Flag::project_modified, true);
}

void ImportCamerasComponent::createModel()
{
  setModel(new ImportCamerasModelImp(app()->project()));
}

void ImportCamerasComponent::createView()
{
  setView(new ImportCamerasViewImp());
}

void ImportCamerasComponent::createPresenter()
{
  setPresenter(new ImportCamerasPresenterImp(dynamic_cast<ImportCamerasView *>(view()), 
                                             dynamic_cast<ImportCamerasModel *>(model())));
  
  connect(dynamic_cast<ImportCamerasPresenter *>(presenter()), &ImportCamerasPresenter::importedCameras,
          this, &ImportCamerasComponent::onImportedCameras);
}

void ImportCamerasComponent::createCommand()
{
}

void ImportCamerasComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bImagesLoaded = app_status->isActive(AppStatus::Flag::images_added);
  action()->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
}

} // namespace graphos
