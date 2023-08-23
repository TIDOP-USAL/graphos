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

#ifndef GRAPHOS_IMAGE_CONTEXT_MENU_H
#define GRAPHOS_IMAGE_CONTEXT_MENU_H

#include "graphos/widgets/GraphosWidget.h"

class QAction;

namespace graphos
{

class ImageContextMenu
  : public GraphosContextMenu
{
    Q_OBJECT

public:

    ImageContextMenu(QWidget *parent = nullptr);
    ~ImageContextMenu() override = default;

signals:

    void zoomIn();
    void zoomOut();
    void zoomExtend();
    void zoom11();

private:

    void init();
    void initSignalAndSlots();

// GraphosContextMenu

private slots:

    void retranslate() override;

private:

    QAction *mActionZoomIn;
    QAction *mActionZoomOut;
    QAction *mActionZoomExtend;
    QAction *mActionZoom11;

};

} // namespace graphos

#endif // GRAPHOS_IMAGE_CONTEXT_MENU_H
