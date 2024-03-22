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

#include "graphos/widgets/GraphicItem.h"

#include <tidop/math/math.h>

TL_DISABLE_WARNINGS
#include <QPainter>
#include <QStyleOptionGraphicsItem>
TL_DEFAULT_WARNINGS

namespace graphos
{

CircleGraphicItem::CircleGraphicItem(QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(),
    mPen(),
    mSize(10.)
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}

CircleGraphicItem::CircleGraphicItem(const QPointF &center, double size, QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(center),
    mPen(),
    mSize(size)
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}

CircleGraphicItem::~CircleGraphicItem()
{

}

auto CircleGraphicItem::pen() const -> QPen
{
    return mPen;
}

void CircleGraphicItem::setPen(const QPen &pen)
{
    mPen = pen;
}

auto CircleGraphicItem::size() -> double
{
    return mSize;
}

void CircleGraphicItem::setSize(double size)
{
    mSize = size;
}

auto CircleGraphicItem::selectedPen() const -> QPen
{
    return mSelectedPen;
}

void CircleGraphicItem::setSelectedPen(const QPen &pen)
{
    mSelectedPen = pen;
}

auto CircleGraphicItem::boundingRect() const -> QRectF
{
    double r = mSize / 2.;
    return QRectF(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
}

auto CircleGraphicItem::shape() const -> QPainterPath
{
    QPainterPath path;
    double r = mSize / 2.;
    path.addRect(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
    return path;
}

void CircleGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (this->isSelected())
        painter->setPen(mSelectedPen);
    else
        painter->setPen(mPen);

    double halfSize = mSize / 2.;
    painter->drawEllipse(mCenter, halfSize, halfSize);
}




CrossGraphicItem::CrossGraphicItem(QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(),
    mPen(),
    mSize(10.)
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}


CrossGraphicItem::CrossGraphicItem(const QPointF &center, double size, QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(center),
    mPen(),
    mSize(size)
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}

CrossGraphicItem::~CrossGraphicItem()
{

}

auto CrossGraphicItem::pen() const -> QPen
{
    return mPen;
}

void CrossGraphicItem::setPen(const QPen &pen)
{
    mPen = pen;
}

auto CrossGraphicItem::size() -> double
{
    return mSize;
}

void CrossGraphicItem::setSize(double size)
{
    mSize = size;
}

auto CrossGraphicItem::selectedPen() const -> QPen
{
    return mSelectedPen;
}

void CrossGraphicItem::setSelectedPen(const QPen &pen)
{
    mSelectedPen = pen;
}

auto CrossGraphicItem::boundingRect() const -> QRectF
{
    double r = mSize / 2.;
    return QRectF(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
}

auto CrossGraphicItem::shape() const -> QPainterPath
{
    QPainterPath path;
    double r = mSize / 2.;
    path.addRect(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
    return path;
}

void CrossGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (this->isSelected())
        painter->setPen(mSelectedPen);
    else
        painter->setPen(mPen);

    double halfSize = mSize / 2.;
    double x = mCenter.x();
    double y = mCenter.y();
    painter->drawLine(QPointF(x - halfSize, y), QPointF(x + halfSize, y));
    painter->drawLine(QPointF(x, y - halfSize), QPointF(x, y + halfSize));
}





KeyPointGraphicsItem::KeyPointGraphicsItem(QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(),
    mPen(),
    mSize(10.),
    mAngle(0.)
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}

KeyPointGraphicsItem::KeyPointGraphicsItem(QPointF center, double size, double angle, QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(center),
    mPen(),
    mSize(size),
    mAngle(angle)
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}

KeyPointGraphicsItem::~KeyPointGraphicsItem()
{

}

auto KeyPointGraphicsItem::pen() const -> QPen
{
    return mPen;
}

void KeyPointGraphicsItem::setPen(const QPen &pen)
{
    mPen = pen;
}

auto KeyPointGraphicsItem::size() -> double
{
    return mSize;
}

void KeyPointGraphicsItem::setSize(double size)
{
    mSize = size;
}

auto KeyPointGraphicsItem::angle() const -> double
{
    return mAngle;
}

void KeyPointGraphicsItem::setAngle(double angle)
{
    mAngle = angle;
}

auto KeyPointGraphicsItem::selectedPen() const -> QPen
{
    return mSelectedPen;
}

void KeyPointGraphicsItem::setSelectedPen(const QPen &pen)
{
    mSelectedPen = pen;
}

auto KeyPointGraphicsItem::boundingRect() const -> QRectF
{
    double r = mSize / 2.;
    return QRectF(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
}

auto KeyPointGraphicsItem::shape() const -> QPainterPath
{
    QPainterPath path;
    double r = 5/*mSize*/ / 2.;
    path.addRect(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
    return path;
}

void KeyPointGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (this->isSelected())
        painter->setPen(mSelectedPen);
    else
        painter->setPen(mPen);

    double r = mSize / 2.;
    painter->drawEllipse(mCenter, r, r);
    QPointF pt2(mCenter.x() + r * cos(mAngle * tl::consts::deg_to_rad<float>), mCenter.y() + r * sin(mAngle * tl::consts::deg_to_rad<float>));
    painter->drawLine(mCenter, pt2);
}




DiagonalCrossGraphicItem::DiagonalCrossGraphicItem(QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(),
    mPen(),
    mSize(10.)
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}


DiagonalCrossGraphicItem::DiagonalCrossGraphicItem(const QPointF &center, double size, QGraphicsItem *parent)
  : GraphicItem(parent),
    mCenter(center),
    mPen(),
    mSize(size),
    mSelectedPen(QColor(QString("#FF0000")))
{
    mSelectedPen.setColor(QColor(QString("#FF0000")));
    mSelectedPen.setWidth(2);
    mSelectedPen.setCosmetic(true);
}

DiagonalCrossGraphicItem::~DiagonalCrossGraphicItem()
{

}

auto DiagonalCrossGraphicItem::pen() const -> QPen
{
    return mPen;
}

void DiagonalCrossGraphicItem::setPen(const QPen &pen)
{
    mPen = pen;
}

auto DiagonalCrossGraphicItem::size() -> double
{
    return mSize;
}

void DiagonalCrossGraphicItem::setSize(double size)
{
    mSize = size;
}

auto DiagonalCrossGraphicItem::selectedPen() const -> QPen
{
    return mSelectedPen;
}

void DiagonalCrossGraphicItem::setSelectedPen(const QPen &pen)
{
    mSelectedPen = pen;
}

auto DiagonalCrossGraphicItem::boundingRect() const -> QRectF
{
    double r = mSize / 2.;
    return QRectF(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
}

auto DiagonalCrossGraphicItem::shape() const -> QPainterPath
{
    QPainterPath path;
    double r = mSize / 2.;
    path.addRect(mCenter.x() - r, mCenter.y() - r, mSize, mSize);
    return path;
}

void DiagonalCrossGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (this->isSelected()) painter->setPen(mSelectedPen);
    else painter->setPen(mPen);

    double half_size = mSize / 2.;
    double x = mCenter.x();
    double y = mCenter.y();
    painter->drawLine(QPointF(x - half_size, y - half_size), QPointF(x + half_size, y + half_size));
    painter->drawLine(QPointF(x - half_size, y + half_size), QPointF(x + half_size, y - half_size));
}


} // namespace graphos
