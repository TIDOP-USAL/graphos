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

#include <QAction>
#include <QApplication>

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
  QIcon iconZoomIn;
  iconZoomIn.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomIn->setIcon(iconZoomIn);

  QIcon iconZoomOut;
  iconZoomOut.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-out.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomOut->setIcon(iconZoomOut);

  QIcon iconZoomExtend;
  iconZoomExtend.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-magnifying-glass-with-expand-sign.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomExtend->setIcon(iconZoomExtend);

  QIcon iconZoom11;
  iconZoom11.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-one-to-one.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoom11->setIcon(iconZoom11);

  this->addAction(mActionZoomIn);
  this->addAction(mActionZoomOut);
  this->addAction(mActionZoomExtend);
  this->addAction(mActionZoom11);

  retranslate();
}

void ImageContextMenu::initSignalAndSlots()
{
  connect(mActionZoomIn,      &QAction::triggered, this, &ImageContextMenu::zoomIn);
  connect(mActionZoomOut,     &QAction::triggered, this, &ImageContextMenu::zoomOut);
  connect(mActionZoomExtend,  &QAction::triggered, this, &ImageContextMenu::zoomExtend);
  connect(mActionZoom11,      &QAction::triggered, this, &ImageContextMenu::zoom11);
}

void ImageContextMenu::retranslate()
{
  mActionZoomIn->setText(QApplication::translate("ImageContextMenu", "Zoom In"));
  mActionZoomOut->setText(QApplication::translate("ImageContextMenu", "Zoom Out"));
  mActionZoomExtend->setText(QApplication::translate("ImageContextMenu", "Zoom Extend"));
  mActionZoom11->setText(QApplication::translate("ImageContextMenu", "Zoom 1:1"));
}


} // namespace graphos