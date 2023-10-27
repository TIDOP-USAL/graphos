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

#include "Viewer3d.h"

//#include "graphos/widgets/plugins/ccPluginInterface.h"
//#include "graphos/widgets/plugins/core/qEDL/include/qEDL.h"
//#include "graphos/widgets/plugins/core/qEDL/include/ccEDLFilter.h"
#include "EDLfilter/ccEDLFilter.h"

/* TidopLib */
#include <tidop/core/msg/message.h>
#include <tidop/math/math.h>

TL_DISABLE_WARNINGS

/* CloudCompare*/
#include <FileIOFilter.h>
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>
#include <ccColorScalesManager.h>
#include <ccCameraSensor.h>
#include <cc2DLabel.h>
#include <cc2DViewportLabel.h>
#include <ccGenericPrimitive.h>

/* Qt */
#include <QMouseEvent>
#include <QFileInfo>
#include <QApplication>
#include <QDir>

TL_DEFAULT_WARNINGS

#include <array>

namespace graphos
{


Viewer3DContextMenu::Viewer3DContextMenu(QWidget *parent)
  : GraphosContextMenu(parent),
    mActionGlobalZoom(new QAction(this)),
    mActionViewFront(new QAction(this)),
    mActionViewTop(new QAction(this)),
    mActionViewLeft(new QAction(this)),
    mActionViewRight(new QAction(this)),
    mActionViewBack(new QAction(this)),
    mActionViewBottom(new QAction(this)),
    mActionEDLFilter(new QAction(this))
{
    init();
    initSignalAndSlots();
}

void Viewer3DContextMenu::init()
{
    mActionGlobalZoom->setIcon(QIcon::fromTheme("zoom-extend"));
    mActionViewFront->setIcon(QIcon::fromTheme("front-view"));
    mActionViewTop->setIcon(QIcon::fromTheme("top-view"));
    mActionViewLeft->setIcon(QIcon::fromTheme("left-view"));
    mActionViewRight->setIcon(QIcon::fromTheme("right-view"));
    mActionViewBack->setIcon(QIcon::fromTheme("back-view"));
    mActionViewBottom->setIcon(QIcon::fromTheme("bottom-view"));

    mActionEDLFilter->setCheckable(true);

    addAction(mActionGlobalZoom);
    addSeparator();
    addAction(mActionViewFront);
    addAction(mActionViewTop);
    addAction(mActionViewLeft);
    addAction(mActionViewRight);
    addAction(mActionViewBack);
    addAction(mActionViewBottom);
    addSeparator();
    addAction(mActionEDLFilter);

    retranslate();
}

void Viewer3DContextMenu::initSignalAndSlots()
{
    connect(mActionGlobalZoom, &QAction::triggered, this, &Viewer3DContextMenu::globalZoom);
    connect(mActionViewFront, &QAction::triggered, this, &Viewer3DContextMenu::viewFront);
    connect(mActionViewTop, &QAction::triggered, this, &Viewer3DContextMenu::viewTop);
    connect(mActionViewLeft, &QAction::triggered, this, &Viewer3DContextMenu::viewLeft);
    connect(mActionViewRight, &QAction::triggered, this, &Viewer3DContextMenu::viewRight);
    connect(mActionViewBack, &QAction::triggered, this, &Viewer3DContextMenu::viewBack);
    connect(mActionViewBottom, &QAction::triggered, this, &Viewer3DContextMenu::viewBottom);
    connect(mActionEDLFilter, &QAction::toggled, this, &Viewer3DContextMenu::filterEDL);
}

void Viewer3DContextMenu::retranslate()
{
    mActionGlobalZoom->setText(QApplication::translate("Viewer3DContextMenu", "Global Zoom", nullptr));
    mActionViewFront->setText(QApplication::translate("Viewer3DContextMenu", "View Front", nullptr));
    mActionViewTop->setText(QApplication::translate("Viewer3DContextMenu", "View Top", nullptr));
    mActionViewLeft->setText(QApplication::translate("Viewer3DContextMenu", "View Left", nullptr));
    mActionViewRight->setText(QApplication::translate("Viewer3DContextMenu", "View Right", nullptr));
    mActionViewBack->setText(QApplication::translate("Viewer3DContextMenu", "View Back", nullptr));
    mActionViewBottom->setText(QApplication::translate("Viewer3DContextMenu", "View Bottom", nullptr));
    mActionEDLFilter->setText(QApplication::translate("Viewer3DContextMenu", "EDL Filter", nullptr));
}


/* ----------------------------------------------------------------------------------------- */


CCViewer3D::CCViewer3D(QWidget *parent)
    : ccGLWindow(nullptr),
    mSelectedObject(nullptr),
    mScaleX(1),
    mScaleY(1),
    mOrderedLabelsContainer(nullptr),
    mContextMenu(new Viewer3DContextMenu(this)),
    mShowClassification(false),
    mRGBAColors(nullptr),
    edl(false),
    filter(nullptr)
{
    init();
    initSignalsAndSlots();
}

CCViewer3D::~CCViewer3D()
{
    /// TODO: la destructora de ccGLWindow no es virtual con lo cual no se llama...
    ///       Mejor composición que herencia.
    if (ccHObject *currentRoot = this->getSceneDB()) {
        mSelectedObject = nullptr;
        this->setSceneDB(nullptr);
        delete currentRoot;
        currentRoot = nullptr;
    }

    if (mLabel) {
        delete mLabel;
        mLabel = nullptr;
    }

    if (mRect2DLabel) {
        delete mRect2DLabel;
        mRect2DLabel = nullptr;
    }

    //if (filter) {
    //  delete filter;
    //  filter = nullptr;
    //}
}

void CCViewer3D::clear()
{
    if (ccHObject *currentRoot = this->getSceneDB()) {
        mSelectedObject = nullptr;
        this->setSceneDB(nullptr);
        this->redraw();
        this->setPickingMode(ccGLWindow::DEFAULT_PICKING);
        //clearLables();
        delete currentRoot;
        currentRoot = nullptr;
    }
    init();
}

void CCViewer3D::createGroup(const QString &group, const QString &parent)
{
    if (ccHObject *currentRoot = this->getSceneDB()) {
        if (parent.isEmpty()) {
            ccHObject *new_group = new ccHObject(group);
            currentRoot->addChild(new_group);
        } else {
            ///TODO: Buscar el grupo y añadir como hijo el nuevo grupo

        }
    }
}

void CCViewer3D::deleteEntity(const QString &id)
{
    if (ccHObject *currentRoot = getSceneDB()) {

        ccHObject *temp = nullptr;

        for (uint32_t i = 0; i < currentRoot->getChildrenNumber(); i++) {
            temp = currentRoot->getChild(i);
            if (temp->getName().compare(id) == 0 /*&&
                temp->isKindOf(CC_TYPES::POINT_CLOUD)*/) {
                if (temp->getParent()) {
                    temp->getParent()->addDependency(temp, ccHObject::DP_DELETE_OTHER); //we force deletion!
                    temp->getParent()->removeChild(temp);
                } else {
                    delete temp;
                }
                break;
            }
        }

        redraw();
    }
}

void CCViewer3D::loadFromFile(const QString &file, const QString &parent)
{
    FileIOFilter::LoadParameters parameters;
    parameters.alwaysDisplayLoadDialog = false;
    parameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG_AUTO_SHIFT;
    parameters.parentWidget = this;
    CC_FILE_ERROR result = CC_FERR_NO_ERROR;

    ///TODO: esto asigna un grupo por defecto a la nube de puntos cargada...
    //if (ccHObject *newEntities = FileIOFilter::LoadFromFile(file, parameters, result)) {
    //  newEntities->setName(file);
    //  addToDB(newEntities);
    //}

    ///TODO: Por lo anterior extraigo el código necesario y lo adapto
    QString extension = QFileInfo(file).suffix();
    if (extension.isEmpty()) {
        tl::Message::error("[Load] Can't guess file format: no file extension");
        return; ///TODO: Devolver error
    }

    //convert extension to file format
    QSharedPointer<FileIOFilter> filter = FileIOFilter::FindBestFilterForExtension(extension);

    //unknown extension?
    if (!filter) {
        tl::Message::error("[Load] Can't guess file format: unhandled file extension '{}'", extension.toStdString());
        return; ///TODO: Devolver error
    }

    //check file existence
    QFileInfo fi(file);
    if (!fi.exists()) {
        tl::Message::error("[Load] File '{}' doesn't exist!", file.toStdString());
        return; ///TODO: Devolver error
    }

    ccHObject *group = nullptr;
    // Se carga el grupo si existe. Si no se añade a root
    /// TODO: Esto tiene que ser recursivo
    ccHObject *currentRoot = this->getSceneDB();
    if (currentRoot) {
        ccHObject *temp = nullptr;
        for (uint32_t i = 0; i < currentRoot->getChildrenNumber(); i++) {
            temp = currentRoot->getChild(i);
            if (temp->getName().compare(parent) == 0 &&
                temp->isKindOf(CC_TYPES::HIERARCHY_OBJECT)) {
                group = temp;
                break;
            }
        }

    }

    if (group == nullptr) {
        group = currentRoot;
    }

    //load file
    try {
        result = filter->loadFile(file,
                                  *group,
                                  parameters);
    } catch (...) {
        tl::Message::warning("[I/O] CC has caught an unhandled exception while loading file '{}'", file.toStdString());
        //if (container)
        //  container->removeAllChildren();
        return; ///TODO: Devolver error
    }

    // Disable normals
    ccHObject::Container clouds;
    group->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);
    for (auto cloud : clouds) {
        if (cloud) {
            static_cast<ccGenericPointCloud *>(cloud)->showNormals(false);
            static_cast<ccGenericPointCloud *>(cloud)->showColors(true);
            static_cast<ccGenericPointCloud *>(cloud)->showSF(false);
        }

    }

