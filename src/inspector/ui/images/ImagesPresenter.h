#ifndef INSPECTOR_IMAGES_PRESENTER_H
#define INSPECTOR_IMAGES_PRESENTER_H

#include "inspector/ui/images/Images.h"
#include "inspector/core/camera.h"

namespace inspector
{

namespace ui
{

class ImagesView;
class ImagesModel;
class CamerasModel;
class HelpDialog;

class ImagesPresenterImp
  : public ImagesPresenter
{

  Q_OBJECT

public:

  ImagesPresenterImp(ImagesView *view,
                     ImagesModel *model,
                     CamerasModel *camerasModel);
  ~ImagesPresenterImp() override;

// ImagesPresenter interface

public slots:

  void setImages(const QStringList &files) override;

protected slots:

  void addImage(int imageId, int cameraId) override;

// ProcessPresenter interface
  
private slots:

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

private:

  ImagesView *mView;
  ImagesModel *mModel;
  CamerasModel *mCamerasModel;
  HelpDialog *mHelp;
  QStringList mImageFiles;
  std::vector<Image> mImages;
  std::vector<Camera> mCameras;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGES_PRESENTER_H
