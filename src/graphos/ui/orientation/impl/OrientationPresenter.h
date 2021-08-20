#ifndef GRAPHOS_ORIENTATION_PRESENTER_H
#define GRAPHOS_ORIENTATION_PRESENTER_H

#include "graphos/graphos_global.h"

#include "graphos/ui/orientation/OrientationPresenter.h"
#include "graphos/core/orientation/gcp.h"

namespace graphos
{

namespace ui
{

class OrientationView;
class OrientationModel;
//class ImagesModel;
//class CamerasModel;
//class SettingsModel;
class HelpDialog;

class OrientationPresenterImp
  : public OrientationPresenter
{
  Q_OBJECT

public:

  OrientationPresenterImp(OrientationView *view,
                          OrientationModel *model/*,
                          ImagesModel *imagesModel,
                          CamerasModel *camerasModel,
                          SettingsModel *settingsModel*/);
  ~OrientationPresenterImp() override;

private slots:

  void onRelativeOrientationFinished();
  void onAbsoluteOrientationFinished();

// ProcessPresenter interface
  
protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  bool createProcess() override;

public slots:

  void cancel() override;

// Presenter interface

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
  //ImagesModel *mImagesModel;
  //CamerasModel *mCamerasModel;
  //SettingsModel *mSettingsModel;
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_PRESENTER_H
