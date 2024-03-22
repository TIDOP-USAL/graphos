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

#ifndef GRAPHOS_GRAPHIC_VIEWER_H
#define GRAPHOS_GRAPHIC_VIEWER_H

#include <QGraphicsView>
#include <QGraphicsEllipseItem>

// Default zoom factors
#define DEFAULT_ZOOM_FACTOR 1.15
#define DEFAULT_ZOOM_CTRL_FACTOR 1.01

namespace graphos
{

class ImageContextMenu;



/*!
 * \brief The GraphicViewer class is a subclass of QGraphicsView and provides
 * functionality for displaying and interacting with graphics, particularly images.
 */
class GraphicViewer
  : public QGraphicsView
{
    Q_OBJECT

public:

    /*!
     * \brief Constructs a GraphicViewer object.
     * \param parent The parent widget.
     */
    GraphicViewer(QWidget *parent = nullptr);

    /*!
     * \brief Destroys the GraphicViewer object.
     */
    ~GraphicViewer() override;

    /*!
     * \brief Sets the image to be displayed.
     * \param[in] image The QImage to be displayed.
     */
    virtual void setImage(const QImage &image);

    /*!
      * \brief Sets the image from raw data.
      * \param data Raw image data (RGBRGBRGBRGB ...).
      * \param[in] width Width of the image.
      * \param[in] height Height of the image.
      */
    virtual void setImageFromRawData(const uchar *data, int width, int height);

    /*!
     * \brief Sets the zoom factor when CTRL key is not pressed.
     * \param[in] factor Zoom factor (>1).
     */
    virtual void setZoomFactor(double factor);


    /*!
     * \brief Sets the zoom factor when CTRL key is pressed.
     * \param[in] factor Zoom factor (>1).
     */
    virtual void setZoomCtrlFactor(double factor);

protected:

    /*!
     * \brief Draws on the image.
     * \param[in] painter The QPainter object used for drawing.
     * \param[in] imageSize Size of the image.
     */
    virtual void drawOnImage(QPainter *painter, QSize imageSize);

    /*!
     * \brief Draws in the viewport.
     * \param[in] painter The QPainter object used for drawing.
     * \param[in] portSize Size of the viewport.
     */
    virtual void drawInViewPort(QPainter *painter, QSize portSize);

    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

#ifndef QT_NO_WHEELEVENT

    void wheelEvent(QWheelEvent *event) override;

#endif

    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

public slots:

    /*!
     * \brief Zooms to fit the viewport.
     */
    virtual void zoomExtend();

    /*!
     * \brief Zooms to a 1:1 scale.
     */
    virtual void zoom11();

    /*!
     * \brief Zooms in.
     * \param point The point to zoom in (default is QPoint()).
     */
    virtual void zoomIn(QPoint point = QPoint());

    /*!
     * \brief Zooms out.
     * \param point The point to zoom out (default is QPoint()).
     */
    virtual void zoomOut(QPoint point = QPoint());

protected slots:

    /*!
     * \brief Displays the context menu (on right-click).
     * \param position Position of the mouse in the widget.
     */
    virtual void showContextMenu(const QPoint &position);

signals:

    void mousePosition(QPoint);
    void mouseScenePosition(QPointF);
    void mouseClicked(QPoint);
    void mouseClicked(QPointF);
    void mousePressed(QPointF);
    void mouseReleased(QPointF);
    void selectionChanged();
    void removeSelectItems();

private:

    void init();
    void initSignalsAndSlots();

protected:

    QGraphicsScene *mScene;
    QGraphicsPixmapItem *mPixmapItem;
    QSize mImageSize;
    double mZoomFactor;
    double mZoomCtrlFactor;
    QPixmap mPixmap;
    QPoint mPointOld;
    ImageContextMenu *mContextMenu;

};


} // namespace graphos

#endif // GRAPHOS_GRAPHIC_VIEWER_H
