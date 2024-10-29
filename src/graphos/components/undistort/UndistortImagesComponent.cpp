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


#include "UndistortImagesComponent.h"

#ifdef GRAPHOS_GUI
#include "graphos/components/undistort/impl/UndistortImagesModel.h"
#include "graphos/components/undistort/impl/UndistortImagesView.h"
#include "graphos/components/undistort/impl/UndistortImagesPresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/undistort/impl/UndistortImagesCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

UndistortImagesComponent::UndistortImagesComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

UndistortImagesComponent::~UndistortImagesComponent()
{
}

void UndistortImagesComponent::init()
{
    setName(tr("Undistort Images"));
    setMenu("tools");
    setToolbar("tools");
    setIcon(QIcon::fromTheme("undistort"));

    createCommand();
}

void UndistortImagesComponent::createModel()
{
#ifdef GRAPHOS_GUI
    setModel(new UndistortImagesModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void UndistortImagesComponent::createView()
{
#ifdef GRAPHOS_GUI
    setView(new UndistortImagesViewImp());
#endif // GRAPHOS_GUI
}

void UndistortImagesComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new UndistortImagesPresenterImp(dynamic_cast<UndistortImagesView *>(view()),
                                                 dynamic_cast<UndistortImagesModel *>(model())));
#endif // GRAPHOS_GUI
}

void UndistortImagesComponent::createCommand()
{
    setCommand(std::make_shared<UndistortImagesCommand>());
}

void UndistortImagesComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    action()->setEnabled(app_status->isEnabled(AppStatus::Flag::project_exists) &&
                         !app_status->isEnabled(AppStatus::Flag::processing) &&
                         app_status->isEnabled(AppStatus::Flag::oriented));
}

void UndistortImagesComponent::onRunning()
{
    TaskComponent::onRunning();
}

void UndistortImagesComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();
}

void UndistortImagesComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
}


} // namespace graphos
