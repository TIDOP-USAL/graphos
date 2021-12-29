/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#ifndef GRAPHOS_TAB_HANDLER_H
#define GRAPHOS_TAB_HANDLER_H

#include <QTabWidget>

namespace graphos
{

class GraphicViewer;
class Viewer3D;

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
  void addCamera(const QString &image, 
                 const std::array<double, 3> &cameraPosition, 
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

} // namespace graphos

#endif // GRAPHOS_TAB_HANDLER_H
