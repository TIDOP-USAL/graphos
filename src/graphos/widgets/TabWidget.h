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

#ifndef GRAPHOS_TAB_WIDGET_H
#define GRAPHOS_TAB_WIDGET_H

#include <QTabWidget>

class QMenu;
class QAction;


namespace graphos
{

class Viewer3D;


class TabWidget
  : public QTabWidget
{
    Q_OBJECT

public:

    explicit TabWidget(QWidget *parent = nullptr);
    ~TabWidget() override = default;

    auto fileTab(const QString& file) const -> int;
    void clear();

public slots:

    void closeTab(int tabId);
    void setCurrentTab(int tabId);

protected slots:

    void onTabChanged(int tabId);
    void onTabWidgetContextMenu(const QPoint &position);

signals:

    void currentTabChanged(int);
    void imageActive(bool);
    void model3dActive(bool);
    void model3dChange(Viewer3D *);
    void all_tabs_closed();

private:

    void initUI();
    void initActions();
    void initMenu();
    void initSignalAndSlots();
    void retranslate();
    void update();

// QWidget interface

protected:

    void changeEvent(QEvent *event) override;

protected:

    QMenu *mMenu;
    QAction *mCloseTab;
    QAction *mCloseAllTabs;
    QAction *mCloseAllTabsButCurrentOne;
};

} // namespace graphos


#endif // GRAPHOS_TAB_WIDGET_H
