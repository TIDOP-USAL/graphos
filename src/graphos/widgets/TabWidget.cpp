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

#include "TabWidget.h"

#include "graphos/widgets/ImageMenu.h"
#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/Viewer3d.h"

TL_DISABLE_WARNINGS
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QTabBar>
TL_DEFAULT_WARNINGS

namespace graphos
{

TabWidget::TabWidget(QWidget *parent)
  : QTabWidget(parent)
{
    initUI();
    initSignalAndSlots();
}

void TabWidget::initUI()
{
    this->setObjectName(QString("TabWidget"));
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setTabsClosable(true);

    this->initActions();
    this->initMenu();

    this->retranslate();
    this->update();
}

void TabWidget::initActions()
{
    mCloseTab = new QAction(this);
    mCloseAllTabs = new QAction(this);
    mCloseAllTabsButCurrentOne = new QAction(this);
}

void TabWidget::initMenu()
{
    mMenu = new QMenu(this);
    mMenu->addAction(mCloseTab);
    mMenu->addAction(mCloseAllTabs);
    mMenu->addAction(mCloseAllTabsButCurrentOne);
}

void TabWidget::initSignalAndSlots()
{
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onTabWidgetContextMenu(const QPoint &)));
}

void TabWidget::clear()
{
    int n = this->count();
    for (int i = 0; i < n; i++) {
        closeTab(0);
    }

    update();
}

void TabWidget::update()
{

}

void TabWidget::retranslate()
{
    mCloseTab->setText(QApplication::translate("TabWidget", "Close"));
    mCloseAllTabs->setText(QApplication::translate("TabWidget", "Close all tabs"));
    mCloseAllTabsButCurrentOne->setText(QApplication::translate("TabWidget", "Close all tabs but current one"));
}

void TabWidget::closeTab(int tabId)
{
    if (tabId != -1) {
        GraphicViewer *graphicViewer = dynamic_cast<GraphicViewer *>(this->widget(tabId));
        Viewer3D *viewer3D = dynamic_cast<Viewer3D *>(this->widget(tabId));
        this->removeTab(tabId);
        if (graphicViewer) {
            delete graphicViewer;
            graphicViewer = nullptr;
        } else if (viewer3D) {
            delete viewer3D;
            viewer3D = nullptr;
        }

        if (this->count() == 0)
            emit all_tabs_closed();
    }
}

void TabWidget::setCurrentTab(int tabId)
{
}

auto TabWidget::fileTab(const QString& file) const -> int
{
    int id = -1;

    for (int i = 0; i < this->count(); i++) {

        if (this->tabToolTip(i) == file) {
            id = i;
            break;
        }
    }

    return id;
}

void TabWidget::onTabChanged(int tabId)
{
    GraphicViewer *graphic_viewer = dynamic_cast<GraphicViewer *>(this->widget(tabId));
    Viewer3D *viewer_3d = dynamic_cast<Viewer3D *>(this->widget(tabId));

    if (graphic_viewer) {
        emit imageActive(true);
        emit model3dActive(false);
        emit model3dChange(nullptr);
    } else if (viewer_3d) {
        emit model3dActive(true);
        emit imageActive(false);
        emit model3dChange(viewer_3d);
    } else {
        emit imageActive(false);
        emit model3dActive(false);
        emit model3dChange(nullptr);
    }

    update();
}

void TabWidget::onTabWidgetContextMenu(const QPoint &position)
{
    if (position.isNull()) return;

    if (this->tabBar() == nullptr) return;

    int tab_index = this->tabBar()->tabAt(position);

    if (tab_index == -1) return;

    QPoint globalPos = mapToGlobal(position);
    if (QAction *action = mMenu->exec(globalPos)) {
        if (action->text() == tr("Close")) {
            closeTab(tab_index);
        } else if (action->text() == tr("Close all tabs")) {
            int n = this->count();
            for (int i = 0; i < n; i++) {
                closeTab(0);
            }
        } else if (action->text() == tr("Close all tabs but current one")) {
            int n = this->count();
            int tab_to_close_id = 0;
            QString tab_text = this->tabBar()->tabText(tab_index);
            for (int i = 0; i < n; i++) {
                if (this->tabBar()->tabText(tab_to_close_id).compare(tab_text) == 0) {
                    tab_to_close_id = 1;
                } else {
                    closeTab(tab_to_close_id);
                }
            }
        }
    }
}

void TabWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        this->retranslate();
    }
}


} // namespace graphos
