#include "GeoreferencePresenter.h"

#include "inspector/ui/georeference/impl/GeoreferenceModel.h"
#include "inspector/ui/georeference/impl/GeoreferenceView.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/orientation/GeoreferenceProcess.h"
#include "inspector/core/orientation/photoorientation.h"
#include "inspector/ui/images/ImagesModel.h"

#include <tidop/core/defs.h>
#include <tidop/core/messages.h>

#include <QFileInfo>
#include <QAbstractItemModel>
#include <QStandardItemModel>

namespace inspector
{

namespace ui
{

GeoreferencePresenterImp::GeoreferencePresenterImp(GeoreferenceView *view,
                                                   GeoreferenceModel *model,
                                                   ImagesModel *imagesModel)
  : mView(view),
    mModel(model),
    mImagesModel(imagesModel),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

GeoreferencePresenterImp::~GeoreferencePresenterImp()
{
}

void GeoreferencePresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Georeference process error"));
  }
}

void GeoreferencePresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Georeference finished"));
  }

  //msgInfo("Georeference finished");
}

void GeoreferencePresenterImp::createProcess()
{
  mMultiProcess->clearProcessList();

  QString ori_relative = mModel->projectPath() + "/ori/relative/";
  QString ori_absolute = mModel->projectPath() + "/ori/absolute/";

  std::shared_ptr<GeoreferenceProcess> georeference_process(new GeoreferenceProcess(ori_relative, 
                                                                                    ori_absolute, 
                                                                                    mModel->groundControlPoints()));

  connect(georeference_process.get(), SIGNAL(georeferenceFinished()), this, SLOT(onGeoreferenceFinished()));

  mMultiProcess->appendProcess(georeference_process);

  if (mProgressHandler){
    mProgressHandler->setRange(0, 0);
    mProgressHandler->setTitle("Computing Georeference...");
    mProgressHandler->setDescription("Computing Georeference...");
  }

}

void GeoreferencePresenterImp::cancel()
{
  ProcessPresenter::cancel();
  
  msgWarning("Processing has been canceled by the user");
}

void GeoreferencePresenterImp::onGeoreferenceFinished()
{
  QString ori_relative_path = mModel->projectPath() + "/ori/absolute/";
  QString sparse_model = ori_relative_path + "/sparse.ply";
  if (QFileInfo(sparse_model).exists()){
    mModel->setReconstructionPath(ori_relative_path);
    mModel->setSparseModel(ori_relative_path + "/sparse.ply");
    mModel->setOffset(ori_relative_path + "/offset.txt");

    ReadPhotoOrientations readPhotoOrientations;
    readPhotoOrientations.open(ori_relative_path);
    for(auto image = mImagesModel->begin(); image != mImagesModel->end(); image++){
      PhotoOrientation photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
      if (photoOrientation.x != 0. && photoOrientation.y != 0. && photoOrientation.z != 0.) {
        mModel->addPhotoOrientation(image->name(), photoOrientation);
      }
    }

    emit georeferenceFinished();
  }
}

void GeoreferencePresenterImp::help()
{
  if (mHelp){
    TL_TODO("Añadir ayuda")
    mHelp->setPage("");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void GeoreferencePresenterImp::open()
{
  mView->clear();
  
  mModel->loadGroundControlPoints();
  mView->setCrs(mModel->crs());
  mView->showMaximized();
  
  std::vector<QString> images = mModel->images();
  if (images.empty() == false) {
    mView->setImageList(images);
    setImageActive(QFileInfo(images[0]).baseName());
  }
  
}

void GeoreferencePresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void GeoreferencePresenterImp::init()
{
  mView->setItemModelGroundControlPoints(mModel->itemModelGroundControlPoints());
  mView->setItemModelImagePoints(mModel->itemModelImagePoints());
}

void GeoreferencePresenterImp::initSignalAndSlots()
{
  connect(mView, &GeoreferenceView::imageChange, this, &GeoreferencePresenterImp::setImageActive);
  connect(mView, &GeoreferenceView::addGroundControlPoint, mModel, &GeoreferenceModel::addGroundControlPoint);
  connect(mView, &GeoreferenceView::accepted,     mModel, &GeoreferenceModel::save);
  connect(mView, &IDialogView::help,                    this, &GeoreferencePresenterImp::help);
  connect(mView, &GeoreferenceView::removeGroundControlPoint, mModel, &GeoreferenceModel::removeGroundControlPoint);
  connect(mView, &GeoreferenceView::addImagePoint, mModel, &GeoreferenceModel::addImagePoint);
  connect(mView, &GeoreferenceView::removeImagePoint, mModel, &GeoreferenceModel::removeImagePoint);
  connect(mView, &GeoreferenceView::crsChange, mModel, &GeoreferenceModel::setCrs);
  connect(mView, &GeoreferenceView::georeference, this, &ProcessPresenter::run);
}

void GeoreferencePresenterImp::setImageActive(const QString &image)
{
  mView->setCurrentImage(image);
  std::list<std::pair<QString, QPointF>> points = mModel->points(image);
  mView->setPoints(points);
}

} // namespace ui

} // namespace inspector
