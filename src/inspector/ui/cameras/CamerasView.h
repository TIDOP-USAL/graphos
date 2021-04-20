#ifndef INSPECTOR_CAMERAS_VIEW_INTERFACE_H
#define INSPECTOR_CAMERAS_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{

namespace ui
{

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

  virtual void setCalibCx(double cx) = 0;
  virtual void setCalibCy(double cy) = 0;
  virtual void setCalibF(double f) = 0;
  virtual void setCalibFx(double fx) = 0;
  virtual void setCalibFy(double fy) = 0;
  virtual void setCalibK1(double k1) = 0;
  virtual void setCalibK2(double k2) = 0;
  virtual void setCalibK3(double k3) = 0;
  virtual void setCalibK4(double k4) = 0;
  virtual void setCalibK5(double k5) = 0;
  virtual void setCalibK6(double k6) = 0;
  virtual void setCalibP1(double p1) = 0;
  virtual void setCalibP2(double p2) = 0;

signals:

  void cameraChange(int);
  void makeChanged(QString);
  void modelChanged(QString);
  //void widthChanged(int);
  //void heightChange(int);
  void sensorSizeChange(QString);
  void focalChange(QString);
  void typeChange(QString);

  void calibrationImport(QString, QString);
  void calibrationExport(QString, QString);
  void fixCalibration(bool);

  void calibCxChange(double);
  void calibCyChange(double);
  void calibFChange(double);
  void calibFxChange(double);
  void calibFyChange(double);
  void calibK1Change(double);
  void calibK2Change(double);
  void calibK3Change(double);
  void calibK4Change(double);
  void calibK5Change(double);
  void calibK6Change(double);
  void calibP1Change(double);
  void calibP2Change(double);

protected slots:

  virtual void onSelectionChanged() = 0;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_VIEW_INTERFACE_H
