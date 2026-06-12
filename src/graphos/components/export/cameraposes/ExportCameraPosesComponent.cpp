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

#include "ExportCameraPosesComponent.h"

#ifdef GRAPHOS_GUI
#include "graphos/components/export/cameraposes/impl/ExportCameraPosesModel.h"
#include "graphos/components/export/cameraposes/impl/ExportCameraPosesView.h"
#include "graphos/components/export/cameraposes/impl/ExportCameraPosesPresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/export/cameraposes/impl/ExportCameraPosesCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


ExportCameraPosesComponent::ExportCameraPosesComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

ExportCameraPosesComponent::~ExportCameraPosesComponent()
{
}

void ExportCameraPosesComponent::init()
{
    setName(QApplication::translate("ExportCameraPosesComponent", "Export Cameras"));
    setMenu("file_export");

    createCommand();
}

void ExportCameraPosesComponent::createModel()
{
#ifdef GRAPHOS_GUI
    setModel(new ExportCameraPosesModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void ExportCameraPosesComponent::createView()
{
#ifdef GRAPHOS_GUI
    setView(new ExportCameraPosesViewImp());
#endif // GRAPHOS_GUI
}

void ExportCameraPosesComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new ExportCameraPosesPresenterImp(dynamic_cast<ExportCameraPosesView *>(view()),
                                                   dynamic_cast<ExportCameraPosesModel *>(model())));
#endif // GRAPHOS_GUI
}

void ExportCameraPosesComponent::createCommand()
{
    setCommand(std::make_shared<ExportCameraPosesCommand>());
}

void ExportCameraPosesComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool images_loaded = app_status->isEnabled(AppStatus::Flag::oriented) ||
                         app_status->isEnabled(AppStatus::Flag::absolute_oriented);
    action()->setEnabled(project_exists && images_loaded && !processing);
}

void ExportCameraPosesComponent::onRunning()
{
    TaskComponent::onRunning();
}

void ExportCameraPosesComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::feature_matching, true);
}

void ExportCameraPosesComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();

    app_status->activeFlag(AppStatus::Flag::feature_matching, false);
}

} // namespace graphos
