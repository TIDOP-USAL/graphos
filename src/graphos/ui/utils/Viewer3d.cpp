#include "Viewer3d.h"

/* TidopLib */
#include <tidop/core/messages.h>

/* OpenSceneGraph */
#ifdef HAVE_OPENSCENEGRAPH
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Geometry>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osg/LightSource>
#endif // HAVE_OPENSCENEGRAPH

/* CloudCompare*/
#ifdef HAVE_CLOUDCOMPARE
#include <FileIOFilter.h>
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>
#include <ccColorScalesManager.h>
#endif // HAVE_CLOUDCOMPARE

#ifdef HAVE_PCL
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#endif

/* Qt */
#include <QMouseEvent>
#include <QFileInfo>

#if (__cplusplus >= 201703L)
#include <filesystem>
namespace fs = std::filesystem;
#elif defined HAVE_BOOST
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif

#include <array>

namespace graphos
{

#ifdef HAVE_OPENSCENEGRAPH

OsgViewer3D::OsgViewer3D(QWidget *parent) 
  : QOpenGLWidget(parent),
    mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded(x(), y(), width(), height())),
    mViewer(new osgViewer::Viewer),
    mScaleX(1),
    mScaleY(1)
{
  init();
}

OsgViewer3D::~OsgViewer3D()
{
}

void OsgViewer3D::clear()
{

}

void OsgViewer3D::createGroup(const QString &group, const QString &parent)
{

}

void OsgViewer3D::deleteEntity(const QString &id)
{

}

void OsgViewer3D::loadFromFile(const QString &file, const QString &parent)
{
#ifdef HAVE_PCL
  pcl::PointCloud<pcl::PointXYZRGB> cloud;
#endif // HAVE_PCL

  std::string _file = file.toStdString();
  fs::path path(_file);
  std::string ext = path.extension().string();
  int i_err = -1;
#ifdef HAVE_PCL
  if (boost::iequals(ext, ".ply")){
    i_err = pcl::io::loadPLYFile(_file, cloud);
  } else if (boost::iequals(ext, ".pcd")){
    i_err = pcl::io::loadPCDFile(_file, cloud);
  } else {
    i_err = -1;
  }
#endif // HAVE_PCL
  if (i_err == -1) {
    msgError("Couldn't read file %s", _file.c_str());
    return;
  }

  osg::Geode* geode = new osg::Geode;
  //osg::Geode *geode = dynamic_cast<osg::Geode*>(mViewer->getSceneData());
  osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry);

  osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array);
  osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array);

#ifdef HAVE_PCL
  for (int i=0; i<cloud.points.size(); i++) {
    vertices->push_back (osg::Vec3 (cloud.points[i].x, cloud.points[i].y, cloud.points[i].z));
    uint32_t rgb_val_;
    memcpy(&rgb_val_, &(cloud.points[i].rgb), sizeof(uint32_t));

    uint32_t red,green,blue;
    blue=rgb_val_ & 0x000000ff;
    rgb_val_ = rgb_val_ >> 8;
    green=rgb_val_ & 0x000000ff;
    rgb_val_ = rgb_val_ >> 8;
    red=rgb_val_ & 0x000000ff;

    colors->push_back (osg::Vec4f ((float)red/255.0f, (float)green/255.0f, (float)blue/255.0f,1.0f));
  }
#endif // HAVE_PCL

  geometry->setVertexArray(vertices.get());
  geometry->setColorArray(colors.get());
  geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

  geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices->size()));
  geode->addDrawable(geometry.get());
  osg::StateSet *state = geometry->getOrCreateStateSet();
  state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osg::Camera* camera = new osg::Camera;
  camera->setViewport(0, 0, this->width(), this->height());
  camera->setClearColor(osg::Vec4(0.9f, 0.9f, 1.f, 1.f));
  float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
  camera->setProjectionMatrixAsPerspective(30., static_cast<double>(aspectRatio), 1., 1000.);
//  osg::Vec3f eye(1000.f, 0.f, 0.f);
//  osg::Vec3f dir(0.f, 0.f, 0.f);
//  osg::Vec3f up(0.f, 0.f, 1.f);
//  camera->setViewMatrixAsLookAt(eye,dir,up);

  camera->setGraphicsContext(mGraphicsWindow);

  //luces
