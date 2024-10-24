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

#include "RecentProjectsComponent.h"

#include "graphos/components/recentprojects/impl/RecentProjectsModel.h"
#include "graphos/components/recentprojects/impl/RecentProjectsView.h"
#include "graphos/components/recentprojects/impl/RecentProjectsPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QString>

namespace graphos
{


RecentProjectsComponent::RecentProjectsComponent(Application *application)
  : MultiComponentBase(application)
{
    init();
}

RecentProjectsComponent::~RecentProjectsComponent()
{
}

void RecentProjectsComponent::init()
{
    setName("Recent Projects");
    setMenu("file");

    createModel();
    createView();
    createPresenter();
}

void RecentProjectsComponent::createModel()
{
    setModel(new RecentProjectsModelImp());
}

void RecentProjectsComponent::createView()
{
    setView(new RecentProjectsViewImp());
}

void RecentProjectsComponent::createPresenter()
{
    setPresenter(new RecentProjectsPresenterImp(dynamic_cast<RecentProjectsView *>(view()),
                                                dynamic_cast<RecentProjectsModel *>(model()),
                                                app()->status()));

    connect(dynamic_cast<RecentProjectsPresenter *>(presenter()), &RecentProjectsPresenter::open_project,
            this, &RecentProjectsComponent::open_project);
}

void RecentProjectsComponent::update()
{
}

} // namespace graphos
