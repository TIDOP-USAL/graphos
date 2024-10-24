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


#include "OrthophotoComponent.h"

#include "graphos/components/orthophoto/impl/OrthophotoModel.h"
#include "graphos/components/orthophoto/impl/OrthophotoView.h"
#include "graphos/components/orthophoto/impl/OrthophotoPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

OrthophotoComponent::OrthophotoComponent(Application *application)
  : TaskComponent(application)
{
    ComponentBase::setName(tr("Orthophoto"));
    ComponentBase::setMenu("tools");
}

OrthophotoComponent::~OrthophotoComponent() = default;

void OrthophotoComponent::createModel()
{
    setModel(new OrthophotoModelImp(app()->project()));
}

void OrthophotoComponent::createView()
{
    setView(new OrthophotoViewImp());
}

void OrthophotoComponent::createPresenter()
{
    setPresenter(new OrthophotoPresenterImp(dynamic_cast<OrthophotoView *>(view()),
                                            dynamic_cast<OrthophotoModel *>(model())));
}

void OrthophotoComponent::createCommand()
{}

void OrthophotoComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool dtm = app_status->isEnabled(AppStatus::Flag::dtm);
    bool dsm = app_status->isEnabled(AppStatus::Flag::dsm);
    bool process_run = app_status->isEnabled(AppStatus::Flag::processing);
    action()->setEnabled(project_exists && (dtm || dsm) && !process_run);
}

void OrthophotoComponent::onRunning()
{
    TaskComponent::onRunning();
}

void OrthophotoComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::ortho, true);
}

void OrthophotoComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();

    app_status->activeFlag(AppStatus::Flag::ortho, false);
}


} // namespace graphos
