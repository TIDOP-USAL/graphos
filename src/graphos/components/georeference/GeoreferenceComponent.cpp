#include "GeoreferenceComponent.h"
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

#include "GeoreferenceComponent.h"

#ifdef GRAPHOS_GUI
#include "graphos/components/georeference/impl/GeoreferenceModel.h"
#include "graphos/components/georeference/impl/GeoreferenceView.h"
#include "graphos/components/georeference/impl/GeoreferencePresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/georeference/impl/GeoreferenceCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

GeoreferenceComponent::GeoreferenceComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

GeoreferenceComponent::~GeoreferenceComponent()
{
}

void GeoreferenceComponent::init()
{
    setName("Georeference");
    setMenu("tools");
    setToolbar("tools");
    setIcon(QIcon::fromTheme("orientation"));

    createCommand();
}

void GeoreferenceComponent::createModel()
{
#ifdef GRAPHOS_GUI
    setModel(new GeoreferenceModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void GeoreferenceComponent::createView()
{
#ifdef GRAPHOS_GUI
    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setView(new GeoreferenceViewImp(nullptr, f));
#endif // GRAPHOS_GUI
}

void GeoreferenceComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new GeoreferencePresenterImp(dynamic_cast<GeoreferenceView *>(view()),
                                              dynamic_cast<GeoreferenceModel *>(model())));
#endif // GRAPHOS_GUI
}

void GeoreferenceComponent::createCommand()
{
    setCommand(std::make_shared<GeoreferenceCommand>());
}

void GeoreferenceComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool images_added = app_status->isEnabled(AppStatus::Flag::images_added);
    action()->setEnabled(project_exists && images_added && !processing);
}

void GeoreferenceComponent::onRunning()
{
    TaskComponent::onRunning();
}

void GeoreferenceComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();
    app_status->activeFlag(AppStatus::Flag::absolute_oriented, true);
    app_status->activeFlag(AppStatus::Flag::project_modified, true);
}

void GeoreferenceComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
    app_status->activeFlag(AppStatus::Flag::absolute_oriented, false);
}

} // namespace graphos
