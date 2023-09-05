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

#include "ExportCamerasComponent.h"

#include "graphos/components/export/cameras/impl/ExportCamerasModel.h"
#include "graphos/components/export/cameras/impl/ExportCamerasView.h"
#include "graphos/components/export/cameras/impl/ExportCamerasPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


ExportCamerasComponent::ExportCamerasComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

ExportCamerasComponent::~ExportCamerasComponent()
{
}

void ExportCamerasComponent::init()
{
    this->setName("Export Cameras");
    this->setMenu("file_export");

    createCommand();
}

void ExportCamerasComponent::createModel()
{
    setModel(new ExportCamerasModelImp(app()->project()));
}

void ExportCamerasComponent::createView()
{
    setView(new ExportCamerasViewImp());
}

void ExportCamerasComponent::createPresenter()
{
    setPresenter(new ExportCamerasPresenterImp(dynamic_cast<ExportCamerasView *>(view()),
                                               dynamic_cast<ExportCamerasModel *>(model())));
}

void ExportCamerasComponent::createCommand()
{
}

void ExportCamerasComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool bProjectExists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool bProcessing = app_status->isEnabled(AppStatus::Flag::processing);
    bool bImagesLoaded = app_status->isEnabled(AppStatus::Flag::oriented) ||
                         app_status->isEnabled(AppStatus::Flag::absolute_oriented);
    action()->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
}

void ExportCamerasComponent::onRunning()
{
    TaskComponent::onRunning();
}

void ExportCamerasComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::feature_matching, true);
}

void ExportCamerasComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();

    app_status->activeFlag(AppStatus::Flag::feature_matching, false);
}

} // namespace graphos
