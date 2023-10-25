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


#include "ScaleComponent.h"

#include "graphos/components/scale/impl/ScaleModel.h"
#include "graphos/components/scale/impl/ScaleView.h"
#include "graphos/components/scale/impl/ScalePresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

ScaleComponent::ScaleComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

ScaleComponent::~ScaleComponent()
{
}

void ScaleComponent::init()
{
    setName(tr("Scale"));
    setMenu("tools");

    createCommand();
}

void ScaleComponent::createModel()
{
    setModel(new ScaleModelImp(app()->project()));
}

void ScaleComponent::createView()
{
    setView(new ScaleViewImp());
}

void ScaleComponent::createPresenter()
{
    setPresenter(new ScalePresenterImp(dynamic_cast<ScaleView *>(view()),
                 dynamic_cast<ScaleModel *>(model())));

    connect(dynamic_cast<ScalePresenter *>(presenter()), &ScalePresenter::open_3d_model,
            [&]() {
                emit open_3d_model();
                dynamic_cast<ScalePresenterImp *>(presenter())->measure(true);
            });
}

void ScaleComponent::createCommand()
{
}

void ScaleComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    action()->setEnabled(app_status->isEnabled(AppStatus::Flag::project_exists) &&
                         !app_status->isEnabled(AppStatus::Flag::processing) &&
                         app_status->isEnabled(AppStatus::Flag::dense_model));
}

void ScaleComponent::onRunning()
{
    TaskComponent::onRunning();
}

void ScaleComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
}

void ScaleComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
}


} // namespace graphos
