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

#include "GroundControlPointsComponent.h"

#ifdef GRAPHOS_GUI
#include "graphos/components/gcps/impl/GroundControlPointsModel.h"
#include "graphos/components/gcps/impl/GroundControlPointsView.h"
#include "graphos/components/gcps/impl/GroundControlPointsPresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/gcps/impl/GroundControlPointsCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

GroundControlPointsComponent::GroundControlPointsComponent(Application *application)
  : ComponentBase(application)
{
    init();
}

GroundControlPointsComponent::~GroundControlPointsComponent()
{
}

void GroundControlPointsComponent::init()
{
    setName("Ground Control Points");
    setMenu("tools");
    setToolbar("tools");
    setIcon(QIcon::fromTheme("orientation"));

    createCommand();
}

void GroundControlPointsComponent::createModel()
{
#ifdef GRAPHOS_GUI
    setModel(new GroundControlPointsModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void GroundControlPointsComponent::createView()
{
#ifdef GRAPHOS_GUI
    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setView(new GroundControlPointsViewImp(nullptr, f));
    connect(dynamic_cast<GroundControlPointsView *>(view()), &GroundControlPointsView::select_crs,
            this, &GroundControlPointsComponent::select_crs);
#endif // GRAPHOS_GUI
}

void GroundControlPointsComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new GroundControlPointsPresenterImp(dynamic_cast<GroundControlPointsView *>(view()),
                                                     dynamic_cast<GroundControlPointsModel *>(model())));
#endif // GRAPHOS_GUI
}

void GroundControlPointsComponent::createCommand()
{
    setCommand(std::make_shared<GroundControlPointsCommand>());
}

void GroundControlPointsComponent::update()
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

void GroundControlPointsComponent::setCRS(const QString &crs)
{
#ifdef GRAPHOS_GUI
    if (view())
        dynamic_cast<GroundControlPointsView *>(view())->setCrs(crs);
#endif // GRAPHOS_GUI
}

} // namespace graphos
