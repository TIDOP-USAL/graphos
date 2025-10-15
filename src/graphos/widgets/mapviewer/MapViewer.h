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

#ifndef GRAPHOS_MAP_VIEWER_H
#define GRAPHOS_MAP_VIEWER_H

#include <QWidget>

#include <QGeoView/QGVMap.h>
#include <QGeoView/QGVWidget.h>

class QGVLayerGoogle;
class QGVLayerOSM;
class QGVLayerBing;

namespace graphos
{

class MapContextMenu;

class MapViewer 
  : public QWidget
{
    Q_OBJECT

public:

    enum class BaseMap
    {
        osm,
        google_schema,
        google_satellite,
        google_hybrid,
        bing_schema,
        bing_satellite,
        bing_hybrid
    };

public:

    explicit MapViewer(QWidget *parent = nullptr);
    ~MapViewer();

public:

    void loadImage(const QString &imagePath);
    void loadShapefile(const QString &shapefilePath);
    void loadGeoTiff(const QString &tifPath);

public slots:

    void zoomExtend();
    void zoomIn();
    void zoomOut();
    void changeBaseMap(BaseMap baseMap);

protected slots:

    void showContextMenu(const QPoint &position);

private:

    void init();
    void initSignalsAndSlots();
    QGV::GeoRect maxGeoExtent() const;

private:

    QGVMap *mMap;

    //QGVLayerOSM *mBaseLayerOSM;
    //QGVLayerGoogle *mBaseLayerGoogleSatellite;
    QGVLayerGoogle *mBaseLayerGoogleHybrid;
    QGVLayerGoogle *mBaseLayerGoogleSchema;
    QGVLayerBing *mBaseLayerBingSatellite;
    QGVLayerBing *mBaseLayerBingHybrid;
    QGVLayerBing *mBaseLayerBingSchema;
    QRectF mVectorExtent;
    MapContextMenu *mContextMenu;
};


}

#endif // GRAPHOS_MAP_VIEWER_H