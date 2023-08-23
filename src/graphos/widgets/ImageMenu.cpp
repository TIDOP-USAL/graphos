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

#include "ImageMenu.h"

TL_DISABLE_WARNINGS
#include <QAction>
#include <QApplication>
TL_DEFAULT_WARNINGS

namespace graphos
{

ImageContextMenu::ImageContextMenu(QWidget *parent)
  : GraphosContextMenu(parent),
    mActionZoomIn(new QAction(this)),
    mActionZoomOut(new QAction(this)),
    mActionZoomExtend(new QAction(this)),
    mActionZoom11(new QAction(this))
{
    init();
    initSignalAndSlots();
}

void ImageContextMenu::init()
{
    mActionZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
    mActionZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
    mActionZoomExtend->setIcon(QIcon::fromTheme("zoom-extend"));
    mActionZoom11->setIcon(QIcon::fromTheme("zoom-to-actual-size"));

    this->addAction(mActionZoomIn);
    this->addAction(mActionZoomOut);
    this->addAction(mActionZoomExtend);
    this->addAction(mActionZoom11);

    retranslate();
}

void ImageContextMenu::initSignalAndSlots()
{
    connect(mActionZoomIn, &QAction::triggered, this, &ImageContextMenu::zoomIn);
    connect(mActionZoomOut, &QAction::triggered, this, &ImageContextMenu::zoomOut);
    connect(mActionZoomExtend, &QAction::triggered, this, &ImageContextMenu::zoomExtend);
    connect(mActionZoom11, &QAction::triggered, this, &ImageContextMenu::zoom11);
}

void ImageContextMenu::retranslate()
{
    mActionZoomIn->setText(QApplication::translate("ImageContextMenu", "Zoom In"));
    mActionZoomOut->setText(QApplication::translate("ImageContextMenu", "Zoom Out"));
    mActionZoomExtend->setText(QApplication::translate("ImageContextMenu", "Zoom Extend"));
    mActionZoom11->setText(QApplication::translate("ImageContextMenu", "Zoom 1:1"));
}


} // namespace graphos
