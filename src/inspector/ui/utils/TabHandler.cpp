#include "TabHandler.h"

#include "inspector/ui/utils/GraphicViewer.h"
#include "inspector/ui/utils/GraphicItem.h"
#include "inspector/ui/utils/Viewer3d.h"

#include <QTabBar>
#include <QMenu>
#include <QFileInfo>
#include <QEvent>
#include <QApplication>

#include <array>

namespace inspector
{

namespace ui
{

TabHandler::TabHandler(QWidget *parent)
  : QTabWidget(parent),
    mGraphicViewer(nullptr),
    mViewer3D(nullptr),
    mActionZoomIn(new QAction(this)),
    mActionZoomOut(new QAction(this)),
    mActionZoomExtend(new QAction(this)),
    mActionZoom11(new QAction(this)),
    mActionGlobalZoom(new QAction(this)),
    mActionViewFront(new QAction(this)),
    mActionViewTop(new QAction(this)),
    mActionViewLeft(new QAction(this)),
    mActionViewRight(new QAction(this)),
    mActionViewBack(new QAction(this)),
    mActionViewBottom(new QAction(this))
{
  this->init();
  this->connectSignalsAndSlots();
}

void TabHandler::init()
{
  this->setObjectName(QString("TabHandler"));
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  this->setTabsClosable(true);

  this->initActions();
  this->retranslate();
  this->update();
}

void TabHandler::initActions()
{
  QIcon iconZoomIn;
  iconZoomIn.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomIn->setIcon(iconZoomIn);

  QIcon iconZoomOut;
  iconZoomOut.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-out.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomOut->setIcon(iconZoomOut);

  QIcon iconZoomExtend;
  iconZoomExtend.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-magnifying-glass-with-expand-sign.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomExtend->setIcon(iconZoomExtend);

  QIcon iconZoom11;
  iconZoom11.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-one-to-one.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoom11->setIcon(iconZoom11);

  mActionGlobalZoom->setIcon(iconZoomExtend);

  QIcon iconViewFront;
  iconViewFront.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-front-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewFront->setIcon(iconViewFront);

  QIcon iconViewTop;
  iconViewTop.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-top-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewTop->setIcon(iconViewTop);

  QIcon iconViewLeft;
  iconViewLeft.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-cube.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewLeft->setIcon(iconViewLeft);

  QIcon iconViewRight;
  iconViewRight.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-view_right.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewRight->setIcon(iconViewRight);

  QIcon iconViewBack;
  iconViewBack.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-back-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewBack->setIcon(iconViewBack);

  QIcon iconViewBottom;
  iconViewBottom.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-bottom-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewBottom->setIcon(iconViewBottom);
}

void TabHandler::connectSignalsAndSlots()
{
  connect(this, SIGNAL(tabCloseRequested(int)),                     this, SLOT(hideTab(int)));
  connect(this, SIGNAL(currentChanged(int)),                        this, SLOT(onTabChanged(int)));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onTabWidgetContextMenu(const QPoint &)));
}

void TabHandler::hideTab(int tabId)
{
  if (tabId != -1) {
    GraphicViewer *graphicViewer = dynamic_cast<GraphicViewer *>(this->widget(tabId));
    Viewer3D *viewer3D = dynamic_cast<Viewer3D *>(this->widget(tabId));
    this->removeTab(tabId);
    if (graphicViewer){
      delete graphicViewer;
      graphicViewer = nullptr;
      mGraphicViewer = nullptr;
    } else if (viewer3D){
      delete viewer3D;
      viewer3D = nullptr;
      mViewer3D = nullptr;
    }
  }

  update();
}

void TabHandler::setCurrentTab(int tabId)
{
  this->setCurrentIndex(tabId);
  if (GraphicViewer *graphicViewer = dynamic_cast<GraphicViewer *>(this->widget(tabId)))
    mGraphicViewer = graphicViewer;

  if (Viewer3D *viewer3D = dynamic_cast<Viewer3D *>(this->widget(tabId)))
    mViewer3D = viewer3D;
}

void TabHandler::clear()
{
  int n = this->count();
  for (int i = 0; i < n; i++){
    hideTab(0);
  }

  update();
}

