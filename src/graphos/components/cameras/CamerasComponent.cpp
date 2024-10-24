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

#include "CamerasComponent.h"

#include "graphos/components/cameras/impl/CamerasModel.h"
#include "graphos/components/cameras/impl/CamerasView.h"
#include "graphos/components/cameras/impl/CamerasPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

CamerasComponent::CamerasComponent(Application *application)
  : ComponentBase(application)
{
    init();
}

CamerasComponent::~CamerasComponent()
{
}

void CamerasComponent::init()
{
    setName("Cameras");
    setMenu("tools");
    setToolbar("tools");
    setIcon(QIcon::fromTheme("cameras"));
}

void CamerasComponent::onUpdateCameras() const
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
}

void CamerasComponent::createModel()
{
    setModel(new CamerasModelImp(app()->project()));
}

void CamerasComponent::createView()
{
    setView(new CamerasViewImp());
}

void CamerasComponent::createPresenter()
{
    setPresenter(new CamerasPresenterImp(dynamic_cast<CamerasView *>(view()),
                                         dynamic_cast<CamerasModel *>(model())));
    connect(dynamic_cast<CamerasPresenter *>(presenter()), &CamerasPresenter::updateCameras,
            this, &CamerasComponent::onUpdateCameras);
}

void CamerasComponent::createCommand()
{
}

void CamerasComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool images_loaded = app_status->isEnabled(AppStatus::Flag::images_added);
    action()->setEnabled(project_exists && images_loaded);
}

} // namespace graphos
