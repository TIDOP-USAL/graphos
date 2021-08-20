#include "ImageLoaderPresenter.h"

#include "graphos/ui/images/ImageLoaderModel.h"
#include "graphos/ui/images/ImageLoaderView.h"
#include "graphos/ui/HelpDialog.h"
#include "graphos/ui/utils/Progress.h"
#include "graphos/process/MultiProcess.h"
#include "graphos/process/images/LoadImagesProcess.h"
#include "graphos/core/camera.h"
#include "graphos/core/image.h"

#include <tidop/core/messages.h>



namespace graphos
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
  mView->setImagesDirectory(mModel->imagesDirectory());
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

  QString crs_proj = mModel->projectCRS();
  QString crs_image = image.cameraPosition().crs();
  if (crs_proj.isEmpty() && !crs_image.isEmpty()) {
    mModel->setProjectCRS(crs_image);
  }

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

bool ImageLoaderPresenterImp::createProcess()
{
  if (mImageFiles.empty()) return false;

  mImages.clear();
  for (auto &image : mImageFiles) {
    Image img(image);
    mImages.push_back(img);
  }

  mCameras.clear();
  for (auto &it = mModel->cameraBegin(); it != mModel->cameraEnd(); it++) {
    mCameras.push_back(it->second);
  }

  std::shared_ptr<LoadImagesProcess> load_images(new LoadImagesProcess(&mImages, &mCameras, mModel->projectCRS()));

  connect(load_images.get(), &LoadImagesProcess::imageAdded, this, &ImageLoaderPresenterImp::addImage);

  mMultiProcess->appendProcess(load_images);

  if (mProgressHandler) {
    mProgressHandler->setRange(0, mImages.size());
    mProgressHandler->setTitle("Load images...");
    mProgressHandler->setDescription("Load images...");
  }
  
  return true;
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

} // namespace graphos
