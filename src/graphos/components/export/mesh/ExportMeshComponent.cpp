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

#include "ExportMeshComponent.h"

#include "graphos/components/export/mesh/impl/ExportMeshModel.h"
#include "graphos/components/export/mesh/impl/ExportMeshView.h"
#include "graphos/components/export/mesh/impl/ExportMeshPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


ExportMeshComponent::ExportMeshComponent(Application *application)
    : ComponentBase(application)
{
    init();
}

void ExportMeshComponent::init()
{
    setName("Export Mesh");
    setMenu("file_export");
    setIcon(QIcon::fromTheme("export"));

    createCommand();
}

void ExportMeshComponent::createModel()
{
    setModel(new ExportMeshModelImp(app()->project()));
}

void ExportMeshComponent::createView()
{
    setView(new ExportMeshViewImp());
}

void ExportMeshComponent::createPresenter()
{
    setPresenter(new ExportMeshPresenterImp(dynamic_cast<ExportMeshView *>(view()),
                                            dynamic_cast<ExportMeshModel *>(model()),
                                            app()->status()));
}

void ExportMeshComponent::createCommand()
{
}

void ExportMeshComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool mesh = app_status->isEnabled(AppStatus::Flag::mesh);
    action()->setEnabled(project_exists && mesh);
}

} // namespace graphos
