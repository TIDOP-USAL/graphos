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

#include "graphos/widgets/GraphicViewer.h"

#include "graphos/widgets/ImageMenu.h"

TL_DISABLE_WARNINGS
#include <QWheelEvent>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QApplication>
#include <QMenu>
TL_DEFAULT_WARNINGS

namespace graphos
{

GraphicViewer::GraphicViewer(QWidget *parent)
  : QGraphicsView(parent),
    mScene(new QGraphicsScene()),
    mPixmapItem(nullptr),
    mZoomFactor(DEFAULT_ZOOM_FACTOR),
    mZoomCtrlFactor(DEFAULT_ZOOM_CTRL_FACTOR),
    mContextMenu(new ImageContextMenu(this))
{
    init();
    initSignalsAndSlots();
}

GraphicViewer::~GraphicViewer()
{
    if (mPixmapItem) {
        delete mPixmapItem;
        mPixmapItem = nullptr;
    }

    if (mScene) {
        delete mScene;
        mScene = nullptr;
    }
}

void GraphicViewer::init()
{
    // Permitir el seguimiento del mouse incluso si no se presiona ningún botón
    this->setMouseTracking(true);

    // Añade la escena al QGraphicsView
    this->setScene(mScene);

    this->setBackgroundBrush(QBrush(QColor(220, 220, 220), Qt::SolidPattern));

    // Actualiza toda la ventana de observación cuando sea necesario, de lo contrario, el drawInViewPort puede experimentar problemas
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Al hacer zoom, la vista permanece centrada sobre el mouse
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // Add the default pixmap at startup
    mPixmapItem = mScene->addPixmap(mPixmap);

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void GraphicViewer::initSignalsAndSlots()
{
    connect(mScene, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

    connect(mContextMenu, SIGNAL(zoomIn()), this, SLOT(zoomIn()));
    connect(mContextMenu, SIGNAL(zoomOut()), this, SLOT(zoomOut()));
    connect(mContextMenu, &ImageContextMenu::zoomExtend, this, &GraphicViewer::zoomExtend);
    connect(mContextMenu, &ImageContextMenu::zoom11, this, &GraphicViewer::zoom11);
}

void GraphicViewer::setImage(const QImage &image)
{
    // Update the pixmap in the scene
    if (image.isGrayscale()) {
        mPixmap = QPixmap::fromImage(image.convertToFormat(QImage::Format::Format_RGB888));
    } else {
        mPixmap = QPixmap::fromImage(image);
    }

    mPixmapItem->setPixmap(mPixmap);

    mScene->setSceneRect(QRect(QPoint(0, 0), image.size()));

    mImageSize = image.size();
}

void GraphicViewer::setImageFromRawData(const uchar *data, int width, int height)
{
    QImage image(data, width, height, width * 3, QImage::Format_RGB888);
    setImage(image);
}

void GraphicViewer::setZoomFactor(double factor)
{
    mZoomFactor = factor;
}

void GraphicViewer::setZoomCtrlFactor(double factor)
{
    mZoomCtrlFactor = factor;
}

void GraphicViewer::drawOnImage(QPainter *painter, QSize imageSize)
{

}

void GraphicViewer::drawInViewPort(QPainter *painter, QSize portSize)
{

}

void GraphicViewer::drawForeground(QPainter *painter, const QRectF &rect)
{
    //  // Call the function to draw over the image
    //  drawOnImage(painter, mImageSize);

    //  // Reset transformation and call the function draw in the view port
    //  painter->resetTransform();

    //  // Call the function to draw in the view port
    //  drawInViewPort(painter, viewport()->size());
}

void GraphicViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        setDragMode(QGraphicsView::ScrollHandDrag);

    mPointOld = event->pos();

    QPointF image_point = mapToScene(QPoint(event->x(), event->y()));
    emit mousePressed(image_point);   

    QGraphicsView::mousePressEvent(event);
}

void GraphicViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        if (this->dragMode() == QGraphicsView::ScrollHandDrag) {
            setDragMode(QGraphicsView::NoDrag);
        }

        QPointF image_point = mapToScene(QPoint(event->x(), event->y()));
        if (event->pos() == mPointOld) {
            emit mouseClicked(image_point);
        } else {
            emit mouseReleased(image_point);
        }

    }

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
    //QPoint pt(event->x(), event->y());
    //QGraphicsItem *item = this->itemAt(pt);
    //QGraphicsEllipseItem *ell = qgraphicsitem_cast<QGraphicsEllipseItem *>(item);
    //if (ell){
    //  QBrush br(Qt::SolidPattern);
    //  br.setColor(Qt::black);
    //  ell->setBrush(br);
    //  ell->update();
    //}

    QGraphicsView::mouseDoubleClickEvent(event);
}

#ifndef QT_NO_WHEELEVENT

void GraphicViewer::wheelEvent(QWheelEvent *event)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    double factor = (event->modifiers() & Qt::ControlModifier) ? mZoomCtrlFactor : mZoomFactor;
    if (event->angleDelta().y() > 0)
        // Zoom +
        scale(factor, factor);
    else {
        // Zoom -
        factor = 1.0 / factor;
        scale(factor, factor);
    }

    event->accept();
}

#endif

void GraphicViewer::mouseMoveEvent(QMouseEvent *event)
{

    QPointF image_point = mapToScene(QPoint(event->x(), event->y()));
    emit mouseScenePosition(image_point);

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicViewer::resizeEvent(QResizeEvent *event)
{
    if (event->oldSize().width() == -1 || event->oldSize().height() == -1) return;

    // Get the previous rectangle of the scene in the viewport
    QPointF p1 = mapToScene(QPoint(0, 0));
    QPointF p2 = mapToScene(QPoint(event->oldSize().width(), event->oldSize().height()));

    // Stretch the rectangle around the scene
    if (p1.x() < 0) p1.setX(0);
    if (p1.y() < 0) p1.setY(0);
    if (p2.x() > mScene->width()) p2.setX(mScene->width());
    if (p2.y() > mScene->height()) p2.setY(mScene->height());

    // Fit the previous area in the scene
    this->fitInView(QRect(p1.toPoint(), p2.toPoint()), Qt::KeepAspectRatio);
}

void GraphicViewer::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch (key) {
        case Qt::Key_Delete:
        {
            emit removeSelectItems();
            break;
        }
    }
}

void GraphicViewer::zoomExtend()
{
    Qt::ScrollBarPolicy	current_horizontal_policy = horizontalScrollBarPolicy();
    Qt::ScrollBarPolicy	currentvertical_policy = verticalScrollBarPolicy();

    // Disable scroll bar to avoid a margin around the image
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    fitInView(mScene->sceneRect(), Qt::KeepAspectRatio);

    // Restaure scroll bar policy
    setHorizontalScrollBarPolicy(current_horizontal_policy);
    setVerticalScrollBarPolicy(currentvertical_policy);
}

void GraphicViewer::zoom11()
{
    double scale11 = 1. / transform().m11();
    scale(scale11, scale11);
}

void GraphicViewer::zoomIn(QPoint point)
{
    scale(mZoomFactor, mZoomFactor);
}

void GraphicViewer::zoomOut(QPoint point)
{
    double factor = 1.0 / mZoomFactor;
    scale(factor, factor);
}

void GraphicViewer::showContextMenu(const QPoint &position)
{
    QPoint global_pos = mapToGlobal(position);
    mContextMenu->exec(global_pos);
}

} // namespace graphos