    group->setDisplay_recursive(this);

    size_t childCount = group->getChildrenNumber();
    if (childCount != 0) {
        for (size_t i = 0; i < childCount; ++i) {
            ccHObject *child = group->getChild(static_cast<unsigned int>(i));
            QString newName = child->getName();
            if (newName.startsWith("unnamed")) {
                child->setName(file);
            }
        }
    } else {
        if (group) {
            delete group;
            group = nullptr;
        }
    }

    this->setSceneDB(currentRoot);
}

void CCViewer3D::loadFromFiles(const QStringList &files, const QString &parent)
{
    FileIOFilter::LoadParameters parameters;
    parameters.alwaysDisplayLoadDialog = false;
    parameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG_AUTO_SHIFT;
    parameters.parentWidget = this;

    for (int i = 0; i < files.size(); ++i) {
        CC_FILE_ERROR result = CC_FERR_NO_ERROR;
        if (ccHObject *newEntities = FileIOFilter::LoadFromFile(files[i], parameters, result)) {
            newEntities->setName(files[i]);

            // Disable normals
            ccHObject::Container clouds;
            newEntities->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);
            for (auto cloud : clouds) {
                if (cloud) {
                    static_cast<ccGenericPointCloud *>(cloud)->showNormals(false);
                    static_cast<ccGenericPointCloud *>(cloud)->showColors(true);
                    static_cast<ccGenericPointCloud *>(cloud)->showSF(false);
                }

            }

            addToDB(newEntities);
        }

        if (result == CC_FERR_CANCELED_BY_USER) {
            //stop importing the file if the user has cancelled the current process!
            break;
        }
    }
}

