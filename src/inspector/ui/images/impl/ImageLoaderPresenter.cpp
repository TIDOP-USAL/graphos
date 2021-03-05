#include "ImageLoaderPresenter.h"

#include "inspector/ui/images/ImageLoaderModel.h"
#include "inspector/ui/images/ImageLoaderView.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/images/LoadImagesProcess.h"
#include "inspector/core/camera.h"
#include "inspector/core/image.h"

#include <tidop/core/messages.h>

#include <QFileDialog>


namespace inspector
{

namespace ui
{

ImageLoaderPresenterImp::ImageLoaderPresenterImp(ImageLoaderView *view,
                                                 ImageLoaderModel *model)
  : ImageLoaderPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

ImageLoaderPresenterImp::~ImageLoaderPresenterImp()
{
}

void ImageLoaderPresenterImp::init()
{
}

void ImageLoaderPresenterImp::initSignalAndSlots()
{
  connect(mView, &ImageLoaderView::accepted,      this, &ImageLoaderPresenterImp::run);
  connect(mView, &ImageLoaderView::rejected,      this, &ImageLoaderPresenterImp::cancel);
  connect(mView, &ImageLoaderView::filesSelected, this, &ImageLoaderPresenterImp::setImages);
}

void ImageLoaderPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("Images.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void ImageLoaderPresenterImp::open()
{
  mView->setImageDirectory(mModel->imageDirectory());
  mView->exec();
}

void ImageLoaderPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void ImageLoaderPresenterImp::addImage(int imageId, int cameraId)
{
  Image image = mImages[imageId];
  Camera camera = mCameras[cameraId];
  int camera_id = mModel->cameraID(camera);
  if (camera_id == 0)
    camera_id = mModel->addCamera(camera);

  image.setCameraId(camera_id);
  mModel->addImage(image);

  emit imageLoaded(image.path());
}

void ImageLoaderPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Images loaded"));
  }

  //msgInfo("Images loaded");
}

void ImageLoaderPresenterImp::createProcess()
{
  if (mImageFiles.empty()) return;

  mImages.clear();
  for (auto &image : mImageFiles) {
    Image img(image);
    mImages.push_back(img);
  }

  mCameras.clear();
  for (auto &it = mModel->cameraBegin(); it != mModel->cameraEnd(); it++) {
    mCameras.push_back(it->second);
  }

  std::shared_ptr<LoadImagesProcess> load_images(new LoadImagesProcess(&mImages, &mCameras));

  connect(load_images.get(), &LoadImagesProcess::imageAdded, this, &ImageLoaderPresenterImp::addImage);

  mMultiProcess->appendProcess(load_images);

  if (mProgressHandler) {
    mProgressHandler->setRange(0, mImages.size());
    mProgressHandler->setTitle("Load images...");
    mProgressHandler->setDescription("Load images...");
  }
}

void ImageLoaderPresenterImp::setImages(const QStringList &files)
{
  mImageFiles = files;
}

void ImageLoaderPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

void ImageLoaderPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Load images error"));
  }
}

} // namespace ui

} // namespace inspector