void TabHandler::setImage(const QString &image)
{
  const QSignalBlocker blocker(this);

  QFileInfo fileInfo(image);

  if (mGraphicViewer != nullptr){
    disconnect(mActionZoomIn,      SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomIn()));
    disconnect(mActionZoomOut,     SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomOut()));
    disconnect(mActionZoomExtend,  SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomExtend()));
    disconnect(mActionZoom11,      SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoom11()));

    for (auto &item : mGraphicViewer->scene()->items()) {
      KeyPointGraphicsItem *keyPoints = dynamic_cast<KeyPointGraphicsItem *>(item);
      if (keyPoints){
        mGraphicViewer->scene()->removeItem(item);
      }
    }
  }

  // Carga en nueva pestaña
  int id = -1;
  for (int i = 0; i < this->count(); i++){

    if (this->tabToolTip(i) == image){
      id = i;
      this->setCurrentTab(i);
      break;
    }
  }

  if (id == -1) {
    GraphicViewer *graphicViewer = new GraphicViewerImp(this);
    mGraphicViewer = graphicViewer;
    mGraphicViewer->setImage(QImage(image));
    id = this->addTab(mGraphicViewer, fileInfo.fileName());
    this->setCurrentIndex(id);
    this->setTabToolTip(id, image);

    mGraphicViewer->zoomExtend();

    QMenu *contextMenu = new QMenu(graphicViewer);
    contextMenu->addAction(mActionZoomIn);
    contextMenu->addAction(mActionZoomOut);
    contextMenu->addAction(mActionZoomExtend);
    contextMenu->addAction(mActionZoom11);
    mGraphicViewer->setContextMenu(contextMenu);
  }

  connect(mActionZoomIn,      SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomIn()));
  connect(mActionZoomOut,     SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomOut()));
  connect(mActionZoomExtend,  SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomExtend()));
  connect(mActionZoom11,      SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoom11()));

  update();
}

void TabHandler::setModel3D(const QString &model3D)
{
  const QSignalBlocker blocker(this);

  QFileInfo fileInfo(model3D);

  if (mViewer3D != nullptr){
    disconnect(mActionGlobalZoom,   SIGNAL(triggered(bool)), this, SLOT(setGlobalZoom()));
    disconnect(mActionViewFront,    SIGNAL(triggered(bool)), this, SLOT(setFrontView()));
    disconnect(mActionViewBack,     SIGNAL(triggered(bool)), this, SLOT(setBackView()));
    disconnect(mActionViewTop,      SIGNAL(triggered(bool)), this, SLOT(setTopView()));
    disconnect(mActionViewBottom,   SIGNAL(triggered(bool)), this, SLOT(setBottomView()));
    disconnect(mActionViewLeft,     SIGNAL(triggered(bool)), this, SLOT(setLeftView()));
    disconnect(mActionViewRight,    SIGNAL(triggered(bool)), this, SLOT(setRightView()));
  }


  // Carga en nueva pestaña
  int id = -1;
  for (int i = 0; i < this->count(); i++){

    if (this->tabToolTip(i) == model3D){
      id = i;
      ///No se si es necesario
      //mViewer3D = dynamic_cast<Viewer3D *>(this->widget(i));
      this->setCurrentIndex(id);
      break;
    }
  }

  if (id == -1) {
#ifdef HAVE_CLOUDCOMPARE
    mViewer3D = new CCViewer3D(/*this*/);
#elif defined (HAVE_OPENSCENEGRAPH)
    mViewer3D = new OsgViewer3D(this);
#endif

    mViewer3D->loadFromFile(model3D);
    id = this->addTab(dynamic_cast<QWidget *>(mViewer3D), fileInfo.fileName());
    this->setCurrentIndex(id);
    this->setTabToolTip(id, model3D);

    mViewer3D->setGlobalZoom();
  }

  connect(mActionGlobalZoom, SIGNAL(triggered(bool)), this, SLOT(setGlobalZoom()));
  connect(mActionViewFront,  SIGNAL(triggered(bool)), this, SLOT(setFrontView()));
  connect(mActionViewBack,   SIGNAL(triggered(bool)), this, SLOT(setBackView()));
  connect(mActionViewTop,    SIGNAL(triggered(bool)), this, SLOT(setTopView()));
  connect(mActionViewBottom, SIGNAL(triggered(bool)), this, SLOT(setBottomView()));
  connect(mActionViewLeft,   SIGNAL(triggered(bool)), this, SLOT(setLeftView()));
  connect(mActionViewRight,  SIGNAL(triggered(bool)), this, SLOT(setRightView()));

  update();
}