void CCViewer3D::setScale(double x, double y)
{
    mScaleX = x;
    mScaleY = y;
    this->resizeGL(this->width(), this->height());
}

void CCViewer3D::addCamera(const QString &id,
                           double x,
                           double y,
                           double z,
                           const std::array<std::array<float, 3>, 3> &rot)
{
    ccCameraSensor *camera = new ccCameraSensor();

    ccHObject *currentRoot = getSceneDB();
    CCVector3d camera_center(x, y, z);
    if (currentRoot) {
        ccHObject *child = currentRoot->getFirstChild();
        ccGenericPointCloud *cloud = ccHObjectCaster::ToGenericPointCloud(child);
        bool isShifted = cloud->isShifted();
        if (isShifted) {
            CCVector3d shift = cloud->getGlobalShift();
            //msgInfo("Desplazamiento camara [%lf,%lf,%lf]", shift.x, shift.y, shift.z);
            //msgInfo("Camara cargada [%lf,%lf,%lf]", camera_center.x, camera_center.y, camera_center.z);
            camera_center += shift;
        }
        ccBBox bb = cloud->getOwnBB();
        double diag = bb.getDiagNorm();
        if (diag < 1.0)
            camera->setGraphicScale(static_cast<PointCoordinateType>(1.0e-3));
        else if (diag > 10000.0)
            camera->setGraphicScale(static_cast<PointCoordinateType>(1.0e3));

        //set position
        ccIndexedTransformation trans;
        camera->addPosition(trans, 0);

    }

    ccGLMatrix cc_rot;
    {
        float *mat = cc_rot.data();

        mat[0] = rot[0][0];
        mat[1] = rot[0][1];
        mat[2] = rot[0][2];

        mat[4] = rot[1][0];
        mat[5] = rot[1][1];
        mat[6] = rot[1][2];

        mat[8] = rot[2][0];
        mat[9] = rot[2][1];
        mat[10] = rot[2][2];
    }

    CCVector3 center(static_cast<PointCoordinateType>(camera_center.x),
                     static_cast<PointCoordinateType>(camera_center.y),
                     static_cast<PointCoordinateType>(camera_center.z));
    cc_rot.setTranslation(center);

    camera->setRigidTransformation(cc_rot);

    TL_TODO("Esto se tiene que rellenar con la calibración de la cámara")
        /*** Intrinsic parameters ***/
    {
        ccCameraSensor::IntrinsicParameters iParams;

        iParams.vertFocal_pix = 3859.63f;
        iParams.vFOV_rad = 45.f * tl::consts::deg_to_rad<float>;
        iParams.arrayWidth = 4863;
        iParams.arrayHeight = 3221;
        iParams.pixelSize_mm[0] = static_cast<float>(1 / 4863);
        iParams.pixelSize_mm[1] = static_cast<float>(1 / 3221);
        iParams.zNear_mm = 0.f;
        iParams.zFar_mm = 100.f;
        iParams.skew = 0.f;
        iParams.principal_point[0] = 2412.5f;
        iParams.principal_point[1] = 1595.53f;

        camera->setIntrinsicParameters(iParams);
    }

    mCameras[id] = camera;

    ccGLWindow *win = nullptr;
    if (currentRoot) {
        currentRoot->addChild(camera);
        win = static_cast<ccGLWindow *>(currentRoot->getDisplay());
    }

    if (win) {
        camera->setDisplay(win);
        camera->setVisible(true);
        if (currentRoot) {
            ccBBox box = currentRoot->getOwnBB();
            win->updateConstellationCenterAndZoom(&box);
        }
    }

    addToDB(camera);
}

