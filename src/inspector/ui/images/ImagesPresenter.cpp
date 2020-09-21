#include "ImagesPresenter.h"

#include "inspector/ui/images/ImagesModel.h"
#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/images/LoadImagesProcess.h"

#include <tidop/core/messages.h>

#include <QFileDialog>


namespace inspector
{

namespace ui
{

ImagesPresenterImp::ImagesPresenterImp(ImagesView *view,
                                       ImagesModel *model,
                                       CamerasModel *camerasModel)
  : ImagesPresenter(),
    mView(view),
    mModel(model),
    mCamerasModel(camerasModel),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

ImagesPresenterImp::~ImagesPresenterImp()
{
}

void ImagesPresenterImp::init()
{
}

void ImagesPresenterImp::initSignalAndSlots()
{
  connect(mView, &ImagesView::accepted,      this, &ImagesPresenterImp::run);
  connect(mView, &ImagesView::rejected,      this, &ImagesPresenterImp::cancel);
  connect(mView, &ImagesView::filesSelected, this, &ImagesPresenterImp::setImages);
}

void ImagesPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("Images.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void ImagesPresenterImp::open()
{
  mView->setImageDirectory(mModel->imageDirectory());
  mView->exec();
}

void ImagesPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}


void ImagesPresenterImp::addImage(int imageId, int cameraId)
{
  Image image = mImages[imageId];
  Camera camera = mCameras[cameraId];
  int camera_id = mCamerasModel->cameraID(camera);
  if (camera_id == 0)
    camera_id = mCamerasModel->addCamera(camera);

  image.setCameraId(camera_id);
  mModel->addImage(image);

  emit imageLoaded(image.path());
}

void ImagesPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Images loaded"));
  }

  //msgInfo("Images loaded");
}

void ImagesPresenterImp::createProcess()
{
  if (mImageFiles.empty()) return;

  mImages.clear();
  for (auto &image : mImageFiles) {
    Image img(image);
    mImages.push_back(img);
  }

  mCameras.clear();
  for (auto &it = mCamerasModel->begin(); it != mCamerasModel->end(); it++) {
    mCameras.push_back(it->second);
  }

  std::shared_ptr<LoadImagesProcess> load_images(new LoadImagesProcess(&mImages, &mCameras));

  connect(load_images.get(), &LoadImagesProcess::imageAdded, this, &ImagesPresenterImp::addImage);

  mMultiProcess->appendProcess(load_images);

  if (mProgressHandler) {
    mProgressHandler->setRange(0, mImages.size());
    mProgressHandler->setTitle("Load images...");
    mProgressHandler->setDescription("Load images...");
  }
}

void ImagesPresenterImp::setImages(const QStringList &files)
{
  mImageFiles = files;
}

void ImagesPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

void ImagesPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Load images error"));
  }
}

} // namespace ui

} // namespace inspector