//  osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
//  ls->getLight()->setPosition(osg::Vec4(1, -1, 1, 0));
//  ls->getLight()->setAmbient(osg::Vec4(0.2f, 0.2f, 0.2f, 1.f));
//  ls->getLight()->setDiffuse(osg::Vec4(0.7f, 0.4f, 0.6f, 1.0f));
//  ls->getLight()->setSpecular(osg::Vec4(1.f, 1.f, 1.f, 1.f));
//  geode->addChild(ls.get());

  mViewer->setCamera(camera);
  mViewer->setSceneData(geode);
  osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
  manipulator->setAllowThrow(false);
  this->setMouseTracking(true);
  mViewer->setCameraManipulator(manipulator);
  mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
  mViewer->realize();


}

void OsgViewer3D::loadFromFiles(const QStringList &files, const QString &parent)
{

}
void OsgViewer3D::setScale(double x, double y)
{
  mScaleX = x;
  mScaleY = y;
  this->resizeGL(this->width(), this->height());
}

void OsgViewer3D::addCamera(const QString & id, double x, double y, double z, const std::array<std::array<float, 3>, 3>& rot)
{
}

/* public slots */

void OsgViewer3D::deleteSelectEntity()
{

}

void OsgViewer3D::setFrontView()
{
  osg::Camera* camera = mViewer->getCamera();
}

void OsgViewer3D::setBottomView() 
{

}

void OsgViewer3D::setTopView() 
{

}

void OsgViewer3D::setBackView() 
{

}

void OsgViewer3D::setLeftView() 
{

}

void OsgViewer3D::setRightView() 
{

}

void OsgViewer3D::setIsoView1() 
{

}

void OsgViewer3D::setIsoView2() 
{

}

void OsgViewer3D::setGlobalZoom()
{

}

void OsgViewer3D::setVisible(const QString &id, bool visible)
{
/// Desactivar / activar nodos
/// osg::ref_ptr<osg::Switch> switchNode = new osg::Switch;
/// switchNode->addChild(someNode.get());
/// switchNode->addChild(otherNode.get());
///
/// switchNode->setAllChildrenOff(); // desactiva todo los nodos
/// switchNode->setSingleChildOn(1); // Activa un nodo
///
/// root->addChild(switchNode.get());
}

/* protected */

void OsgViewer3D::paintGL()
{
  mViewer->frame();
}

void OsgViewer3D::resizeGL(int width, int height)
{
  this->getEventQueue()->windowResize(this->x()*mScaleX, this->y() * mScaleY, width*mScaleX, height*mScaleY);
  mGraphicsWindow->resized(this->x()*mScaleX, this->y() * mScaleY, width * mScaleX, height * mScaleY);
  osg::Camera *camera = mViewer->getCamera();
  camera->setViewport(0, 0, this->width()*mScaleX, this->height()* mScaleY);
}

void OsgViewer3D::initializeGL()
{
  osg::Geode *geode = dynamic_cast<osg::Geode*>(mViewer->getSceneData());
  if (geode){
    osg::StateSet *stateSet = geode->getOrCreateStateSet();
    osg::Material *material = new osg::Material;
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  }
}

void OsgViewer3D::mouseMoveEvent(QMouseEvent *event)
{
  this->getEventQueue()->mouseMotion(event->x()*mScaleX, event->y()*mScaleY);
}

void OsgViewer3D::mousePressEvent(QMouseEvent* event)
{
  unsigned int button = 0;
  switch (event->button()) {
  case Qt::LeftButton:
    button = 1;
    break;
  case Qt::MiddleButton:
    button = 2;
    break;
  case Qt::RightButton:
    button = 3;
    break;
  default:
    break;
  }
  this->getEventQueue()->mouseButtonPress(event->x()*mScaleX, event->y()*mScaleY, button);
}

void OsgViewer3D::mouseReleaseEvent(QMouseEvent* event)
{
  unsigned int button = 0;
  switch (event->button()) {
  case Qt::LeftButton:
    button = 1;
    break;
  case Qt::MiddleButton:
    button = 2;
    break;
  case Qt::RightButton:
    button = 3;
    break;
  default:
    break;
  }
  this->getEventQueue()->mouseButtonRelease(event->x()*mScaleX, event->y()*mScaleY, button);
}

