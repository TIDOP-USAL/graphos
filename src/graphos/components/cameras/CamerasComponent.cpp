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

#ifdef GRAPHOS_GUI
#include "graphos/components/cameras/impl/CamerasModel.h"
#include "graphos/components/cameras/impl/CamerasView.h"
#include "graphos/components/cameras/impl/CamerasPresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/cameras/impl/CamerasCommand.h"
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
    setName(QApplication::translate("CamerasComponent", "Cameras"));
    setMenu("tools");
    setToolbar("tools");
    setIcon(QIcon::fromTheme("cameras"));

    createCommand();
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
#ifdef GRAPHOS_GUI
    setModel(new CamerasModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void CamerasComponent::createView()
{
#ifdef GRAPHOS_GUI
    setView(new CamerasViewImp());
#endif // GRAPHOS_GUI
}

void CamerasComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new CamerasPresenterImp(dynamic_cast<CamerasView *>(view()),
                                         dynamic_cast<CamerasModel *>(model())));
    connect(dynamic_cast<CamerasPresenter *>(presenter()), &CamerasPresenter::updateCameras,
            this, &CamerasComponent::onUpdateCameras);
#endif // GRAPHOS_GUI
}

void CamerasComponent::createCommand()
{
    setCommand(std::make_shared<CamerasCommand>());
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