GraphicViewer *TabHandler::graphicViewer(int tabId)
{
  return dynamic_cast<GraphicViewer *>(this->widget(tabId));
}

int TabHandler::tabId(const QString &name)
{
  int id = -1;
  for (int i = 0; i < this->count(); i++){
    if (this->tabToolTip(i) == name){
      id = i;
    }
  }
  return id;
}

GraphicViewer *TabHandler::addGraphicViewer(const QString &name)
{

  if (mGraphicViewer != nullptr){
    disconnect(mActionZoomIn,     SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomIn()));
    disconnect(mActionZoomOut,    SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomOut()));
    disconnect(mActionZoomExtend, SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomExtend()));
    disconnect(mActionZoom11,     SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoom11()));
  }

  int id = this->tabId(name);

  if (id == -1 ) {
    GraphicViewer *graphicViewer = new GraphicViewerImp(this);

    QMenu *contextMenu = new QMenu(graphicViewer);
    contextMenu->addAction(mActionZoomIn);
    contextMenu->addAction(mActionZoomOut);
    contextMenu->addAction(mActionZoomExtend);
    contextMenu->addAction(mActionZoom11);
    graphicViewer->setContextMenu(contextMenu);

    id = this->addTab(graphicViewer, name);
    this->setTabToolTip(id, name);
  }

  this->setCurrentTab(id);

  connect(mActionZoomIn,     SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomIn()));
  connect(mActionZoomOut,    SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomOut()));
  connect(mActionZoomExtend, SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoomExtend()));
  connect(mActionZoom11,     SIGNAL(triggered(bool)), mGraphicViewer, SLOT(zoom11()));

  return mGraphicViewer;
}

Viewer3D *TabHandler::viewer3D(int tabId)
{
  return dynamic_cast<Viewer3D *>(this->widget(tabId));
}

void TabHandler::addCamera(const QString &image, 
                           const std::array<double, 3> &cameraPosition, 
                           const std::array<std::array<float, 3>, 3> &cameraRotationMatrix)
{
  mViewer3D->addCamera(image, cameraPosition[0], cameraPosition[1], cameraPosition[2], cameraRotationMatrix);
}

//Viewer3D *TabHandler::addViewer3D(const QString &name)
//{

//}

void TabHandler::onTabChanged(int tabId)
{
  GraphicViewer *graphicViewer = dynamic_cast<GraphicViewer *>(this->widget(tabId));
  Viewer3D *viewer3D = dynamic_cast<Viewer3D *>(this->widget(tabId));

  if (graphicViewer){
    this->setImage(this->tabToolTip(tabId));
  } else if (viewer3D){
    this->setModel3D(this->tabToolTip(tabId));
  } else {
    mGraphicViewer = nullptr;
  }

  update();
}

void TabHandler::onTabWidgetContextMenu(const QPoint &pt)
{
  if (pt.isNull()) return;

  if (this->tabBar() == nullptr) return;

  int tabIndex = this->tabBar()->tabAt(pt);

  if (tabIndex == -1) return;

  QString tabText = this->tabBar()->tabText(tabIndex);

  QMenu menu;
  menu.addAction(tr("Close"));
  menu.addAction(tr("Close all tabs"));
  menu.addAction(tr("Close all tabs but current one"));

  if (QAction *selectedTab = menu.exec(this->tabBar()->mapToGlobal(pt))) {
    if (selectedTab->text() == tr("Close")) {
      hideTab(tabIndex);
    } else if (selectedTab->text() == tr("Close all tabs")) {
      const QSignalBlocker blocker(this);
      int n = this->count();
      for (int i = 0; i < n; i++){
        hideTab(0);
      }
    } else if (selectedTab->text() == tr("Close all tabs but current one")) {
      const QSignalBlocker blocker(this);
      int n = this->count();
      int tabToCloseId = 0;
      for (int i = 0; i < n; i++){
        if (this->tabBar()->tabText(tabToCloseId).compare(tabText) == 0){
          tabToCloseId = 1;
        } else {
          hideTab(tabToCloseId);
        }
      }
    }
  }
}