void OsgViewer3D::wheelEvent(QWheelEvent* event)
{
  int delta = event->delta();
  osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
    osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
  this->getEventQueue()->mouseScroll(motion);
}

bool OsgViewer3D::event(QEvent* event)
{
  bool handled = QOpenGLWidget::event(event);
  this->update();
  return handled;
}

/* private */

void OsgViewer3D::init()
{
//  osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(0.f, 0.f, 0.f), 0.25f, 0.5f);
//  osg::ShapeDrawable* sd = new osg::ShapeDrawable(cylinder);
//  sd->setColor(osg::Vec4(0.8f, 0.5f, 0.2f, 1.f));
//  osg::Geode* geode = new osg::Geode;
//  geode->addDrawable(sd);

//  osg::Camera* camera = new osg::Camera;
//  camera->setViewport(0, 0, this->width(), this->height());
//  camera->setClearColor(osg::Vec4(0.9f, 0.9f, 1.f, 1.f));
//  float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
//  camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
//  camera->setGraphicsContext(mGraphicsWindow);

//  mViewer->setCamera(camera);
//  mViewer->setSceneData(geode);
//  osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
//  manipulator->setAllowThrow(false);
//  this->setMouseTracking(true);
//  mViewer->setCameraManipulator(manipulator);
//  mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
//  //osg::Vec3d eye( 1000.0, 1000.0, 0.0 );
//  //osg::Vec3d center( 0.0, 0.0, 0.0 );
//  //osg::Vec3d up( 0.0, 1.0, 0.0 );
//  //mViewer->getCamera()->setViewMatrixAsLookAt( eye, center, up );
//  mViewer->realize();
}

osgGA::EventQueue *OsgViewer3D::getEventQueue() const
{
  osgGA::EventQueue* eventQueue = mGraphicsWindow->getEventQueue();
  return eventQueue;
}

#endif // HAVE_OPENSCENEGRAPH

/* ----------------------------------------------------------------------------------------- */

#ifdef HAVE_CLOUDCOMPARE

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

#endif // HAVE_CLOUDCOMPARE

/* ----------------------------------------------------------------------------------------- */

