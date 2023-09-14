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


#include "MeshComponent.h"

#include "graphos/components/mesh/impl/MeshModel.h"
#include "graphos/components/mesh/impl/MeshView.h"
#include "graphos/components/mesh/impl/MeshPresenter.h"
#include "graphos/components/mesh/impl/MeshCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

MeshComponent::MeshComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::init()
{
    setName(tr("Mesh"));
    setMenu("workflow");
    setToolbar("workflow");
    setIcon(QIcon::fromTheme("mesh"));

    createCommand();
}

void MeshComponent::createModel()
{
    setModel(new MeshModelImp(app()->project()));
}

void MeshComponent::createView()
{
    setView(new MeshViewImp());
}

void MeshComponent::createPresenter()
{
    setPresenter(new MeshPresenterImp(dynamic_cast<MeshView *>(view()),
                 dynamic_cast<MeshModel *>(model())));
}

void MeshComponent::createCommand()
{
    setCommand(std::make_shared<MeshCommand>());
}

void MeshComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool bProjectExists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool process_run = app_status->isEnabled(AppStatus::Flag::processing);
    bool bDenseModel = app_status->isEnabled(AppStatus::Flag::dense_model);
    action()->setEnabled(bProjectExists && !process_run && bDenseModel);
}

void MeshComponent::onRunning()
{
    TaskComponent::onRunning();
}

void MeshComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();
    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::mesh, true);
}

void MeshComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
    app_status->activeFlag(AppStatus::Flag::mesh, false);
}


} // namespace graphos
