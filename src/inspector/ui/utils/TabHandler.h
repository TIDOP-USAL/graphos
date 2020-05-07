#ifndef INSPECTOR_TAB_HANDLER_H
#define INSPECTOR_TAB_HANDLER_H

#include <QTabWidget>

namespace inspector
{

class GraphicViewer;

namespace ui
{

class TabHandler
  : public QTabWidget
{

Q_OBJECT

public:

  TabHandler(QWidget *parent = nullptr);
  ~TabHandler(){}

  QAction *actionZoomIn() const;
  QAction *actionZoomOut() const;
  QAction *actionZoomExtend() const;
  QAction *actionZoom11() const;

public slots:

  void hideTab(int tabId);
  void setCurrentTab(int tabId);
  void clear();
  void setImage(const QString &image);
  GraphicViewer *graphicViewer(int tabId);
  int graphicViewerId(const QString &name);
  GraphicViewer *addGraphicViewer(const QString &name);

protected slots:

  void onTabChanged(int tabId);
  void onTabWidgetContextMenu(const QPoint &pt);
  void update();

protected:

  void init();

protected:

  GraphicViewer *mGraphicViewer;
  QAction *mActionZoomIn;
  QAction *mActionZoomOut;
  QAction *mActionZoomExtend;
  QAction *mActionZoom11;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_TAB_HANDLER_H