void CCViewer3D::addPrimitive(ccGenericPrimitive *primitive)
{
    addToDB(primitive);
}

void CCViewer3D::activatePicker(PickingMode pickerMode)
{
    deactivatePicker();

    this->setPickingMode(ccGLWindow::POINT_PICKING);

    mPickingMode = pickerMode;

    this->addToOwnDB(mLabel);
    this->addToOwnDB(mRect2DLabel);

    if (pickerMode != PickingMode::point_list) {
        mLabel->setVisible(true);
        mLabel->setSelected(true);
    }

    mRect2DLabel->setVisible(false);
    mRect2DLabel->setSelected(true);
    this->redraw();

    // #if CLOUDCOMPARE_VERSION_MAJOR == 2 && CLOUDCOMPARE_VERSION_MINOR >= 12 || CLOUDCOMPARE_VERSION_MAJOR > 2
    this->setInteractionMode(ccGLWindow::MODE_TRANSFORM_CAMERA);
    // #else
    //   this->setInteractionMode(ccGLWindow::TRANSFORM_CAMERA());
    // #endif

    this->redraw(true);

    connect(this, &CCViewer3D::itemPicked,
            this, &CCViewer3D::processPickedPoint);
}

void CCViewer3D::deactivatePicker()
{
    disconnect(this, &CCViewer3D::itemPicked,
               this, &CCViewer3D::processPickedPoint);

    mLabel->clear();

    this->removeFromOwnDB(mLabel);
    this->removeFromOwnDB(mRect2DLabel);
    this->redraw();
}