void TabHandler::update()
{
  bool bImageOpen = (mGraphicViewer != nullptr);
  mActionZoomIn->setEnabled(bImageOpen);
  mActionZoomOut->setEnabled(bImageOpen);
  mActionZoomExtend->setEnabled(bImageOpen);
  mActionZoom11->setEnabled(bImageOpen);

  bool bModel3DOpen = (mViewer3D != nullptr);
  mActionGlobalZoom->setEnabled(bModel3DOpen);
  mActionViewFront->setEnabled(bModel3DOpen);
  mActionViewBack->setEnabled(bModel3DOpen);
  mActionViewTop->setEnabled(bModel3DOpen);
  mActionViewBottom->setEnabled(bModel3DOpen);
  mActionViewLeft->setEnabled(bModel3DOpen);
  mActionViewRight->setEnabled(bModel3DOpen);
}

void TabHandler::retranslate()
{
  mActionZoomIn->setText(QApplication::translate("TabHandler", "Zoom In"));
  mActionZoomOut->setText(QApplication::translate("TabHandler", "Zoom Out"));
  mActionZoomExtend->setText(QApplication::translate("TabHandler", "Zoom Extend"));
  mActionZoom11->setText(QApplication::translate("TabHandler", "Zoom 1:1"));

  mActionGlobalZoom->setText(QApplication::translate("TabHandler", "Global Zoom", nullptr));
  mActionViewFront->setText(QApplication::translate("TabHandler", "View Front", nullptr));
  mActionViewTop->setText(QApplication::translate("TabHandler", "View Top", nullptr));
  mActionViewLeft->setText(QApplication::translate("TabHandler", "View Left", nullptr));
  mActionViewRight->setText(QApplication::translate("TabHandler", "View Right", nullptr));
  mActionViewBack->setText(QApplication::translate("TabHandler", "View Back", nullptr));
  mActionViewBottom->setText(QApplication::translate("TabHandler", "View Bottom", nullptr));
}

QAction *TabHandler::actionZoom11() const
{
  return mActionZoom11;
}

QAction *TabHandler::actionZoomExtend() const
{
  return mActionZoomExtend;
}

QAction *TabHandler::actionZoomOut() const
{
  return mActionZoomOut;
}

QAction *TabHandler::actionZoomIn() const
{
  return mActionZoomIn;
}

QAction *TabHandler::actionViewBottom() const
{
  return mActionViewBottom;
}

QAction *TabHandler::actionViewBack() const
{
  return mActionViewBack;
}

QAction *TabHandler::actionViewRight() const
{
  return mActionViewRight;
}

QAction *TabHandler::actionViewLeft() const
{
  return mActionViewLeft;
}

QAction *TabHandler::actionViewTop() const
{
  return mActionViewTop;
}

QAction *TabHandler::actionViewFront() const
{
  return mActionViewFront;
}

QAction *TabHandler::actionGlobalZoom() const
{
  return mActionGlobalZoom;
}

void TabHandler::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange){
      this->retranslate();
  }
}

void TabHandler::setGlobalZoom()
{
  if (mViewer3D){
    mViewer3D->setGlobalZoom();
  }
}

void TabHandler::setBackView()
{
  if (mViewer3D){
    mViewer3D->setBackView();
  }
}

void TabHandler::setBottomView()
{
  if (mViewer3D){
    mViewer3D->setBottomView();
  }
}

void TabHandler::setFrontView()
{
  if (mViewer3D){
    mViewer3D->setFrontView();
  }
}

void TabHandler::setTopView()
{
  if (mViewer3D){
    mViewer3D->setTopView();
  }
}

void TabHandler::setLeftView()
{
  if (mViewer3D){
    mViewer3D->setLeftView();
  }
}

void TabHandler::setRightView()
{
  if (mViewer3D){
    mViewer3D->setRightView();
  }
}

} // namespace ui

} // namespace inspector


