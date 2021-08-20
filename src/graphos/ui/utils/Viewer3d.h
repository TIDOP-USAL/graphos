#ifndef GRAPHOS_VIEWER_3D_H
#define GRAPHOS_VIEWER_3D_H

#include "config_graphos.h"

#include <memory>

/* Qt */
#include <QObject>
#include <QOpenGLWidget>

#include <tidop/core/defs.h>

/* OpenSceneGraph */
#ifdef HAVE_OPENSCENEGRAPH
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#endif // HAVE_OPENSCENEGRAPH

/* CloudCompare */
#ifdef HAVE_CLOUDCOMPARE
#include "ccGLWindow.h"
#include <ccCameraSensor.h>
#endif // HAVE_CLOUDCOMPARE

namespace graphos
{

TL_TODO("Debido a que Qt no permite la herencia de mas de un QObject la "
        "interfaz tiene que ser una clase simple. Problemas con las señales y slots...")

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


/* Visor basado en OpenSceneGraph */

#ifdef HAVE_OPENSCENEGRAPH

class OsgViewer3D
  : public QOpenGLWidget,
    public Viewer3D
{

  Q_OBJECT

private:

  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> mGraphicsWindow;
  osg::ref_ptr<osgViewer::Viewer> mViewer;
  //std::shared_ptr<osgViewer::GraphicsWindowEmbedded> mGraphicsWindow;
  //std::shared_ptr<osgViewer::Viewer> mViewer;
  double mScaleX;
  double mScaleY;

public:

  explicit OsgViewer3D(QWidget *parent = nullptr);
  virtual ~OsgViewer3D();

  void clear() override;
  void createGroup(const QString &group, const QString &parent = QString()) override;
  void deleteEntity(const QString &id) override;

  void loadFromFile(const QString &file, const QString &parent = QString()) override;
  void loadFromFiles(const QStringList &files, const QString &parent = QString()) override;

  void setScale(double x, double y);
  void addCamera(const QString &id, double x, double y, double z, const std::array<std::array<float, 3>, 3> &rot) override;

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

  virtual void paintGL() override;
  virtual void resizeGL(int width, int height) override;
  virtual void initializeGL() override;
  virtual void mouseMoveEvent(QMouseEvent* event) override;
  virtual void mousePressEvent(QMouseEvent* event) override;
  virtual void mouseReleaseEvent(QMouseEvent* event) override;
  virtual void wheelEvent(QWheelEvent* event) override;
  virtual bool event(QEvent* event) override;

private:

  void init();

  osgGA::EventQueue *getEventQueue() const;

};

#endif // HAVE_OPENSCENEGRAPH



/* Visor basado en CloudCompare */

#ifdef HAVE_CLOUDCOMPARE


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

#endif // HAVE_CLOUDCOMPARE

/* Visor basado en osgEarth */

//class OsgEarthViewer3D : public QOpenGLWidget, public TViewer3D
//{
//
//  Q_OBJECT
//
//private:
//
//  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> mGraphicsWindow;
//  osg::ref_ptr<osgViewer::Viewer> mViewer;
//  //std::shared_ptr<osgViewer::GraphicsWindowEmbedded> mGraphicsWindow;
//  //std::shared_ptr<osgViewer::Viewer> mViewer;
//  double mScaleX;
//  double mScaleY;
//
//public:
//
//  explicit OsgEarthViewer3D(QWidget *parent = nullptr);
//  virtual ~OsgEarthViewer3D();
//
//  void clear() override;
  //void deleteEntity(const QString &id) override;
  //void deleteSelectEntity() override;
//  void loadFromFile(const QString &file) override;
//  void loadFromFiles(const QStringList &files) override;
//
//  void setScale(double x, double y);
//
//  public slots:
//
//  /* Vistas por defecto */
//  void setFrontView() override;
//  void setBottomView() override;
//  void setTopView() override;
//  void setBackView() override;
//  void setLeftView() override;
//  void setRightView() override;
//  void setIsoView1() override;
//  void setIsoView2() override;
//  void setGlobalZoom() override;
//
//  void setVisible(const QString &id, bool visible) override;
//
//protected:
//
//  virtual void paintGL() override;
//  virtual void resizeGL(int width, int height) override;
//  virtual void initializeGL() override;
//  virtual void mouseMoveEvent(QMouseEvent* event) override;
//  virtual void mousePressEvent(QMouseEvent* event) override;
//  virtual void mouseReleaseEvent(QMouseEvent* event) override;
//  virtual void wheelEvent(QWheelEvent* event) override;
//  virtual bool event(QEvent* event) override;
//
//private:
//
//  void init();
//
//  osgGA::EventQueue *getEventQueue() const;
//
//};

}

#endif // GRAPHOS_VIEWER_3D_H
