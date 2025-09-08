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
#include <QGeoView/QGVDrawItem.h>
#include <QGeoView/QGVLayerTiles.h>

#include <tidop/math/geometry/affine.h>

typedef QList<QGV::GeoPos> PointList;

class QGVPolygon : public QGVDrawItem
{
    Q_OBJECT

public:
    explicit QGVPolygon(const PointList &geoPoints, QColor stroke, QColor fill);

    void setPoints(const PointList &geoPoints);
    PointList getPoints() const;

private:
    void onProjection(QGVMap *geoMap) override;
    QPainterPath projShape() const override;
    void projPaint(QPainter *painter) override;
    QPointF projAnchor() const override;
    QTransform projTransform() const override;
    QString projTooltip(const QPointF &projPos) const override;
    void projOnMouseClick(const QPointF &projPos) override;
    void projOnMouseDoubleClick(const QPointF &projPos) override;
    void projOnObjectStartMove(const QPointF &projPos) override;
    void projOnObjectMovePos(const QPointF &projPos) override;
    void projOnObjectStopMove(const QPointF &projPos) override;

private:

    PointList mGeoPoints;
    QPolygonF mProjPoints;
    QColor mColorStroke;
    QColor mColorFill;
};


class GDALDataset;
namespace tl
{
class ImageReader;
}


class RasterTiledLayer 
  : public QGVLayerTiles
{
    Q_OBJECT

public:

    RasterTiledLayer(const QString &tifPath);
    ~RasterTiledLayer();

    QGV::GeoRect maxGeoExtent() const;

protected:

    void onProjection(QGVMap *geoMap) override;
    int minZoomlevel() const override;
    int maxZoomlevel() const override;
    void request(const QGV::GeoTilePos &tilePos) override;
    void cancel(const QGV::GeoTilePos &tilePos) override;

private:

    QString mTifPath;
    GDALDataset *mDataset;
    QRect mTileGridBounds;
    QGV::GeoRect mGeoExtent;
    std::unique_ptr<tl::ImageReader> reader;
    tl::Affine<double, 2> mGeoreference;
    double mScaleMin;
};


namespace graphos
{


class MapViewer 
  : public QWidget
{
    Q_OBJECT

public:

    explicit MapViewer(QWidget *parent = nullptr);
    ~MapViewer();

public:

    void loadImage(const QString &imagePath);
    void loadShapefile(const QString &shapefilePath);
    void loadGeoTiff(const QString &tifPath);
    //void setProjection(const QString &epsgCode);
    void zoomExtend() const;

private:

    QGV::GeoRect maxGeoExtent() const;

private:

    QGVMap *mMap;
    QRectF mVectorExtent;
};


}

#endif // GRAPHOS_MAP_VIEWER_H