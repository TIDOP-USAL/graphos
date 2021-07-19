#ifndef INSPECTOR_IMAGE_LOADER_PRESENTER_H
#define INSPECTOR_IMAGE_LOADER_PRESENTER_H

#include "inspector/ui/images/ImageLoaderPresenter.h"


namespace inspector
{

class Image;
class Camera;

namespace ui
{

class ImageLoaderView;
class ImageLoaderModel;
class HelpDialog;

class ImageLoaderPresenterImp
  : public ImageLoaderPresenter
{

  Q_OBJECT

public:

  ImageLoaderPresenterImp(ImageLoaderView *view,
                          ImageLoaderModel *model);
  ~ImageLoaderPresenterImp() override;

// ImageLoaderPresenter interface

public slots:

  void setImages(const QStringList &files) override;

protected slots:

  void addImage(int imageId, int cameraId) override;

// ProcessPresenter interface
  
private slots:

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

private:

  ImageLoaderView *mView;
  ImageLoaderModel *mModel;
  HelpDialog *mHelp;
  QStringList mImageFiles;
  std::vector<Image> mImages;
  std::vector<Camera> mCameras;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMAGE_LOADER_PRESENTER_H
