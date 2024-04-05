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

#include "RecentProjectsPresenter.h"

#include "graphos/components/recentprojects/RecentProjectsModel.h"
#include "graphos/components/recentprojects/RecentProjectsView.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

RecentProjectsPresenterImp::RecentProjectsPresenterImp(RecentProjectsView *view,
                                                       RecentProjectsModel *model,
                                                       AppStatus *status)
  : RecentProjectsPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
    RecentProjectsPresenterImp::init();
    RecentProjectsPresenterImp::initSignalAndSlots();
}

RecentProjectsPresenterImp::~RecentProjectsPresenterImp()
{
}

void RecentProjectsPresenterImp::clearHistory()
{
    mModel->clear();
}

void RecentProjectsPresenterImp::open()
{

}

void RecentProjectsPresenterImp::init()
{
    mView->setHistory(mModel->history());
}

void RecentProjectsPresenterImp::initSignalAndSlots()
{
    connect(mView, &RecentProjectsView::clearHistory, this, &RecentProjectsPresenterImp::clearHistory);
    connect(&Application::instance(), &Application::update_history, [&]() {
        mView->setHistory(mModel->history());
    });

    connect(mView, &RecentProjectsView::openProject, this, &RecentProjectsPresenter::open_project);
}

} // namespace graphos