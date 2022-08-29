#include "DensificationPresenter.h"

#include "graphos/components/dense/DensificationModel.h"
#include "graphos/components/dense/DensificationView.h"
#include "graphos/core/dense/CmvsPmvs.h"
#include "graphos/core/dense/Smvs.h"
#include "graphos/core/dense/mvs.h"
#include "graphos/core/process/Progress.h"
#include "graphos/widgets/CmvsPmvsWidget.h"
#include "graphos/widgets/SmvsWidget.h"
#include "graphos/widgets/MvsWidget.h"

#include <tidop/core/messages.h>

#include <QMessageBox>
#include <QFileInfo>

#include <memory>


namespace graphos
{

DensificationPresenterImp::DensificationPresenterImp(DensificationView *view,
                                                     DensificationModel *model)
  : mView(view),
    mModel(model),
    mCmvsPmvs(new CmvsPmvsWidgetImp),
    mSmvs(new SmvsWidgetImp),
    mMVS(new MvsWidget)
{
  this->init();
  this->initSignalAndSlots();
}

DensificationPresenterImp::~DensificationPresenterImp()
{
  if (mCmvsPmvs){
    delete mCmvsPmvs;
    mCmvsPmvs = nullptr;
  }

  if (mSmvs){
    delete mSmvs;
    mSmvs = nullptr;
  }

  if(mMVS) {
    delete mMVS;
    mMVS = nullptr;
  }
}

void DensificationPresenterImp::open()
{
  this->setCmvsPmvsProperties();
  this->setSmvsProperties();
  this->setMvsProperties();
  mView->setCurrentDensificationMethod(mMVS->windowTitle());

  mView->exec();
}

void DensificationPresenterImp::init()
{
  mView->addDensification(mMVS);
  mView->addDensification(mCmvsPmvs);
  mView->addDensification(mSmvs);

  mView->setCurrentDensificationMethod(mCmvsPmvs->windowTitle());
}

void DensificationPresenterImp::initSignalAndSlots()
{
  connect(mView, &DensificationView::densificationChanged, this, &DensificationPresenterImp::onDensificationChanged);
  connect(mView, &DensificationView::run,                  this, &DensificationPresenterImp::run);

  connect(mView, &DialogView::help, [&]() {
    emit help("dense_cloud.html");
  });
}

void DensificationPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

void DensificationPresenterImp::setCurrentDensifier(const QString &densifier)
{
  mView->setCurrentDensificationMethod(densifier);
}


void DensificationPresenterImp::setCmvsPmvsProperties()
{
  CmvsPmvs *cmvsPmvs = nullptr;
  if (std::shared_ptr<Densification> densification = mModel->densification()){
    if (densification->method() == Densification::Method::cmvs_pmvs){
      cmvsPmvs = dynamic_cast<CmvsPmvs *>(densification.get());
    }
  } else {
    TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
  }

  if (cmvsPmvs){
    mCmvsPmvs->setLevel(cmvsPmvs->level());
    mCmvsPmvs->setCellSize(cmvsPmvs->cellSize());
    mCmvsPmvs->setThreshold(cmvsPmvs->threshold());
    mCmvsPmvs->setWindowSize(cmvsPmvs->windowSize());
    mCmvsPmvs->setImagesPerCluster(cmvsPmvs->imagesPerCluster());
    mCmvsPmvs->setMinimunImageNumber(cmvsPmvs->minimunImageNumber());
    mCmvsPmvs->setUseVisibilityInformation(cmvsPmvs->useVisibilityInformation());
  }
}

void DensificationPresenterImp::setSmvsProperties()
{
  Smvs *smvs = nullptr;
  if (std::shared_ptr<Densification> densification = mModel->densification()){
    if (densification->method() == Densification::Method::smvs){
      smvs = dynamic_cast<Smvs *>(densification.get());
    }
  } else {
    TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
  }

  if (smvs){
    mSmvs->setInputImageScale(smvs->inputImageScale());
    mSmvs->setOutputDepthScale(smvs->outputDepthScale());
    mSmvs->setSemiGlobalMatching(smvs->semiGlobalMatching());
    mSmvs->setSurfaceSmoothingFactor(smvs->surfaceSmoothingFactor());
    mSmvs->setShadingBasedOptimization(smvs->shadingBasedOptimization());
  }
}

void DensificationPresenterImp::setMvsProperties()
{
  Mvs *mvs = nullptr;
  if (std::shared_ptr<Densification> densification = mModel->densification()) {
    if (densification->method() == Densification::Method::mvs) {
      mvs = dynamic_cast<Mvs *>(densification.get());
    }
  } else {
    TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
  }

  if (mvs) {
    mMVS->setResolutionLevel(mvs->resolutionLevel());
    mMVS->setMinResolution(mvs->minResolution());
    mMVS->setMaxResolution(mvs->maxResolution());
    mMVS->setNumberViews(mvs->numberViews());
    mMVS->setNumberViewsFuse(mvs->numberViewsFuse());
  }
}

void DensificationPresenterImp::onDensificationChanged(const QString &densification)
{
  mView->setCurrentDensificationMethod(densification);
}

void DensificationPresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Densification error"));
  }
}

void DensificationPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Densification finished"));
  }

  tl::Path dense_path = dynamic_cast<DensifierBase const *>(event->task())->denseModel();

  mModel->setDenseModel(dense_path);
}

std::unique_ptr<tl::Task> DensificationPresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> dense_process;

  if (mModel->existDenseModel()) {
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes|QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      msgWarning("Process canceled by user");
      return dense_process;
    }
  }

  QString densification_method = mView->currentDensificationMethod();

  tl::Path dense_path(mModel->projectFolder());
  dense_path.append("dense");

  if (densification_method.compare("CMVS/PMVS") == 0) {

    dense_path.append("pmvs");

    auto pmvs = std::make_unique<CmvsPmvsDensifier>(mModel->images(),
                                                    mModel->cameras(),
                                                    mModel->poses(),
                                                    mModel->groundPoints(),
                                                    dense_path,
                                                    mModel->database(),
                                                    mModel->useCuda());

    pmvs->setUseVisibilityInformation(mCmvsPmvs->useVisibilityInformation());
    pmvs->setImagesPerCluster(mCmvsPmvs->imagesPerCluster());
    pmvs->setLevel(mCmvsPmvs->level()); 
    pmvs->setCellSize(mCmvsPmvs->cellSize());
    pmvs->setThreshold(mCmvsPmvs->threshold());
    pmvs->setWindowSize(mCmvsPmvs->windowSize());
    pmvs->setMinimunImageNumber(mCmvsPmvs->minimunImageNumber());

    dense_process = std::move(pmvs);

  } else if (densification_method.compare("Shading-Aware Multi-View Stereo") == 0) {

    dense_path.append("smvs");

    auto smvs = std::make_unique<SmvsDensifier>(mModel->images(),
                                                mModel->cameras(),
                                                mModel->poses(),
                                                mModel->groundPoints(),
                                                dense_path,
                                                mModel->useCuda());

    smvs->setInputImageScale(mSmvs->inputImageScale());
    smvs->setOutputDepthScale(mSmvs->outputDepthScale());
    smvs->setShadingBasedOptimization(mSmvs->shadingBasedOptimization());
    smvs->setSemiGlobalMatching(mSmvs->semiGlobalMatching());
    smvs->setSurfaceSmoothingFactor(mSmvs->surfaceSmoothingFactor());

    dense_process = std::move(smvs);

  } else if(densification_method.compare("MVS") == 0) {

    dense_path.append("mvs");

    auto mvs = std::make_unique<MvsDensifier>(mModel->images(),
                                              mModel->cameras(),
                                              mModel->poses(),
                                              mModel->groundPoints(),
                                              dense_path,
                                              mModel->database(),
                                              mModel->useCuda());

    mvs->setMaxResolution(mMVS->maxResolution());
    mvs->setMinResolution(mMVS->minResolution());
    mvs->setNumberViews(mMVS->numberViews());
    mvs->setNumberViewsFuse(mMVS->numberViewsFuse());
    mvs->setResolutionLevel(mMVS->resolutionLevel());

    dense_process = std::move(mvs);

  } else {
    mView->hide();
    throw std::runtime_error("Densification Method not valid");
  }

  //mModel->setDensification(std::dynamic_pointer_cast<Densification>(densifier));

  if (progressHandler()){
    progressHandler()->setRange(0, 1);
    progressHandler()->setTitle("Generating dense model...");
    progressHandler()->setDescription("Generating dense model...");
  }

  mView->hide();

  return dense_process;
}

} // End namespace graphos



