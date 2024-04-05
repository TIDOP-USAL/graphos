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
 
#include "RecentProjectsView.h"

#include <QAction>
#include <QApplication>
#include <QFileInfo>

namespace graphos
{

RecentProjectsViewImp::RecentProjectsViewImp(QWidget *parent)
  : RecentProjectsView(parent),
    mActionNotRecentProjects(new QAction(this)),
    mActionClearHistory(new QAction(this))
{
    this->init();
    this->initSignalAndSlots();
}

void RecentProjectsViewImp::init()
{
    mActionNotRecentProjects->setEnabled(false);

    mActionClearHistory->setIcon(QIcon::fromTheme("delete-history"));

    this->addAction(mActionNotRecentProjects);
    this->addSeparator();
    this->addAction(mActionClearHistory);

    retranslate();
}

void RecentProjectsViewImp::initSignalAndSlots()
{
    connect(mActionClearHistory, &QAction::triggered, this, &RecentProjectsViewImp::clearHistory);
}

void RecentProjectsViewImp::update() const
{
    mActionNotRecentProjects->setVisible(mHistory.empty());
    mActionClearHistory->setEnabled(!mHistory.empty());
}

void RecentProjectsViewImp::retranslate()
{
    this->setTitle(QApplication::translate("RecentProjectsView", "Recent Projects", nullptr));
    mActionNotRecentProjects->setText(QApplication::translate("RecentProjectsView", "Not recent projects", nullptr));
    mActionClearHistory->setText(QApplication::translate("RecentProjectsView", "Clear History", nullptr));
}

void RecentProjectsViewImp::openFromHistory()
{
    if (auto action = qobject_cast<QAction *>(sender()))
        emit openProject(action->data().toString());
}

void RecentProjectsViewImp::setHistory(const QStringList &history)
{
    int n = history.size();

    if (n == 0) {
        while (!mHistory.empty()) {
            disconnect(mHistory[0], SIGNAL(triggered()), this, SLOT(openFromHistory()));
            this->removeAction(mHistory[0]);
            mHistory.erase(mHistory.begin());
        }
    }

    for (int r = 0; r < n; r++) {

        QString project_path = tr("&%1 %2").arg(r + 1).arg(QFileInfo(history[r]).fileName());

        if (mHistory.size() == static_cast<size_t>(r)) {

            auto action = new QAction(project_path, this);
            action->setData(history[r]);
            action->setToolTip(history[r]);
            mHistory.push_back(action);
            connect(mHistory[static_cast<size_t>(r)], SIGNAL(triggered()), this, SLOT(openFromHistory()));

            this->insertAction(mActionNotRecentProjects, mHistory[static_cast<size_t>(r)]);
            this->setToolTipsVisible(true);

        } else {

            mHistory[static_cast<size_t>(r)]->setText(project_path);
            mHistory[static_cast<size_t>(r)]->setData(history[r]);
            mHistory[static_cast<size_t>(r)]->setToolTip(history[r]);

        }
    }

    update();
}

void RecentProjectsViewImp::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
}

} // namespace graphos