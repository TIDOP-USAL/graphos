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
    mHelp(nullptr),
    mMultiProcess(new MultiProcess(true)),
    mProgressHandler(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

ImagesPresenterImp::~ImagesPresenterImp()
{
  if (mMultiProcess){
    delete mMultiProcess;
    mMultiProcess = nullptr;
  }
}

void ImagesPresenterImp::init()
{
}

void ImagesPresenterImp::initSignalAndSlots()
{
  //connect(mView, &ImagesView::cameraChange, this, &ImagesPresenterImp::activeCamera);
  //connect(mView, &ImagesView::typeChange,   this, &ImagesPresenterImp::updateCurrentCameraType);
  //connect(mView, &ImagesView::makeChanged,  this, &ImagesPresenterImp::updateCurrentCameraMake);
  //connect(mView, &ImagesView::modelChanged,  this, &ImagesPresenterImp::updateCurrentCameraModel);


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
  //mView->clear();
  //this->run();
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
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Images loaded"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
    
  }

  mMultiProcess->clearProcessList();

  emit finished();
  
  msgInfo("Images loaded");

}

void ImagesPresenterImp::setImages(const QStringList &files)
{
  mImageFiles = files;
}

void ImagesPresenterImp::cancel()
{
  mMultiProcess->stop();

  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Processing has been canceled by the user"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();

  msgWarning("Processing has been canceled by the user");
}

void ImagesPresenterImp::onError(int code, const QString &msg)
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Load images error"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(finish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(finish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void ImagesPresenterImp::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void ImagesPresenterImp::run()
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

  connect(load_images.get(), &LoadImagesProcess::imageAdded, this, &ImagesPresenterImp::addImage, Qt::QueuedConnection);

  mMultiProcess->appendProcess(load_images);

  connect(mMultiProcess, SIGNAL(error(int, QString)),          this, SLOT(onError(int, QString)));
  connect(mMultiProcess, SIGNAL(finished()),                   this, SLOT(onFinished()));

  if (mProgressHandler){
    connect(mMultiProcess, SIGNAL(finished()),             mProgressHandler,    SLOT(finish()));
    connect(mMultiProcess, SIGNAL(statusChangedNext()),    mProgressHandler,    SLOT(next()));
    connect(mMultiProcess, SIGNAL(error(int, QString)),    mProgressHandler,    SLOT(finish()));

    mProgressHandler->setRange(0, mImages.size());
    mProgressHandler->setValue(0);
    mProgressHandler->setTitle("Load images...");
    mProgressHandler->setDescription("Load images...");
    mProgressHandler->init();
  }

  emit running();

  mMultiProcess->start();
}

} // namespace ui

} // namespace inspector