ccHObject *CCViewer3D::object()
{
    if (ccHObject *currentRoot = getSceneDB()) return currentRoot;
    else return nullptr;
}

/* public slots */


void CCViewer3D::deleteSelectEntity()
{
    if (ccHObject *currentRoot = getSceneDB()) {
        ccHObject::Container toCheck;
        toCheck.push_back(currentRoot);

        while (!toCheck.empty()) {
            ccHObject *obj = toCheck.back();
            toCheck.pop_back();

            if (obj->isSelected()) {
                if (obj->getParent()) {
                    obj->getParent()->addDependency(obj, ccHObject::DP_DELETE_OTHER); //we force deletion!
                    obj->getParent()->removeChild(obj);
                } else {
                    delete obj;
                }
            } else {
                for (unsigned i = 0; i < obj->getChildrenNumber(); ++i)
                    toCheck.push_back(obj->getChild(i));
            }
        }
        redraw();
    }
}

void CCViewer3D::setFrontView()
{
    this->setView(CC_FRONT_VIEW);
}

void CCViewer3D::setBottomView()
{
    this->setView(CC_BOTTOM_VIEW);
}

void CCViewer3D::setTopView()
{
    this->setView(CC_TOP_VIEW);
}

void CCViewer3D::setBackView()
{
    this->setView(CC_BACK_VIEW);
}

void CCViewer3D::setLeftView()
{
    this->setView(CC_LEFT_VIEW);
}

void CCViewer3D::setRightView()
{
    this->setView(CC_RIGHT_VIEW);
}

void CCViewer3D::setIsoView1()
{
    this->setView(CC_ISO_VIEW_1);
}

void CCViewer3D::setIsoView2()
{
    this->setView(CC_ISO_VIEW_2);
}

void CCViewer3D::setGlobalZoom()
{
    this->zoomGlobal();
}

ccHObject *CCViewer3D::findChild(const QString &name, ccHObject *parent)
{
    if (parent == nullptr) {
        parent = getSceneDB();
    }

    ccHObject *temp = nullptr;
    for (uint32_t i = 0; i < parent->getChildrenNumber(); i++) {
        temp = parent->getChild(i);
        if (temp->getName().compare(name) == 0) {
            return temp;
        } else if (temp->getChildrenNumber() > 0) {
            temp = findChild(name, temp);
            if (temp) return temp;
        }
    }

    return nullptr;
}

void CCViewer3D::showContextMenu(const QPoint &position)
{
    QPoint globalPos = mapToGlobal(position);
    if (mMousePress == position)
        mContextMenu->exec(globalPos);
}

void CCViewer3D::mousePressEvent(QMouseEvent *event)
{
    ccGLWindow::mousePressEvent(event);

    mMousePress = event->pos();
}

