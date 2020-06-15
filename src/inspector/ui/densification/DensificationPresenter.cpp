#include "DensificationPresenter.h"

#include "inspector/widgets/CmvsPmvsWidget.h"
#include "inspector/widgets/SmvsWidget.h"
#include "inspector/core/densification/CmvsPmvs.h"
#include "inspector/core/densification/Smvs.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/process/densification/DensificationProcess.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/ui/HelpDialog.h"

#include <tidop/core/messages.h>

#include <QMessageBox>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace inspector
{

namespace ui
{

DensificationPresenterImp::DensificationPresenterImp(DensificationView *view,
                                     DensificationModel *model)
  : mView(view),
    mModel(model),
    mCmvsPmvs(new CmvsPmvsWidgetImp),
    mSmvs(new SmvsWidgetImp),
    mMultiProcess(new MultiProcess(true)),
    mProgressHandler(nullptr),
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

  if (mMultiProcess){
    delete mMultiProcess;
    mMultiProcess = nullptr;
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
  mView->setCurrentDensificationMethod(mSmvs->windowTitle());

  mView->exec();
}

void DensificationPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void DensificationPresenterImp::init()
{
  mView->addDensification(mSmvs);
  mView->addDensification(mCmvsPmvs);

  mView->setCurrentDensificationMethod(mSmvs->windowTitle());
}

void DensificationPresenterImp::initSignalAndSlots()
{
  connect(mView, &DensificationView::densificationChanged, this, &DensificationPresenterImp::onDensificationChanged);
  connect(mView, &DensificationView::run,                  this, &DensificationPresenterImp::run);
  connect(mView, &IDialogView::help,                       this, &DensificationPresenterImp::help);
}

void DensificationPresenterImp::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void DensificationPresenterImp::cancel()
{
  mMultiProcess->stop();

  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Processing has been canceled by the user"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();

  msgWarning("Processing has been canceled by the user");
}

void DensificationPresenterImp::run()
{

  QString densification_method = mView->currentDensificationMethod();

  std::shared_ptr<Densifier> densifier;
  if (densification_method.compare("CMVS/PMVS") == 0) {
    densifier = std::make_shared<CmvsPmvsDensifier>(mCmvsPmvs->useVisibilityInformation(),
                                                    mCmvsPmvs->imagesPerCluster(),
                                                    mCmvsPmvs->level(),
                                                    mCmvsPmvs->cellSize(),
                                                    mCmvsPmvs->threshold(),
                                                    mCmvsPmvs->windowSize(),
                                                    mCmvsPmvs->minimunImageNumber());
  } else if (densification_method.compare("Shading-Aware Multi-View Stereo") == 0) {
    densifier = std::make_shared<SmvsDensifier>(mSmvs->inputImageScale(),
                                                mSmvs->outputDepthScale(),
                                                mSmvs->shadingBasedOptimization(),
                                                mSmvs->semiGlobalMatching(),
                                                mSmvs->surfaceSmoothingFactor());

  } else {
    msgError("Densification Method not valid");
    return;
  }

  mModel->setDensification(std::dynamic_pointer_cast<Densification>(densifier));

  TL_TODO("Revisar ... ")
  QString mReconstructionPath = mModel->projectFolder() + "/sparse/0";
  QString mImagesPath = mModel->imageDirectory();
  QString mOutputPat = mModel->projectFolder() + "/dense";
  std::shared_ptr<DensificationProcess> densification_process(new DensificationProcess(densifier,
                                                                                       mReconstructionPath,
                                                                                       mImagesPath,
                                                                                       mOutputPat));

  connect(densification_process.get(), &DensificationProcess::densificationFinished, this, &DensificationPresenterImp::onFinishDensification);

  mMultiProcess->appendProcess(densification_process);

  connect(mMultiProcess, SIGNAL(error(int, QString)),          this, SLOT(onError(int, QString)));
  connect(mMultiProcess, SIGNAL(finished()),                   this, SLOT(onFinished()));

  if (mProgressHandler){
    connect(mMultiProcess, SIGNAL(finished()),             mProgressHandler,    SLOT(onFinish()));
    connect(mMultiProcess, SIGNAL(statusChangedNext()),    mProgressHandler,    SLOT(onNextPosition()));
    connect(mMultiProcess, SIGNAL(error(int, QString)),    mProgressHandler,    SLOT(onFinish()));

    mProgressHandler->setRange(0, 0/*mMultiProcess->count()*/);
    mProgressHandler->setValue(0);
    mProgressHandler->setTitle("Generating dense model...");
    mProgressHandler->setDescription("Generating dense model...");
    mProgressHandler->onInit();
  }

  mView->hide();

  emit running();

  mMultiProcess->start();
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

void DensificationPresenterImp::onDensificationChanged(const QString &densification)
{
  mView->setCurrentDensificationMethod(densification);
}

void DensificationPresenterImp::onError(int code, const QString &msg)
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Densification error"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void DensificationPresenterImp::onFinished()
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0, 1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Densification finished"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void DensificationPresenterImp::onFinishDensification()
{
  QString dense_path = mModel->projectFolder();
  dense_path.append("/dense");
  QString densification_method = mView->currentDensificationMethod();
  if (densification_method.compare("CMVS/PMVS") == 0) {
    dense_path.append("/pmvs/models/option-all.ply");
  } else if (densification_method.compare("Shading-Aware Multi-View Stereo") == 0) {
    dense_path.append("/smvs-B").append(QString::number(mSmvs->inputImageScale())).append(".ply");
  } else {
    return;
  }

  mModel->setDenseModel(dense_path);
  emit densificationFinished();
}

} // End namespace ui

} // End namespace inspector



