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

#include "SaveProjectComponent.h"

#include "graphos/components/saveproject/impl/SaveProjectModel.h"
#include "graphos/components/saveproject/impl/SaveProjectView.h"
#include "graphos/components/saveproject/impl/SaveProjectPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{


SaveProjectComponent::SaveProjectComponent(Application *application)
  : ComponentBase(application)
{
    init();
}

SaveProjectComponent::~SaveProjectComponent()
{
}

void SaveProjectComponent::init()
{
    setName("Save Project");
    setMenu("file");
    setToolbar("file");
    setIcon(QIcon::fromTheme("save"));

#ifndef QT_NO_SHORTCUT
    action()->setShortcut(tr("Ctrl+S"));
#endif // QT_NO_SHORTCUT
}

void SaveProjectComponent::createModel()
{
    setModel(new SaveProjectModelImp(app()->project()));
}

void SaveProjectComponent::createView()
{
    setView(new SaveProjectViewImp());
}

void SaveProjectComponent::createPresenter()
{
    setPresenter(new SaveProjectPresenterImp(dynamic_cast<SaveProjectView *>(view()),
                                             dynamic_cast<SaveProjectModel *>(model()),
                                             app()->status()));
}

void SaveProjectComponent::createCommand()
{
}

void SaveProjectComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool loading_images = app_status->isEnabled(AppStatus::Flag::loading_images);
    bool project_modified = app_status->isEnabled(AppStatus::Flag::project_modified);
    action()->setEnabled(!loading_images && !processing && project_modified);
}

} // namespace graphos