void CCViewer3D::setVisible(const QString &id, bool visible)
{
    if (ccHObject *currentRoot = getSceneDB()) {

        ccHObject *temp = findChild(id);
        if (temp) {
            temp->setEnabled(!temp->isEnabled());
            temp->prepareDisplayForRefresh();
        }

        //for (uint32_t i = 0; i < currentRoot->getChildrenNumber(); i++) {
        //  temp = currentRoot->getChild(i);
        //  if (temp->getName().compare(id) == 0 /*&&
        //      temp->isKindOf(CC_TYPES::POINT_CLOUD)*/) {
        //    temp->setEnabled(!temp->isEnabled());
        //    //if (temp->isVisible() != visible) {
        //    //  temp->toggleVisibility();
        //    //  temp->prepareDisplayForRefresh();
        //    //  //temp->prepareDisplayForRefresh_recursive();
        //    //}
        //    break;
        //  }
        //}

        redraw();
    }
}

void CCViewer3D::showClassification(bool show)
{
    mShowClassification = show;

    if (mColorTable == nullptr) return;

    if (ccHObject *currentRoot = getSceneDB()) {

        //currentRoot->showColors(!show);
        //currentRoot->showSF(show);

        ccHObject::Container clouds;
        currentRoot->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

        /// Sólo se permite una nube de puntos en el visor
        if (clouds.size() != 1) return;

        if (auto cloud = static_cast<ccPointCloud *>(clouds.at(0))) {

            if (show) {

                cloud->setColor(ccColor::black);

                for (const auto &reg : *mColorTable) {

                    ScalarType code = static_cast<ScalarType>(reg.first);
                    tl::Color color = reg.second.second;

                    ccColor::Rgba ccColor(color.red(), color.green(), color.blue(), color.opacity());

                    if (CCCoreLib::ScalarField *sf = cloud->getScalarField(0)) {

                        int counter = 0;
                        for (auto it = sf->begin(); it != sf->end(); ++it, ++counter) {
                            if ((*it) == code) {
                                cloud->setPointColor(counter, ccColor);
                            }
                        }

                    }

                }

            } else {

                if (mRGBAColors) {
                    /// Parece que se cambia en la nube de puntos pero no se visualiza.
                    //if (auto rgba = cloud->rgbaColors()) {
                    //  // restore original colors
                    //  mRGBAColors->copy(*rgba);
                    //}
                    int counter = 0;
                    for (const auto &color : *mRGBAColors) {

                        ccColor::Rgba color_reg(color);

                        for (const auto &reg : *mColorTable) {
                            ScalarType code = static_cast<ScalarType>(reg.first);

                            if (CCCoreLib::ScalarField *sf = cloud->getScalarField(0)) {
                                if (sf->at(counter) == code) {
                                    color_reg.a = reg.second.second.opacity();
                                    break;
                                }
                            }
                        }

                        cloud->setPointColor(counter++, color_reg);
                    }

                } else {
                    cloud->unallocateColors();
                }

            }

            cloud->redrawDisplay();

        }
    }

}

void CCViewer3D::setColorTable(std::shared_ptr<ColorTable> colorTable)
{
    mColorTable = colorTable;

    connect(mColorTable.get(), &ColorTable::change, [&]() {
        showClassification(mShowClassification);
            });

    if (ccHObject *currentRoot = getSceneDB()) {

        ccHObject::Container clouds;
        currentRoot->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

        /// Sólo se permite una nube de puntos en el visor
        if (clouds.size() != 1) return;

        if (auto cloud = static_cast<ccPointCloud *>(clouds.at(0))) {

            if (cloud->hasColors()) {

                if (mRGBAColors == nullptr) {
                    mRGBAColors = cloud->rgbaColors()->clone();
                }

                if (!mRGBAColors) {
                    tl::Message::error("Not enough memory to backup previous colors");
                }

            } else {

                // check memory for rgb colors
                if (!cloud->resizeTheRGBTable()) {
                    tl::Message::error("Not enough memory to show colors");
                }
            }

        }

    }
}

/* protected */

