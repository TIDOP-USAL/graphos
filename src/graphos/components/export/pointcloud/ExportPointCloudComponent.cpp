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

#include "ExportPointCloudComponent.h"

#include "graphos/components/export/pointcloud/impl/ExportPointCloudModel.h"
#include "graphos/components/export/pointcloud/impl/ExportPointCloudView.h"
#include "graphos/components/export/pointcloud/impl/ExportPointCloudPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


ExportPointCloudComponent::ExportPointCloudComponent(Application *application)
    : ComponentBase(application)
{
    init();
}

ExportPointCloudComponent::~ExportPointCloudComponent()
{
}

void ExportPointCloudComponent::init()
{
    setName("Export Point Cloud");
    setMenu("file_export");
    setIcon(QIcon::fromTheme("export"));

    createCommand();
}

void ExportPointCloudComponent::createModel()
{
    setModel(new ExportPointCloudModelImp(app()->project()));
}

void ExportPointCloudComponent::createView()
{
    setView(new ExportPointCloudViewImp());
}

void ExportPointCloudComponent::createPresenter()
{
    setPresenter(new ExportPointCloudPresenterImp(dynamic_cast<ExportPointCloudView *>(view()),
                 dynamic_cast<ExportPointCloudModel *>(model()),
                 app()->status()));
}

void ExportPointCloudComponent::createCommand()
{
}

void ExportPointCloudComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool dense_model = app_status->isEnabled(AppStatus::Flag::dense_model);
    action()->setEnabled(project_exists && dense_model);
}

} // namespace graphos
