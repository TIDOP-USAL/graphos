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

#ifndef GRAPHOS_MAP_VIEWER_POLYGON_H
#define GRAPHOS_MAP_VIEWER_POLYGON_H

#include "graphos/widgets/mapviewer/MapPolygon.h"

#include <QGeoView/QGVDrawItem.h>

namespace graphos
{
	
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
    auto projShape() const -> QPainterPath override;
    void projPaint(QPainter *painter) override;
    auto projAnchor() const -> QPointF override;
    auto projTransform() const -> QTransform override;
    auto projTooltip(const QPointF &projPos) const -> QString override;
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

}

#endif // GRAPHOS_MAP_VIEWER_POLYGON_H