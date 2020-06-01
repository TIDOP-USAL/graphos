#ifndef INSPECTOR_CAMERAS_INTERFACES_H
#define INSPECTOR_CAMERAS_INTERFACES_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/camera.h"

namespace inspector
{

namespace ui
{

class CamerasModel
  : public IModel
{

  Q_OBJECT

public:

  typedef std::map<int, Camera>::iterator camera_iterator;
  typedef std::map<int, Camera>::const_iterator camera_const_iterator;

public:

  CamerasModel(QObject *parent = nullptr) : IModel(parent) {}
  virtual ~CamerasModel() override = default;

  virtual int addCamera(const Camera &camera) = 0;
  virtual int cameraID(const Camera &camera) const = 0;
  virtual int cameraID(const QString &make, const QString &model) const = 0;
  virtual Camera camera(int id) const = 0;
  virtual Camera camera(const QString &make, const QString &model) const = 0;
  virtual bool updateCamera(int id, const Camera &camera) = 0;
  virtual bool removeCamera(int id) = 0;
  virtual bool removeCamera(const Camera &camera) = 0;

  virtual camera_iterator begin() = 0;
  virtual camera_const_iterator begin() const = 0;
  virtual camera_iterator end() = 0;
  virtual camera_const_iterator end() const = 0;

};


class CamerasView
  : public IDialogView
{

  Q_OBJECT


public:

  CamerasView(QWidget *parent) : IDialogView(parent) {}
  virtual ~CamerasView() {}

  virtual int activeCamera() const = 0;
  virtual void setActiveCamera(int id) = 0;

  virtual void enableCameraEdition(bool enable) = 0;
  virtual void addCamera(int cameraId, const QString &cameraName) = 0;
  virtual void setMake(const QString &make) = 0;
  virtual void setModel(const QString &model) = 0;
  virtual void setWidth(int width) = 0;
  virtual void setHeight(int height) = 0;
  virtual void setSensorSize(const QString &sensorSize) = 0;
  virtual void setFocal(const QString &focal) = 0;
  virtual void setType(const QString &type) = 0;
  virtual void setImages(const QStringList &images) = 0;

  virtual void setCalibCx(const QString &cx) = 0;
  virtual void setCalibCy(const QString &cy) = 0;
  virtual void setCalibF(const QString &f) = 0;
  virtual void setCalibK1(const QString &k1) = 0;
  virtual void setCalibK2(const QString &k2) = 0;
  virtual void setCalibK3(const QString &k3) = 0;
  virtual void setCalibP1(const QString &p1) = 0;
  virtual void setCalibP2(const QString &p2) = 0;

signals:

  void cameraChange(int);
  void makeChanged(QString);
  void modelChanged(QString);
  //void widthChanged(int);
  //void heightChange(int);
  void sensorSizeChange(QString);
  void focalChange(QString);
  void typeChange(QString);

  void calibCxChange(QString);
  void calibCyChange(QString);
  void calibFChange(QString);
  void calibK1Change(QString);
  void calibK2Change(QString);
  void calibK3Change(QString);
  void calibP1Change(QString);
  void calibP2Change(QString);

protected slots:

  virtual void onSelectionChanged() = 0;

};


class CamerasPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  CamerasPresenter(){}
  virtual ~CamerasPresenter() override = default;

public slots:

  virtual void activeCamera(int id) = 0;

protected slots:

  //virtual void onTypeChange(const QString &type) = 0;
  virtual void updateCurrentCameraMake(const QString &make) = 0;
  virtual void updateCurrentCameraModel(const QString &model) = 0;
  virtual void updateCurrentCameraWidth(int width) = 0;
  virtual void updateCurrentCameraHeight(int height) = 0;
  virtual void updateCurrentCameraSensorSize(const QString &sensorSize) = 0;
  virtual void updateCurrentCameraFocal(const QString &focal) = 0;
  virtual void updateCurrentCameraType(const QString &type) = 0;

  virtual void save() = 0;
  virtual void discart() = 0;

signals:

  void projectModified();

};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_INTERFACES_H
