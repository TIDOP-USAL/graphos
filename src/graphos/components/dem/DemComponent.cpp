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

#include "DemComponent.h"

#ifdef GRAPHOS_GUI
#include "graphos/components/dem/impl/DemModel.h"
#include "graphos/components/dem/impl/DemView.h"
#include "graphos/components/dem/impl/DemPresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/dem/impl/DemCommand.h"
#include "graphos/core/project/Project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

DemComponent::DemComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

void DemComponent::init()
{
    setName(QApplication::translate("DemComponent", "Digital Elevation Model (DEM)"));
    setMenu("workflow");
    setIcon(QIcon::fromTheme("dem"));
    createCommand();
}

void DemComponent::createModel()
{
#ifdef GRAPHOS_GUI
    setModel(new DemModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void DemComponent::createView()
{
#ifdef GRAPHOS_GUI
    setView(new DemViewImp());

    connect(dynamic_cast<DemView *>(view()), &DemView::select_crs,
            this, &DemComponent::select_crs);
#endif // GRAPHOS_GUI
}

void DemComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new DemPresenterImp(dynamic_cast<DemView *>(view()),
                                     dynamic_cast<DemModel *>(model())));
#endif // GRAPHOS_GUI
}

void DemComponent::createCommand()
{
    setCommand(std::make_shared<DemCommand>());
}

void DemComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool absolute_oriented = app_status->isEnabled(AppStatus::Flag::absolute_oriented);
    bool dense_model = app_status->isEnabled(AppStatus::Flag::dense_model);
    //bool mesh = app_status->isEnabled(AppStatus::Flag::mesh);
    action()->setEnabled(project_exists && absolute_oriented && dense_model && !processing);
}

void DemComponent::onRunning()
{
    TaskComponent::onRunning();
}

void DemComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::dtm, true);
    app_status->activeFlag(AppStatus::Flag::dsm, true);
}

void DemComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
    app_status->activeFlag(AppStatus::Flag::dtm, false);
    app_status->activeFlag(AppStatus::Flag::dsm, false);
}

void DemComponent::setCrs(const QString &crs)
{
#ifdef GRAPHOS_GUI
    if (view())
        dynamic_cast<DemView *>(view())->setCrs(crs);
#endif // GRAPHOS_GUI
}

} // namespace graphos
