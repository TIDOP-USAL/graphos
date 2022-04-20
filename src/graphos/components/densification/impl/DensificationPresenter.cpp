#include "DensificationPresenter.h"

#include "graphos/components/densification/DensificationModel.h"
#include "graphos/components/densification/DensificationView.h"
#include "graphos/components/densification/impl/DensificationProcess.h"
#include "graphos/core/densification/CmvsPmvs.h"
#include "graphos/core/densification/Smvs.h"
#include "graphos/core/densification/mvs.h"
#include "graphos/core/process/Progress.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/widgets/CmvsPmvsWidget.h"
#include "graphos/widgets/SmvsWidget.h"
#include "graphos/widgets/MvsWidget.h"

#include <tidop/core/messages.h>

#include <QMessageBox>
#include <QFileInfo>


namespace graphos
{

DensificationPresenterImp::DensificationPresenterImp(DensificationView *view,
                                                     DensificationModel *model)
  : mView(view),
    mModel(model),
    mCmvsPmvs(new CmvsPmvsWidgetImp),
    mSmvs(new SmvsWidgetImp),
    mMVS(new MvsWidget),
    mHelp(nullptr)
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


void DensificationPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("dense_cloud.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void DensificationPresenterImp::open()
{
  this->setCmvsPmvsProperties();
  this->setSmvsProperties();
  this->setMvsProperties();
  mView->setCurrentDensificationMethod(mCmvsPmvs->windowTitle());

  mView->exec();
}

void DensificationPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void DensificationPresenterImp::init()
{
  mView->addDensification(mCmvsPmvs);
  mView->addDensification(mSmvs);
  mView->addDensification(mMVS);

  mView->setCurrentDensificationMethod(mCmvsPmvs->windowTitle());
}

void DensificationPresenterImp::initSignalAndSlots()
{
  connect(mView, &DensificationView::densificationChanged, this, &DensificationPresenterImp::onDensificationChanged);
  connect(mView, &DensificationView::run,                  this, &DensificationPresenterImp::run);
  connect(mView, &DialogView::help,                        this, &DensificationPresenterImp::help);
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

  std::shared_ptr<Densifier> densifier;
  if (densification_method.compare("CMVS/PMVS") == 0) {

    densifier = std::make_shared<CmvsPmvsDensifier>(mCmvsPmvs->useVisibilityInformation(),
                                                    mCmvsPmvs->imagesPerCluster(),
                                                    mCmvsPmvs->level(),
                                                    mCmvsPmvs->cellSize(),
                                                    mCmvsPmvs->threshold(),
                                                    mCmvsPmvs->windowSize(),
                                                    mCmvsPmvs->minimunImageNumber(),
                                                    mModel->useCuda());
  } else if (densification_method.compare("Shading-Aware Multi-View Stereo") == 0) {
    densifier = std::make_shared<SmvsDensifier>(mSmvs->inputImageScale(),
                                                mSmvs->outputDepthScale(),
                                                mSmvs->shadingBasedOptimization(),
                                                mSmvs->semiGlobalMatching(),
                                                mSmvs->surfaceSmoothingFactor(),
                                                mModel->useCuda());

  } else if(densification_method.compare("MVS") == 0) {
    densifier = std::make_shared<MvsDensifier>();

  } else {
    mView->hide();
    throw std::runtime_error("Densification Method not valid");
  }

  mModel->setDensification(std::dynamic_pointer_cast<Densification>(densifier));

  QString mReconstructionPath = mModel->reconstructionPath();
  QString mOutputPat = mModel->projectFolder() + "/dense";

  dense_process = std::make_unique<DensificationProcess>(densifier,
                                                         mReconstructionPath,
                                                         mOutputPat);

  connect(dynamic_cast<DensificationProcess *>(dense_process.get()), &DensificationProcess::densificationFinished,
          this, &DensificationPresenterImp::onFinishDensification);

  if (progressHandler()){
    progressHandler()->setRange(0, 1);
    progressHandler()->setTitle("Generating dense model...");
    progressHandler()->setDescription("Generating dense model...");
  }

  mView->hide();

  return dense_process;
}

/// Redundante con DensificationPresenterImp::onFinished()
void DensificationPresenterImp::onFinishDensification()
{
  QString dense_path = mModel->projectFolder();
  dense_path.append("/dense");
  QString densification_method = mView->currentDensificationMethod();
  if (densification_method.compare("CMVS/PMVS") == 0) {
    dense_path.append("/pmvs/models/option-all.ply");
  } else if (densification_method.compare("Shading-Aware Multi-View Stereo") == 0) {
    dense_path.append("/smvs-");
    if (mSmvs->shadingBasedOptimization()) dense_path.append("S");
    else dense_path.append("B");
    dense_path.append(QString::number(mSmvs->inputImageScale())).append(".ply");
  } else if(densification_method.compare("MVS") == 0) {
    dense_path.append("/mvs/model_dense.ply");
  } else {
    return;
  }

  if (QFileInfo(dense_path).exists()) {
    mModel->setDenseModel(dense_path);
    emit densificationFinished();
  } else {
    /// TODO: Devolver error
    msgError("Densification failed");
  }
}

} // End namespace graphos



