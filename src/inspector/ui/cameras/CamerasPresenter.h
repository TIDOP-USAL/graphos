#ifndef INSPECTOR_CAMERAS_PRESENTER_INTERFACE_H
#define INSPECTOR_CAMERAS_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{


class CamerasPresenter
  : public Presenter
{

  Q_OBJECT

public:

  CamerasPresenter(){}
  virtual ~CamerasPresenter() override = default;

public slots:

  virtual void activeCamera(int id) = 0;
  virtual void fixCalibration(bool fix) = 0;

protected slots:

  //virtual void onTypeChange(const QString &type) = 0;
  //virtual void updateCurrentCameraMake(const QString &make) = 0;
  //virtual void updateCurrentCameraModel(const QString &model) = 0;
  //virtual void updateCurrentCameraWidth(int width) = 0;
  //virtual void updateCurrentCameraHeight(int height) = 0;
  //virtual void updateCurrentCameraSensorSize(const QString &sensorSize) = 0;
  //virtual void updateCurrentCameraFocal(const QString &focal) = 0;
  //virtual void updateCurrentCameraType(const QString &type) = 0;

  virtual void save() = 0;
  virtual void discart() = 0;

signals:

  void updateCameras();

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_PRESENTER_INTERFACE_H