//void CCViewer3D::paintGL()
//{
//  ccGLWindow::paintGL();
//}
//
//void CCViewer3D::resizeGL(int width, int height)
//{
//  ccGLWindow::resizeGL(width, height);
//}
//
//void CCViewer3D::initializeGL()
//{
//  ccGLWindow::initializeGL();
//}
//
//void CCViewer3D::mouseMoveEvent(QMouseEvent* event)
//{
//  ccGLWindow::mouseMoveEvent(event);
//}
//
//void CCViewer3D::mousePressEvent(QMouseEvent* event)
//{
//  ccGLWindow::mousePressEvent(event);
//}
//
//void CCViewer3D::mouseReleaseEvent(QMouseEvent* event)
//{
//  ccGLWindow::mouseReleaseEvent(event);
//}
//
//void CCViewer3D::wheelEvent(QWheelEvent* event)
//{
//  ccGLWindow::wheelEvent(event);
//}
//
//bool CCViewer3D::event(QEvent* event)
//{
//  ccGLWindow::event(event);
//}

/* protected slots */

void CCViewer3D::selectEntity(ccHObject *entity)
{
    bool bSelect = false;
    if (ccHObject *currentRoot = this->getSceneDB()) {
        currentRoot->setSelected_recursive(false);

        if (entity) {
            bSelect = true;
            entity->setSelected(true);
            mSelectedObject = entity;
        }

        redraw();

    }
}

/* private */

void CCViewer3D::processPickedPoint(ccHObject *entity,
                                    unsigned pointIndex,
                                    int x,
                                    int y,
                                    const CCVector3 &point,
                                    const CCVector3d &uvw)
{
    if (!entity)
        return;

    switch (mPickingMode) {
    case PickingMode::point_info:
        mLabel->clear();
        break;
    case PickingMode::distance:
        if (mLabel->size() >= 2)
            mLabel->clear();
        break;
    case PickingMode::angle:
        if (mLabel->size() >= 3)
            mLabel->clear();
        break;
    case PickingMode::level_points:
        if (mLabel->size() >= 3)
            mLabel->clear();
        break;
    case PickingMode::rect_zone:
        return; //we don't use this slot for 2D mode
    }

    CCVector3d P;

    if (entity->isKindOf(CC_TYPES::POINT_CLOUD)) {
        ccPointCloud *cloud = static_cast<ccPointCloud *>(entity);
        if (!cloud) {
            return;
        }
        P = cloud->toGlobal3d(point/**cloud->getPoint(pointIndex)*/);
        //#if CLOUDCOMPARE_VERSION_MAJOR == 2 && CLOUDCOMPARE_VERSION_MINOR >= 11 || CLOUDCOMPARE_VERSION_MAJOR > 3
        if (mPickingMode != PickingMode::level_points) 
            mLabel->addPickedPoint(cloud, pointIndex);
        // #else
        //     mLabel->addPoint(cloud, pointIndex);
        // #endif
    } else if (entity->isKindOf(CC_TYPES::MESH)) {

        auto mesh = static_cast<ccGenericMesh *>(entity);
        if (!mesh) {
            return;
        }

        if (mPickingMode != PickingMode::level_points)
            mLabel->addPickedPoint(mesh, pointIndex, CCVector2d(uvw.x, uvw.y));
        //CCVector3 point;
        //mesh->computePointPosition(pointIndex, CCVector2d(uvw.x, uvw.y), point);
        P = mesh->toGlobal3d(point);
    } else {

        return;
    }

    emit mouseClicked(QVector3D(P.x, P.y, P.z));

    mLabel->setVisible(true);
    if (mPickingMode != PickingMode::level_points)
        mLabel->displayPointLegend(mLabel->size() == 3); //we need to display 'A', 'B' and 'C' for 3-points labels
    if (mLabel->size() == 1) {
        mLabel->setPosition(static_cast<float>(x + 20) / this->glWidth(), static_cast<float>(y + 20) / this->glHeight());
    }

    this->redraw();
}

