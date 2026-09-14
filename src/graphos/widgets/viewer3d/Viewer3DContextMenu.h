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

#pragma once

#include "graphos/widgets/GraphosWidget.h"

class QAction;

namespace graphos
{

class Viewer3DContextMenu : public GraphosContextMenu
{
    Q_OBJECT

public:
    explicit Viewer3DContextMenu(QWidget *parent = nullptr);
    ~Viewer3DContextMenu() override = default;

    void setPerspectiveProjection(bool enabled);
    void setPointLighting(bool enabled);
    void setPointLightingAvailable(bool available);

signals:
    void globalZoom();
    void viewFront();
    void viewTop();
    void viewLeft();
    void viewRight();
    void viewBack();
    void viewBottom();
    void filterEDL(bool);
    void perspectiveProjection(bool);
    void pointLighting(bool);
    void pointSize();

private:
    void init();
    void initSignalAndSlots();

private slots:
    void retranslate() override;

private:
    QAction *mActionGlobalZoom;
    QAction *mActionViewFront;
    QAction *mActionViewTop;
    QAction *mActionViewLeft;
    QAction *mActionViewRight;
    QAction *mActionViewBack;
    QAction *mActionViewBottom;
    QAction *mActionEDLFilter;
    QAction *mActionPerspective;
    QAction *mActionPointLighting;
    QAction *mActionPointSize;
};

}