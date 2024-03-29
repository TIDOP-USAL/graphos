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

#ifndef GRAPHOS_VIEWER_3D_H
#define GRAPHOS_VIEWER_3D_H

#include "config_graphos.h"

#include <memory>

/* Qt */
#include <QObject>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QImage>

#include <tidop/core/defs.h>
#include <tidop/graphic/color.h>

/* CloudCompare */
TL_DISABLE_WARNINGS
#include "ccGLWindow.h"
#include <ccCameraSensor.h>
TL_DEFAULT_WARNINGS

#include "graphos/core/ColorTable.h"
#include "graphos/core/Application.h"
#include "graphos/widgets/GraphosWidget.h"

class cc2DLabel;
class cc2DViewportLabel;
class RGBAColorsTableType;
class ccGlFilter;


namespace graphos
{


class Viewer3DContextMenu
  : public GraphosContextMenu
{
    Q_OBJECT

public:

    Viewer3DContextMenu(QWidget *parent = nullptr);
    ~Viewer3DContextMenu() override = default;

signals:

    void globalZoom();
    void viewFront();
    void viewTop();
    void viewLeft();
    void viewRight();
    void viewBack();
    void viewBottom();
    void filterEDL(bool);

private:

    void init();
    void initSignalAndSlots();

// GraphosContextMenu

private slots:

    void retranslate() override;

private:

    QAction *mActionGlobalZoom;
    QAction *mActionViewFront;
    QAction *mActionViewTop;
    QAction *mActionViewLeft;
    QAction *mActionViewRight;
    QAction *mActionViewBack;
    QAction *mActionViewBottom;
    QAction *mActionEDLFilter;

};


/* Visor basado en CloudCompare */

class CCViewer3D
  : public ccGLWindow,
    public Viewer3D
{

    Q_OBJECT

public:

    /*!
     * \brief Picking mode
     */
    enum class PickingMode
    {
        point_info,
        distance,
        angle,
        rect_zone,
        point_list,
        level_points
    };

    enum class Perspective
    {
        ortho_view,
        centered_perspective,
        view_perspective
    };

    enum class Rotation
    {
        always_show,
        show_on_move,
        hide
    };

public:

    explicit CCViewer3D(QWidget *parent = nullptr);
    ~CCViewer3D() override;

    void clear() override;
    void createGroup(const QString &group,
                     const QString &parent = QString()) override;
    void deleteEntity(const QString &id) override;

    void loadFromFile(const QString &file,
                      const QString &parent = QString()) override;
    void loadFromFiles(const QStringList &files,
                       const QString &parent = QString()) override;

    void setScale(double x, double y);

    void addCamera(const QString &id,
                   double x,
                   double y,
                   double z,
                   const std::array<std::array<float, 3>, 3> &rot) override;
    void drawLine(const std::string &name, 
                  const tl::Point3<double> &point1, 
                  const tl::Point3<double> &point2, 
                  const tl::Color &color, int width) override;
    void addPrimitive(ccGenericPrimitive *primitive) override;

    void activatePicker(PickingMode pickerMode);
    void deactivatePicker();

    auto object() -> ccHObject*;
    auto captureModel() -> QImage override;

signals:

    void mouseClicked(QVector3D);

public slots:

    void deleteSelectEntity() override;

    /* Vistas por defecto */

    void setFrontView() override;
    void setBottomView() override;
    void setTopView() override;
    void setBackView() override;
    void setLeftView() override;
    void setRightView() override;
    void setIsoView1() override;
    void setIsoView2() override;
    void setGlobalZoom() override;

    void setVisible(const QString &id, bool visible) override;
    void showClassification(bool show = true) override;
    void setColorTable(std::shared_ptr<ColorTable> colorTable) override;
    void enableEDL() override;
    void disableEDL() override;

public:

    bool isEDL() const override { return edl; }

protected slots:

    /*!
     * \brief selecciona una entidad
     */
    void selectEntity(ccHObject *entity);
    void processPickedPoint(ccHObject *entity,
                            unsigned pointIndex,
                            int x,
                            int y,
                            const CCVector3 &point,
                            const CCVector3d &uvw);
    void showContextMenu(const QPoint &position);
    void mousePressEvent(QMouseEvent *event) override;

private:

    void init();
    void initSignalsAndSlots();

    void addToDB(ccHObject *entity);

    /*!
     * \brief Función para buscar un objeto mediante su nombre
     */
    auto findChild(const QString& name, ccHObject* parent = nullptr) -> ccHObject*;

private:

    ccHObject *mSelectedObject;
    double mScaleX;
    double mScaleY;
    std::map<QString, ccCameraSensor *> mCameras;
    ccHObject *mOrderedLabelsContainer;
    PickingMode mPickingMode;
    cc2DLabel *mLabel;
    cc2DViewportLabel *mRect2DLabel;
    Viewer3DContextMenu *mContextMenu;
    QPoint mMousePress;
    RGBAColorsTableType *mRGBAColors;
    std::shared_ptr<ColorTable> mColorTable;
    bool mShowClassification;
    bool edl;
    ccGlFilter *filter;
};


}

#endif // GRAPHOS_VIEWER_3D_H
