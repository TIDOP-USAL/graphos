#ifndef INSPECTOR_TAB_HANDLER_H
#define INSPECTOR_TAB_HANDLER_H

#include <QTabWidget>

namespace inspector
{

class GraphicViewer;
class Viewer3D;

namespace ui
{

class TabHandler
  : public QTabWidget
{

Q_OBJECT

public:

  TabHandler(QWidget *parent = nullptr);
  ~TabHandler() override = default;

  QAction *actionZoomIn() const;
  QAction *actionZoomOut() const;
  QAction *actionZoomExtend() const;
  QAction *actionZoom11() const;
  QAction *actionGlobalZoom() const;
  QAction *actionViewFront() const;
  QAction *actionViewTop() const;
  QAction *actionViewLeft() const;
  QAction *actionViewRight() const;
  QAction *actionViewBack() const;
  QAction *actionViewBottom() const;

public slots:

  void hideTab(int tabId);
  void setCurrentTab(int tabId);
  void clear();
  void setImage(const QString &image);
  void setModel3D(const QString &model3D);
  int tabId(const QString &name);
  GraphicViewer *graphicViewer(int tabId);
  GraphicViewer *addGraphicViewer(const QString &name);
  Viewer3D *viewer3D(int tabId);
  //Viewer3D *addViewer3D(const QString &name);
  void addCamera(const QString &image, 
                 const std::array<float, 3> &cameraPosition, 
                 const std::array<std::array<float, 3>, 3> &cameraRotationMatrix); 

protected slots:

  void onTabChanged(int tabId);
  void onTabWidgetContextMenu(const QPoint &pt);
  void update();

  void setGlobalZoom();
  void setBackView();
  void setBottomView();
  void setFrontView();
  void setTopView();
  void setLeftView();
  void setRightView();

protected:

  void init();
  void initActions();
  void connectSignalsAndSlots();
  void retranslate();

// QWidget interface

protected:

  void changeEvent(QEvent *event) override;

protected:

  GraphicViewer *mGraphicViewer;  
  Viewer3D *mViewer3D;
  QAction *mActionZoomIn;
  QAction *mActionZoomOut;
  QAction *mActionZoomExtend;
  QAction *mActionZoom11;
  QAction *mActionGlobalZoom;
  QAction *mActionViewFront;
  QAction *mActionViewTop;
  QAction *mActionViewLeft;
  QAction *mActionViewRight;
  QAction *mActionViewBack;
  QAction *mActionViewBottom;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_TAB_HANDLER_H
