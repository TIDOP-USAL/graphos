#ifndef INSPECTOR_CAMERAS_PRESENTER_H
#define INSPECTOR_CAMERAS_PRESENTER_H

#include "inspector/ui/cameras/CamerasPresenter.h"

namespace inspector
{

class Camera;

namespace ui
{

class CamerasView;
class CamerasModel;
class HelpDialog;

class CamerasPresenterImp
  : public CamerasPresenter
{

  Q_OBJECT

public:

  CamerasPresenterImp(CamerasView *view,
                      CamerasModel *model);
  ~CamerasPresenterImp() override = default;

private:

  void loadCameras();

private slots:

  void calibrationImport(const QString &file,
                         const QString &format);

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

// CamerasPresenter interface

public slots:

  void activeCamera(int id) override;
  void fixCalibration(bool fix);

protected slots:

  //void onTypeChange(const QString &type) override;
  //void updateCurrentCameraMake(const QString &make) override;
  //void updateCurrentCameraModel(const QString &model) override;
  //void updateCurrentCameraWidth(int width) override;
  //void updateCurrentCameraHeight(int height) override;
  //void updateCurrentCameraSensorSize(const QString &sensorSize) override;
  //void updateCurrentCameraFocal(const QString &focal) override;
  //void updateCurrentCameraType(const QString &type) override;

  void save() override;
  void discart() override;

private:

  CamerasView *mView;
  CamerasModel *mModel;
  HelpDialog *mHelp;

  //std::map<int, Camera> mCameraCache;
  //int mActiveCameraId;
  //bool bModifiedProject;
  void clear();
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_PRESENTER_H
