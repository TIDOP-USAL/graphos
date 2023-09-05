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


#include "LoadFromVideoComponent.h"

#include "graphos/components/loadfromvideo/impl/LoadFromVideoModel.h"
#include "graphos/components/loadfromvideo/impl/LoadFromVideoView.h"
#include "graphos/components/loadfromvideo/impl/LoadFromVideoPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

LoadFromVideoComponent::LoadFromVideoComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

LoadFromVideoComponent::~LoadFromVideoComponent()
{
}

void LoadFromVideoComponent::init()
{
    this->setName(tr("Load from Video"));
    this->setMenu("workflow");
    this->setToolbar("workflow");

    action()->setIcon(QIcon::fromTheme("video-folder"));
}

void LoadFromVideoComponent::createModel()
{
    setModel(new LoadFromVideoModelImp(app()->project()));
}

void LoadFromVideoComponent::createView()
{
    setView(new LoadFromVideoViewImp());
}

void LoadFromVideoComponent::createPresenter()
{
    setPresenter(new LoadFromVideoPresenterImp(dynamic_cast<LoadFromVideoView *>(view()),
                 dynamic_cast<LoadFromVideoModel *>(model())));

    connect(dynamic_cast<LoadFromVideoPresenter *>(presenter()), &LoadFromVideoPresenter::frame_loaded,
            this, &LoadFromVideoComponent::frame_loaded);
}

void LoadFromVideoComponent::createCommand()
{
}

void LoadFromVideoComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool loading_images = app_status->isEnabled(AppStatus::Flag::loading_images);
    action()->setEnabled(project_exists && !loading_images && !processing);
}

void LoadFromVideoComponent::onRunning()
{
    TaskComponent::onRunning();
}

void LoadFromVideoComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
}

void LoadFromVideoComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
}


} // namespace graphos
