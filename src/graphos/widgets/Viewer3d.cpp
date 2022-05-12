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

/* TidopLib */
#include <tidop/core/messages.h>

/* CloudCompare*/
#include <FileIOFilter.h>
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>
#include <ccColorScalesManager.h>

/* Qt */
#include <QMouseEvent>
#include <QFileInfo>

#include <array>

namespace graphos
{

/* ----------------------------------------------------------------------------------------- */

CCViewer3D::CCViewer3D(QWidget *parent) 
  : ccGLWindow(nullptr),
    mSelectedObject(nullptr),
    mScaleX(1),
    mScaleY(1)
{
  init();

  connect(this,	SIGNAL(filesDropped(QStringList)),          this, SLOT(addToDB(QStringList)));
  connect(this,	SIGNAL(entitySelectionChanged(ccHObject*)), this, SLOT(selectEntity(ccHObject*)));

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
    msgError("[Load] Can't guess file format: no file extension");
    return; ///TODO: Devolver error
	}

  //convert extension to file format
  QSharedPointer<FileIOFilter> filter = FileIOFilter::FindBestFilterForExtension(extension);

  //unknown extension?
  if (!filter) {
    msgError("[Load] Can't guess file format: unhandled file extension '%s'", extension.toStdString().c_str());
    return; ///TODO: Devolver error
  }

	//check file existence
  QFileInfo fi(file);
  if (!fi.exists()) {
    msgError("[Load] File '%s' doesn't exist!", file.toStdString().c_str());
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
    msgWarning("[I/O] CC has caught an unhandled exception while loading file '%s'", file.toStdString().c_str());
    //if (container)
    //  container->removeAllChildren();
    return; ///TODO: Devolver error
  }

  group->setDisplay_recursive(this);

  size_t childCount = group->getChildrenNumber();
  if (childCount != 0) {
    for (size_t i = 0; i < childCount; ++i) {
      ccHObject* child = group->getChild(static_cast<unsigned int>(i));
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

void CCViewer3D::addCamera(const QString &id, double x, double y, double z, const std::array<std::array<float, 3>, 3> &rot)
{
  ccCameraSensor *camera = new ccCameraSensor();

  ccHObject *currentRoot = getSceneDB();
  CCVector3d camera_center(x, y, z);
  if (currentRoot) {
    ccHObject *child = currentRoot->getFirstChild();
    ccGenericPointCloud* cloud = ccHObjectCaster::ToGenericPointCloud(child);
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
  
  	iParams.vertFocal_pix = static_cast<float>(3859.63);
  	iParams.vFOV_rad = static_cast<float>(45 * CC_DEG_TO_RAD);
  	iParams.arrayWidth = 4863;
  	iParams.arrayHeight = 3221;
  	iParams.pixelSize_mm[0] = static_cast<float>(1 / 4863);
  	iParams.pixelSize_mm[1] = static_cast<float>(1 / 3221);
    iParams.zNear_mm = 0.f;
    iParams.zFar_mm = 100.f;
  	iParams.skew = 0.f;
  	iParams.principal_point[0] = static_cast<float>(2412.5);
  	iParams.principal_point[1] = static_cast<float>(1595.53);
  
    camera->setIntrinsicParameters(iParams);
  }

  mCameras[id] = camera;

  ccGLWindow* win = nullptr;
  if (currentRoot) {
    currentRoot->addChild(camera);
    win = static_cast<ccGLWindow*>(currentRoot->getDisplay());
  }

  if (win)
  {
    camera->setDisplay(win);
    camera->setVisible(true);
    if (currentRoot)
    {
      ccBBox box = currentRoot->getOwnBB();
      win->updateConstellationCenterAndZoom(&box);
    }
  }

  addToDB(camera);
}

/* public slots */


void CCViewer3D::deleteSelectEntity()
{
  if (ccHObject *currentRoot = getSceneDB()) {
    ccHObject::Container toCheck;
    toCheck.push_back(currentRoot);

    while (!toCheck.empty()) {
      ccHObject* obj = toCheck.back();
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

void CCViewer3D::init()
{
  /* Inicialización de CloudCompare */
  FileIOFilter::InitInternalFilters(); //load all known I/O filters (plugins will come later!)
  //ccNormalVectors::GetUniqueInstance(); //force pre-computed normals array initialization
  ccColorScalesManager::GetUniqueInstance(); //force pre-computed color tables initialization

  ccHObject* currentRoot = this->getSceneDB();
  if (currentRoot == nullptr) {
    currentRoot = new ccHObject("root");
    currentRoot->showNormals(false);
    this->setSceneDB(currentRoot);
  }
}

void CCViewer3D::addToDB(ccHObject *entity)
{
  assert(entity && this);

  ///TODO: Esto probablemente deberia estar en otra parte
  ccHObject* currentRoot = this->getSceneDB();
  //if (currentRoot == nullptr) {
  //  currentRoot = new ccHObject("root");
  //  this->setSceneDB(currentRoot);
  //}

  

  if (currentRoot) {
    //already a pure 'root'
    if (currentRoot->isA(CC_TYPES::HIERARCHY_OBJECT)) {
      currentRoot->addChild(entity);
      this->setSceneDB(currentRoot);
      entity->setDisplay_recursive(this);
    } /*else {
      ccHObject* root = new ccHObject("root");
      root->addChild(currentRoot);
      root->addChild(entity);
      this->setSceneDB(root);
    }*/
  } /*else {
    this->setSceneDB(entity);
  }*/

  //checkForLoadedEntities();
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

} // End graphos namespace