void CCViewer3D::init()
{
    /* Inicialización de CloudCompare */
    FileIOFilter::InitInternalFilters(); //load all known I/O filters (plugins will come later!)
    //ccNormalVectors::GetUniqueInstance(); //force pre-computed normals array initialization
    ccColorScalesManager::GetUniqueInstance(); //force pre-computed color tables initialization

    ccHObject *currentRoot = this->getSceneDB();
    if (currentRoot == nullptr) {
        currentRoot = new ccHObject("root");
        currentRoot->showNormals(false);
        this->setSceneDB(currentRoot);
    }

    this->setRectangularPickingAllowed(false);

    //for points picking
    mLabel = new cc2DLabel();
    mLabel->setSelected(true);

    //for 2D zone picking
    mRect2DLabel = new cc2DViewportLabel();
    mRect2DLabel->setVisible(false);	//=invalid
    mRect2DLabel->setSelected(true);	//=closed

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void CCViewer3D::initSignalsAndSlots()
{
    //connect(this, SIGNAL(filesDropped(QStringList)), this, SLOT(addToDB(QStringList)));
    connect(this, SIGNAL(entitySelectionChanged(ccHObject *)), this, SLOT(selectEntity(ccHObject *)));

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

    connect(mContextMenu, &Viewer3DContextMenu::globalZoom, this, &CCViewer3D::setGlobalZoom);
    connect(mContextMenu, &Viewer3DContextMenu::viewFront, this, &CCViewer3D::setFrontView);
    connect(mContextMenu, &Viewer3DContextMenu::viewTop, this, &CCViewer3D::setTopView);
    connect(mContextMenu, &Viewer3DContextMenu::viewLeft, this, &CCViewer3D::setLeftView);
    connect(mContextMenu, &Viewer3DContextMenu::viewRight, this, &CCViewer3D::setRightView);
    connect(mContextMenu, &Viewer3DContextMenu::viewBack, this, &CCViewer3D::setBackView);
    connect(mContextMenu, &Viewer3DContextMenu::viewBottom, this, &CCViewer3D::setBottomView);
    connect(mContextMenu, &Viewer3DContextMenu::filterEDL, [&](bool active) {
        if (active) enableEDL();
        else disableEDL();
            });

}

void CCViewer3D::addToDB(ccHObject *entity)
{
    assert(entity && this);

    if (ccHObject *currentRoot = this->getSceneDB()) {
        //already a pure 'root'
        if (currentRoot->isA(CC_TYPES::HIERARCHY_OBJECT)) {
            currentRoot->addChild(entity);
            this->setSceneDB(currentRoot);
            entity->setDisplay_recursive(this);
        }
    }

}

//void CCViewer3D::scaleAlreadyDisplayed(ccHObject *entity)
//{
//  if (!scaleAlreadyDisplayed) {
//    for (unsigned i = 0; i < entity->getChildrenNumber(); ++i) {
//      ccHObject* ent = entity->getChild(i);
//      if (ent->isA(CC_TYPES::POINT_CLOUD)) {
//        ccPointCloud* pc = static_cast<ccPointCloud*>(ent);
//        if (pc->hasScalarFields()) {
//          pc->setCurrentDisplayedScalarField(0);
//          pc->showSFColorsScale(true);
//          scaleAlreadyDisplayed = true;
//        }
//      } else if (ent->isKindOf(CC_TYPES::MESH)) {
//        ccGenericMesh* mesh = static_cast<ccGenericMesh*>(ent);
//        if (mesh->hasScalarFields()) {
//          mesh->showSF(true);
//          scaleAlreadyDisplayed = true;
//          ccPointCloud* pc = static_cast<ccPointCloud*>(mesh->getAssociatedCloud());
//          pc->showSFColorsScale(true);
//        }
//      }
//    }
//  }
//}

void CCViewer3D::enableEDL()
{
    filter = new ccEDLFilter;
    QString error;

    filter->init(this->width(), this->height(), qApp->applicationDirPath() + "/shaders", error);
    setGlFilter(filter);
    this->edl = true;
}

void CCViewer3D::disableEDL()
{
    setGlFilter(nullptr);

    redraw();
    edl = false;
}


} // End graphos namespace
