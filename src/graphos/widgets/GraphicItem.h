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

#ifndef GRAPHOS_GRAPHIC_ITEM_H
#define GRAPHOS_GRAPHIC_ITEM_H

#include <QGraphicsItem>
#include <QPen>

namespace graphos
{

class GraphicItem
  : public QGraphicsItem
{

public:

  GraphicItem(QGraphicsItem *parent = nullptr) : QGraphicsItem(parent){}
  ~GraphicItem() override = default;

  virtual auto pen() const -> QPen = 0;
  virtual void setPen(const QPen &pen) = 0;

  virtual auto size() -> double = 0;
  virtual void setSize(double size) = 0;

  virtual auto selectedPen() const -> QPen = 0;
  virtual void setSelectedPen(const QPen &pen) = 0;
};


class CircleGraphicItem
  : public GraphicItem
{

public:

  CircleGraphicItem(QGraphicsItem *parent = nullptr);
  CircleGraphicItem(const QPointF &center, double size, QGraphicsItem *parent = nullptr);
  ~CircleGraphicItem() override;

// GraphicItem interface

public:

  auto pen() const -> QPen override;
  void setPen(const QPen &pen) override;
  auto size() -> double override;
  void setSize(double size) override;
  auto selectedPen() const -> QPen override;
  void setSelectedPen(const QPen &pen) override;

// QGraphicsItem interface

public:

  auto boundingRect() const -> QRectF override;
  auto shape() const -> QPainterPath override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

  QPointF mCenter;
  QPen mPen;
  double mSize;
  QPen mSelectedPen;
};



class CrossGraphicItem
  : public GraphicItem
{

public:

  CrossGraphicItem(QGraphicsItem *parent = nullptr);
  CrossGraphicItem(const QPointF &center, double size, QGraphicsItem *parent = nullptr);
  ~CrossGraphicItem() override;

// GraphicItem interface

public:

  auto pen() const -> QPen override;
  void setPen(const QPen &pen) override;
  auto size() -> double override;
  void setSize(double size) override;
  auto selectedPen() const -> QPen override;
  void setSelectedPen(const QPen &pen) override;

// QGraphicsItem interface

public:

  auto boundingRect() const -> QRectF override;
  auto shape() const -> QPainterPath override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

  QPointF mCenter;
  QPen mPen;
  double mSize;
  QPen mSelectedPen;

};




class KeyPointGraphicsItem
  : public GraphicItem
{

public:

  KeyPointGraphicsItem(QGraphicsItem *parent = nullptr);
  KeyPointGraphicsItem(QPointF center, double size, double angle, QGraphicsItem *parent = nullptr);
  ~KeyPointGraphicsItem() override;

  auto angle() const -> double;
  void setAngle(double angle);

// GraphicItem interface

public:

  auto pen() const -> QPen override;
  void setPen(const QPen &pen) override;
  auto size() -> double override;
  void setSize(double size) override;
  auto selectedPen() const -> QPen override;
  void setSelectedPen(const QPen &pen) override;

// QGraphicsItem interface

public:

  auto boundingRect() const -> QRectF override;
  auto shape() const -> QPainterPath override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

  QPointF mCenter;
  QPen mPen;
  double mSize;
  double mAngle;
  QPen mSelectedPen;
};



class DiagonalCrossGraphicItem
  : public GraphicItem
{

public:

  DiagonalCrossGraphicItem(QGraphicsItem *parent = nullptr);
  DiagonalCrossGraphicItem(const QPointF &center, double size, QGraphicsItem *parent = nullptr);
  ~DiagonalCrossGraphicItem() override;

// GraphicItem interface

public:

  auto pen() const -> QPen override;
  void setPen(const QPen &pen) override;
  auto size() -> double override;
  void setSize(double size) override;
  auto selectedPen() const -> QPen override;
  void setSelectedPen(const QPen &pen) override;

// QGraphicsItem interface

public:

  auto boundingRect() const -> QRectF override;
  auto shape() const -> QPainterPath override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

  QPointF mCenter;
  QPen mPen;
  double mSize;
  QPen mSelectedPen;
};


} // namespace graphos

#endif // GRAPHOS_GRAPHIC_ITEM_H
