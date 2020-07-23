#ifndef INSPECTOR_ORIENTATION_PRESENTER_H
#define INSPECTOR_ORIENTATION_PRESENTER_H

#include "inspector/inspector_global.h"

#include "inspector/ui/orientation/Orientation.h"


namespace inspector
{

namespace ui
{

class ImagesModel;
class CamerasModel;
class SettingsModel;
class HelpDialog;

class OrientationPresenterImp
  : public OrientationPresenter
{
  Q_OBJECT

public:

  OrientationPresenterImp(OrientationView *view,
                          OrientationModel *model,
                          ImagesModel *imagesModel,
                          CamerasModel *camerasModel,
                          SettingsModel *settingsModel);
  ~OrientationPresenterImp() override;

private slots:

  void onOrientationFinished();

// ProcessPresenter interface
  
protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  void createProcess() override;

public slots:

  void cancel() override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

protected:

  OrientationView *mView;
  OrientationModel *mModel;
  ImagesModel *mImagesModel;
  CamerasModel *mCamerasModel;
  SettingsModel *mSettingsModel;
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_PRESENTER_H
