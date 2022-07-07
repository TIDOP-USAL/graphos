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

#ifndef GRAPHOS_RECENT_PROJECTS_PRESENTER_H
#define GRAPHOS_RECENT_PROJECTS_PRESENTER_H

#include "graphos/components/recentprojects/RecentProjectsPresenter.h"

#include <QAction>

namespace graphos
{

class RecentProjectsView;
class RecentProjectsModel;
class AppStatus;

class RecentProjectsPresenterImp
  : public RecentProjectsPresenter
{

  Q_OBJECT

public:

  RecentProjectsPresenterImp(RecentProjectsView *view,
                             RecentProjectsModel *model,
                             AppStatus *status);
  ~RecentProjectsPresenterImp() override;

// RecentProjectsPresenter interface

protected slots:

  void clearHistory() override;

// Presenter interface

public slots:

  void open() override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  RecentProjectsView *mView;
  RecentProjectsModel *mModel;
  AppStatus *mAppStatus;
};

} // namespace graphos

#endif // GRAPHOS_RECENT_PROJECTS_PRESENTER_H
