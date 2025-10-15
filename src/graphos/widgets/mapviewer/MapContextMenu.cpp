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

#include "MapContextMenu.h"

TL_DISABLE_WARNINGS
#include <QAction>
#include <QApplication>
#include <QMenu>
TL_DEFAULT_WARNINGS

namespace graphos
{

MapContextMenu::MapContextMenu(QWidget *parent)
  : GraphosContextMenu(parent),
    mMenuBaseMap(new QMenu),
    //mActionOSM(new QAction(this)),
    //mActionGoogleSatellite(new QAction(this)),
    mActionGoogleSchema(new QAction(this)),
    mActionGoogleHybrid(new QAction(this)), 
    mActionBingSatellite(new QAction(this)),
    mActionBingSchema(new QAction(this)),
    mActionBingHybrid(new QAction(this)),
    mActionZoomIn(new QAction(this)),
    mActionZoomOut(new QAction(this)),
    mActionZoomExtend(new QAction(this)),
    mActionZoom11(new QAction(this))
{
    init();
    initSignalAndSlots();
}

void MapContextMenu::init()
{

    mActionZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
    mActionZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
    mActionZoomExtend->setIcon(QIcon::fromTheme("zoom-extend"));
    mActionZoom11->setIcon(QIcon::fromTheme("zoom-to-actual-size"));

    //mMenuBaseMap->addAction(mActionOSM);
    //mMenuBaseMap->addAction(mActionGoogleSatellite);

    mActionGoogleSchema->setCheckable(true);
    mActionGoogleHybrid->setCheckable(true);
    mActionBingSatellite->setCheckable(true);
    mActionBingSchema->setCheckable(true);
    mActionBingHybrid->setCheckable(true);

    auto baseMapGroup = new QActionGroup(this);
    baseMapGroup->setExclusive(true);

    baseMapGroup->addAction(mActionGoogleSchema);
    baseMapGroup->addAction(mActionGoogleHybrid);
    baseMapGroup->addAction(mActionBingSatellite);
    baseMapGroup->addAction(mActionBingSchema);
    baseMapGroup->addAction(mActionBingHybrid);

    mActionGoogleSchema->setChecked(true);
    mMenuBaseMap->addActions(baseMapGroup->actions());

    this->addMenu(mMenuBaseMap);
    this->addSeparator();
    this->addAction(mActionZoomIn);
    this->addAction(mActionZoomOut);
    this->addAction(mActionZoomExtend);
    this->addAction(mActionZoom11);

    retranslate();
}

void MapContextMenu::initSignalAndSlots()
{
    //connect(mActionOSM, &QAction::triggered, this, &MapContextMenu::baseMapOSM);
    //connect(mActionGoogleSatellite, &QAction::triggered, this, &MapContextMenu::baseMapGoogleSatellite);
    connect(mActionGoogleSchema, &QAction::triggered, this, &MapContextMenu::baseMapGoogleSchema);
    connect(mActionGoogleHybrid, &QAction::triggered, this, &MapContextMenu::baseMapGoogleHybrid);
    connect(mActionBingSchema, &QAction::triggered, this, &MapContextMenu::baseMapBingSchema);
    connect(mActionBingHybrid, &QAction::triggered, this, &MapContextMenu::baseMapBingHybrid);
    connect(mActionBingSatellite, &QAction::triggered, this, &MapContextMenu::baseMapBingSatellite);

    connect(mActionZoomIn, &QAction::triggered, this, &MapContextMenu::zoomIn);
    connect(mActionZoomOut, &QAction::triggered, this, &MapContextMenu::zoomOut);
    connect(mActionZoomExtend, &QAction::triggered, this, &MapContextMenu::zoomExtend);
    connect(mActionZoom11, &QAction::triggered, this, &MapContextMenu::zoom11);
}

void MapContextMenu::retranslate()
{
    mMenuBaseMap->setTitle(QApplication::translate("MapContextMenu", "Base Map"));
    //mActionOSM->setText(QApplication::translate("MapContextMenu", "Open Street Map"));
    //mActionGoogleSatellite->setText(QApplication::translate("MapContextMenu", "Google Satellite"));
    mActionGoogleSchema->setText(QApplication::translate("MapContextMenu", "Google Schema"));
    mActionGoogleHybrid->setText(QApplication::translate("MapContextMenu", "Google Hybrid"));
    mActionBingSatellite->setText(QApplication::translate("MapContextMenu", "Bing Satellite"));
    mActionBingSchema->setText(QApplication::translate("MapContextMenu", "Bing Schema"));
    mActionBingHybrid->setText(QApplication::translate("MapContextMenu", "Bing Hybrid"));

    mActionZoomIn->setText(QApplication::translate("MapContextMenu", "Zoom In"));
    mActionZoomOut->setText(QApplication::translate("MapContextMenu", "Zoom Out"));
    mActionZoomExtend->setText(QApplication::translate("MapContextMenu", "Zoom Extend"));
    mActionZoom11->setText(QApplication::translate("MapContextMenu", "Zoom 1:1"));
}


} // namespace graphos
