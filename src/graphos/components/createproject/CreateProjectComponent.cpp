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

#include "CreateProjectComponent.h"

#ifdef GRAPHOS_GUI
#include "graphos/components/createproject/impl/CreateProjectModel.h"
#include "graphos/components/createproject/impl/CreateProjectView.h"
#include "graphos/components/createproject/impl/CreateProjectPresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/createproject/impl/CreateProjectCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

/* Qt */
#include <QAction>
#include <QString>

namespace graphos
{



CreateProjectComponent::CreateProjectComponent(Application *application)
  : ComponentBase(application)
{
    init();
}

CreateProjectComponent::~CreateProjectComponent()
{
}

void CreateProjectComponent::init()
{
    setName("New Project");
    setMenu("file");
    setToolbar("file");
    setIcon(QIcon::fromTheme("new-project"));

    createCommand();

#ifndef QT_NO_SHORTCUT
    action()->setShortcut(tr("Ctrl+N"));
#endif // QT_NO_SHORTCUT
}

void CreateProjectComponent::createModel()
{
#ifdef GRAPHOS_GUI
    setModel(new CreateProjectModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void CreateProjectComponent::createView()
{
#ifdef GRAPHOS_GUI
    setView(new CreateProjectViewImp());
#endif // GRAPHOS_GUI
}

void CreateProjectComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new CreateProjectPresenterImp(dynamic_cast<CreateProjectView *>(view()),
                                               dynamic_cast<CreateProjectModel *>(model()),
                                               app()->status()));

    connect(dynamic_cast<CreateProjectPresenter *>(presenter()), &CreateProjectPresenter::project_created,
            this, &CreateProjectComponent::onProjectCreated);
#endif // GRAPHOS_GUI
}

void CreateProjectComponent::createCommand()
{
    setCommand(std::make_shared<CreateProjectCommand>());
}

void CreateProjectComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    action()->setEnabled(!app_status->isEnabled(AppStatus::Flag::processing));
}

void CreateProjectComponent::onProjectCreated()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    app_status->clear();
    app_status->activeFlag(AppStatus::Flag::project_exists, true);

    emit project_created();
}

} // namespace graphos
