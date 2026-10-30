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

#pragma once

#include "config_graphos.h"

/* Qt */
#include <QObject>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QImage>

#include <memory>
#include <functional>

/* TidopLib */
#include <tidop/geometry/primitives/Point.h>
#include <tidop/math/algebra/rotations/RotationMatrix.h>
#include <tidop/math/algebra/matrix/Matrix.h>

/* Easy3D */
#ifdef HAVE_EASY3D
#include <easy3d/core/model.h>
#include <easy3d/renderer/camera.h>
#include <easy3d/renderer/eye_dome_lighting.h>
#endif

#include "graphos/framework/app/Application.h"
#include "graphos/widgets/GraphosWidget.h"
#include "graphos/widgets/viewer3d/Viewer3DContextMenu.h"

namespace easy3d
{
class TrianglesDrawable;
class LinesDrawable;
class PointsDrawable;
}

namespace graphos
{


#ifdef HAVE_EASY3D

class Easy3DViewer3D
  : public QOpenGLWidget,
    public Viewer3D
{

    Q_OBJECT

public:

    /*!
     * \brief Picking mode
     */
    //enum class PickingMode
    //{
    //    point_info,
    //    distance,
    //    angle,
    //    rect_zone,
    //    point_list,
    //    level_points
    //};

    //enum class Perspective
    //{
    //    ortho_view,
    //    centered_perspective,
    //    view_perspective
    //};

    enum class Rotation
    {
        always_show,
        show_on_move,
        hide
    };

    enum class PickingMode
    {
        none,
        pointInfo,
        distance,
        angle,
        area,
        scale
    };

public:

    explicit Easy3DViewer3D(QWidget *parent = nullptr);
    ~Easy3DViewer3D() override;

    void clear() override;
    void deleteEntity(const QString &id) override;

    // File loading is backend-specific and intentionally not part of Viewer3D.
    void loadFromFile(const QString &file,
                      const QString &parent = QString());
    void loadFromFileAsync(const QString &file,
                           std::function<void(bool)> finished);
    void loadFromFiles(const QStringList &files,
                       const QString &parent = QString());
    void addCamera(const QString &id,
                   double x,
                   double y,
                   double z,
                   const tl::RotationMatrix<double> &rotation);

    void setPickingMode(PickingMode mode);
    auto pickingMode() const -> PickingMode { return mPickingMode; }
    void applyScale(double scale);

    auto captureModel() -> QImage override;

    void update();
    void applyTransform(const tl::Matrix<double, 4, 4> &transform);

signals:

    void mouseClicked(QVector3D);
    void measurementMessage(const QString &message);

public slots:

    void deleteSelectEntity() override;

    /* Vistas por defecto */

    void setFrontView() override;
    void setBottomView() override;
    void setTopView() override;
    void setBackView() override;
    void setLeftView() override;
    void setRightView() override;
    void setIsoView() override;
    void setGlobalZoom() override;

    void enableEDL() override;
    void disableEDL() override;

    void setBackgroundColor(const QColor &color) override;
    void setPerspectiveProjection(bool enabled);
    void setPointLighting(bool enabled);
    void setPointSize(int size);
    void changePointSize();

public:

    auto isEDL() const -> bool override { return mEdlEnabled; }
    auto backGroundColor() const -> QColor override;

protected slots:

    /*!
     * \brief selecciona una entidad
     */
    void selectEntity(easy3d::Model *entity);
    //void processPickedPoint(easy3d::Model *entity,
    //                        unsigned pointIndex,
    //                        int x,
    //                        int y,
    //                        const easy3d::vec3 &point);
    void showContextMenu(const QPoint &position);

protected:

    void drawCornerAxes();
    void drawMeasurements();
    void updateMeasurementDrawables();
    auto pointUnderPixel(const QPoint &position, bool &found) const -> easy3d::vec3;

private:

    void init();
    void initSignalsAndSlots();
    void preDraw();
    void draw();
    void postDraw();
    
    void setStandardView(const easy3d::vec3 &direction,
                         const easy3d::vec3 &up);

    void addModel(easy3d::Model *entity);
    void processPickedPoint(const easy3d::vec3 &point);
    void finishAreaMeasurement();
    auto findChild(const QString& name, easy3d::Model *parent = nullptr) -> easy3d::Model*;

// QOpenGLWidget

protected:

    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

// QWidget

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif

private:

    easy3d::Model *mSelectedObject;
    easy3d::Camera *mCamera;
    std::unique_ptr<easy3d::EyeDomeLighting> mEdl;
    std::vector<easy3d::Model *> mModels;
    PickingMode mPickingMode;
    std::vector<QVector3D> mPickedPoints;
    std::vector<QVector3D> mMeasurementPoints;
    bool mMeasurementComplete;
    bool mAreaClosed;
    Viewer3DContextMenu *mContextMenu;
    QPoint mMousePressed;
    QPoint mMousePrevious;
    Qt::MouseButton mPressedButton;
    QColor mBackgroundColor;
    bool mEdlEnabled;

    easy3d::TrianglesDrawable *mDrawableAxes;
    easy3d::LinesDrawable *mDrawableManipSphere;
    easy3d::LinesDrawable *mDrawableMeasurementLines;
    easy3d::PointsDrawable *mDrawableMeasurementPoints;
    bool mShowManipSphere;

};

#endif // HAVE_EASY3D

}