//
//
//OsgEarthViewer3D::OsgViewer3D(QWidget *parent) 
//  : QOpenGLWidget(parent),
//    mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded(x(), y(), width(), height())),
//    mViewer(new osgViewer::Viewer),
//    mScaleX(1),
//    mScaleY(1)
//{
//  init();
//}
//
//OsgEarthViewer3D::~OsgEarthViewer3D()
//{
//}
//
//void OsgEarthViewer3D::clear()
//{
//
//}
//
//void OsgEarthViewer3D::loadFromFile(const QString &file)
//{
//
//}
//
//void OsgEarthViewer3D::loadFromFiles(const QStringList &files)
//{
//
//}
//
//void OsgEarthViewer3D::setScale(double x, double y)
//{
//  mScaleX = x;
//  mScaleY = y;
//  this->resizeGL(this->width(), this->height());
//}
//
///* public slots */
//
//void OsgEarthViewer3D::setFrontView()
//{
//
//}
//
//void OsgEarthViewer3D::setBottomView() 
//{
//
//}
//
//void OsgEarthViewer3D::setTopView() 
//{
//
//}
//
//void OsgEarthViewer3D::setBackView() 
//{
//
//}
//
//void OsgEarthViewer3D::setLeftView() 
//{
//
//}
//
//void OsgEarthViewer3D::setRightView() 
//{
//
//}
//
//void OsgEarthViewer3D::setIsoView1() 
//{
//
//}
//
//void OsgEarthViewer3D::setIsoView2() 
//{
//
//}
//
//void OsgEarthViewer3D::setGlobalZoom()
//{
//
//}
//
//void OsgEarthViewer3D::setVisible(const QString &id, bool visible)
//{
//
//}
//
///* protected */
//
//void OsgEarthViewer3D::paintGL()
//{
//  mViewer->frame();
//}
//
//void OsgEarthViewer3D::resizeGL(int width, int height)
//{
//  this->getEventQueue()->windowResize(this->x()*mScaleX, this->y() * mScaleY, width*mScaleX, height*mScaleY);
//  mGraphicsWindow->resized(this->x()*mScaleX, this->y() * mScaleY, width * mScaleX, height * mScaleY);
//  osg::Camera *camera = mViewer->getCamera();
//  camera->setViewport(0, 0, this->width()*mScaleX, this->height()* mScaleY);
//}
//
//void OsgEarthViewer3D::initializeGL()
//{
//  osg::Geode *geode = dynamic_cast<osg::Geode*>(mViewer->getSceneData());
//  osg::StateSet *stateSet = geode->getOrCreateStateSet();
//  osg::Material *material = new osg::Material;
//  material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
//  stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
//  stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
//}
//
//void OsgEarthViewer3D::mouseMoveEvent(QMouseEvent *event)
//{
//  this->getEventQueue()->mouseMotion(event->x()*mScaleX, event->y()*mScaleY);
//}
//
//void OsgEarthViewer3D::mousePressEvent(QMouseEvent* event)
//{
//  unsigned int button = 0;
//  switch (event->button()) {
//  case Qt::LeftButton:
//    button = 1;
//    break;
//  case Qt::MiddleButton:
//    button = 2;
//    break;
//  case Qt::RightButton:
//    button = 3;
//    break;
//  default:
//    break;
//  }
//  this->getEventQueue()->mouseButtonPress(event->x()*mScaleX, event->y()*mScaleY, button);
//}
//
//void OsgEarthViewer3D::mouseReleaseEvent(QMouseEvent* event)
//{
//  unsigned int button = 0;
//  switch (event->button()) {
//  case Qt::LeftButton:
//    button = 1;
//    break;
//  case Qt::MiddleButton:
//    button = 2;
//    break;
//  case Qt::RightButton:
//    button = 3;
//    break;
//  default:
//    break;
//  }
//  this->getEventQueue()->mouseButtonRelease(event->x()*mScaleX, event->y()*mScaleY, button);
//}
//
//void OsgEarthViewer3D::wheelEvent(QWheelEvent* event)
//{
//  int delta = event->delta();
//  osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
//    osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
//  this->getEventQueue()->mouseScroll(motion);
//}
//
//bool OsgEarthViewer3D::event(QEvent* event)
//{
//  bool handled = QOpenGLWidget::event(event);
//  this->update();
//  return handled;
//}
//
///* private */
//
//void OsgEarthViewer3D::init()
//{
//  osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(0.f, 0.f, 0.f), 0.25f, 0.5f);
//  osg::ShapeDrawable* sd = new osg::ShapeDrawable(cylinder);
//  sd->setColor(osg::Vec4(0.8f, 0.5f, 0.2f, 1.f));
//  osg::Geode* geode = new osg::Geode;
//  geode->addDrawable(sd);
//
//  osg::Camera* camera = new osg::Camera;
//  camera->setViewport(0, 0, this->width(), this->height());
//  camera->setClearColor(osg::Vec4(0.9f, 0.9f, 1.f, 1.f));
//  float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
//  camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
//  camera->setGraphicsContext(mGraphicsWindow);
//
//  mViewer->setCamera(camera);
//  mViewer->setSceneData(geode);
//  osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
//  manipulator->setAllowThrow(false);
//  this->setMouseTracking(true);
//  mViewer->setCameraManipulator(manipulator);
//  mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
//  //osg::Vec3d eye( 1000.0, 1000.0, 0.0 );
//  //osg::Vec3d center( 0.0, 0.0, 0.0 );
//  //osg::Vec3d up( 0.0, 1.0, 0.0 );
//  //mViewer->getCamera()->setViewMatrixAsLookAt( eye, center, up );
//  mViewer->realize();
//}
//
//osgGA::EventQueue *OsgEarthViewer3D::getEventQueue() const
//{
//  osgGA::EventQueue* eventQueue = mGraphicsWindow->getEventQueue();
//  return eventQueue;
//}

/* ----------------------------------------------------------------------------------------- */


} // End graphos namespace
