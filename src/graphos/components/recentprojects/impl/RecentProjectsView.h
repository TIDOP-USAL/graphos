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

#ifndef GRAPHOS_RECENT_PROJECTS_VIEW_H
#define GRAPHOS_RECENT_PROJECTS_VIEW_H

#include "graphos/components/recentprojects/RecentProjectsView.h"

class QAction;

namespace graphos
{

class RecentProjectsViewImp
  : public RecentProjectsView
{

    Q_OBJECT

public:

    explicit RecentProjectsViewImp(QWidget *parent = nullptr);
    ~RecentProjectsViewImp() override = default;

private:

    void init();
    void initSignalAndSlots();

private slots:

    void update() const;
    void retranslate();
    void openFromHistory();

// RecentProjectsView interface

public:

    void setHistory(const QStringList &history) override;

// QWidget interface

protected:

    void changeEvent(QEvent *event) override;

private:

    QAction *mActionNotRecentProjects;
    QAction *mActionClearHistory;
    std::vector<QAction *> mHistory;
};

} // namespace graphos

#endif // GRAPHOS_RECENT_PROJECTS_VIEW_H
