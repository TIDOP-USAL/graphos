#ifndef INSPECTOR_IMAGES_PRESENTER_H
#define INSPECTOR_IMAGES_PRESENTER_H

#include "inspector/ui/images/Images.h"
#include "inspector/core/camera.h"

class MultiProcess;

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

  virtual void onError(int code, const QString &msg) override;
  virtual void onFinished() override;

public slots:
 
  void setProgressHandler(ProgressHandler *progressHandler) override;
  void cancel() override;

private slots:

  void run() override;

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
  MultiProcess *mMultiProcess;
  ProgressHandler *mProgressHandler;
  QStringList mImageFiles;
  std::vector<Image> mImages;
  std::vector<Camera> mCameras;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGES_PRESENTER_H
