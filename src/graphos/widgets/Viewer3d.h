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

#include <tidop/core/defs.h>

/* CloudCompare */
#include "ccGLWindow.h"
#include <ccCameraSensor.h>

namespace graphos
{

/*!
 * \brief The IGraphicViewer class
 */
class Viewer3D
{

public:

  /*!
   * \brief TViewer3D
   */
  Viewer3D() {}
  virtual ~Viewer3D() {}

  virtual void clear() = 0;
  virtual void createGroup(const QString &group, const QString &parent = QString()) = 0;
  virtual void deleteEntity(const QString &id) = 0;
  virtual void deleteSelectEntity() = 0;
  virtual void loadFromFile(const QString &file, const QString &parent = QString()) = 0;
  virtual void loadFromFiles(const QStringList &files, const QString &parent = QString()) = 0;

  /* Vistas por defecto */
  virtual void setFrontView() = 0;
  virtual void setBottomView() = 0;
  virtual void setTopView() = 0;
  virtual void setBackView() = 0;
  virtual void setLeftView() = 0;
  virtual void setRightView() = 0;
  virtual void setIsoView1() = 0;
  virtual void setIsoView2() = 0;
  virtual void setGlobalZoom() = 0;

  virtual void setVisible(const QString &id, bool visible) = 0;
  
    virtual void addCamera(const QString &id, double x, double y, double z, const std::array<std::array<float,3>, 3> &rot) = 0;

protected:

};


/* Visor basado en CloudCompare */

class CCViewer3D
  : public ccGLWindow,
    public Viewer3D
{

  Q_OBJECT

private:

  ccHObject *mSelectedObject;
  double mScaleX;
  double mScaleY;
  std::map<QString, ccCameraSensor *> mCameras;

public:

  explicit CCViewer3D(QWidget *parent = nullptr);
  ~CCViewer3D() override;

  void clear() override;
  void createGroup(const QString &group, const QString &parent = QString()) override;
  void deleteEntity(const QString &id) override;

  void loadFromFile(const QString &file, const QString &parent = QString()) override;
  void loadFromFiles(const QStringList &files, const QString &parent = QString()) override;

  void setScale(double x, double y);

  void addCamera(const QString &id, double x, double y, double z, const std::array<std::array<float, 3>, 3> &rot) override;

//signals:
//
//  void entitySelect(bool);

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

protected:

  //virtual void paintGL() override;
  //virtual void resizeGL(int width, int height) override;
  //virtual void initializeGL() override;
  //virtual void mouseMoveEvent(QMouseEvent* event) override;
  //virtual void mousePressEvent(QMouseEvent* event) override;
  //virtual void mouseReleaseEvent(QMouseEvent* event) override;
  //virtual void wheelEvent(QWheelEvent* event) override;
  //virtual bool event(QEvent* event) override;

protected slots:

  /*!
   * \brief selecciona una entidad
   */
  void selectEntity(ccHObject *entity);

private:

  void init();

  void addToDB(ccHObject *entity);
  //void scaleAlreadyDisplayed(ccHObject *entity);

  /*!
   * \brief Función para buscar un objeto mediante su nombre
   */
  ccHObject *findChild(const QString &name, ccHObject *parent = nullptr);
};


}

#endif // GRAPHOS_VIEWER_3D_H
