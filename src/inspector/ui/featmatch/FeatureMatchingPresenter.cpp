#include "FeatureMatchingPresenter.h"

#include "inspector/core/features/matching.h"
#include "inspector/ui/featmatch/FeatureMatchingView.h"
#include "inspector/ui/featmatch/FeatureMatchingModel.h"
#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/features/FeatureMatchingProcess.h"
#include "inspector/widgets/FeatureMatchingWidget.h"

#include <tidop/core/messages.h>

#include <QMessageBox>

namespace inspector
{

namespace ui
{


FeatureMatchingPresenterImp::FeatureMatchingPresenterImp(FeatureMatchingView *view,
                                                         FeatureMatchingModel *model,
                                                         SettingsModel *settingsModel)
  : FeatureMatchingPresenter(),
    mView(view),
    mModel(model),
    mSettingsModel(settingsModel),
    mHelp(nullptr),
    mFeatureMatchingWidget(new FeatureMatchingWidgetImp),
    mMultiProcess(new MultiProcess(true)),
    mProgressHandler(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

FeatureMatchingPresenterImp::~FeatureMatchingPresenterImp()
{
  if (mFeatureMatchingWidget){
    delete mFeatureMatchingWidget;
    mFeatureMatchingWidget = nullptr;
  }

  if (mMultiProcess){
    delete mMultiProcess;
    mMultiProcess = nullptr;
    }
}

void FeatureMatchingPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("feature_matching.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void FeatureMatchingPresenterImp::open()
{
  this->setMatchingProperties();

  mView->exec();
}

void FeatureMatchingPresenterImp::setMatchingProperties()
{

}

void FeatureMatchingPresenterImp::onError(int code, const QString &msg)
{
  QByteArray ba = msg.toLocal8Bit();
  msgError(ba.data());

  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Feature Matching error"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void FeatureMatchingPresenterImp::onFinished()
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Feature Matching finished"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();

  //msgInfo("Feature Matching finished.");
}

void FeatureMatchingPresenterImp::onFinishMatching()
{
  mModel->writeMatchPairs();
  emit matchingFinished();
}

void FeatureMatchingPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void FeatureMatchingPresenterImp::init()
{
  mView->addMatchMethod(mFeatureMatchingWidget);
  mView->setCurrentMatchMethod(mFeatureMatchingWidget->windowTitle());
}

void FeatureMatchingPresenterImp::initSignalAndSlots()
{
  connect(mView, &FeatureMatchingView::matchMethodChange,  this, &FeatureMatchingPresenterImp::setCurrentMatchMethod);
  connect(mView, &FeatureMatchingView::run,                this, &FeatureMatchingPresenterImp::run);
  connect(mView, &IDialogView::help,                       this, &FeatureMatchingPresenterImp::help);
}

void FeatureMatchingPresenterImp::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void FeatureMatchingPresenterImp::cancel()
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

void FeatureMatchingPresenterImp::run()
{
  /// Se comprueba si ya se había ejecutado previante y se borran los datos
  if (std::shared_ptr<FeatureMatching> feature_matcher = mModel->featureMatching()){
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes|QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      return;
    }
  }
  ///
  TL_TODO("Borrar puntos de matching de la base de datos")
  mModel->clear();

  QString currentMatchMethod = mView->currentMatchMethod();

  std::shared_ptr<FeatureMatching> featureMatching;
  if (currentMatchMethod.compare("Feature Matching Colmap") == 0){
    featureMatching = std::make_shared<FeatureMatchingProperties>();
    featureMatching->setRatio(mFeatureMatchingWidget->ratio());
    featureMatching->setDistance(mFeatureMatchingWidget->distance());
    featureMatching->setMaxError(mFeatureMatchingWidget->maxError());
    featureMatching->setConfidence(mFeatureMatchingWidget->confidence());
    featureMatching->enableCrossCheck(mFeatureMatchingWidget->crossCheck());
  } else {
    TL_TODO("Devolver una excepción")
    return;
  }

  mModel->setFeatureMatching(featureMatching);

  std::shared_ptr<FeatureMatchingProcess> featMatchingProcess(new FeatureMatchingProcess(mModel->database(),
                                                                                         mSettingsModel->useCuda(),
                                                                                         featureMatching));

  connect(featMatchingProcess.get(), SIGNAL(featureMatchingFinished()), this, SLOT(onFinishMatching()));

  mMultiProcess->appendProcess(featMatchingProcess);

  connect(mMultiProcess, SIGNAL(error(int, QString)),          this, SLOT(onError(int, QString)));
  connect(mMultiProcess, SIGNAL(finished()),                   this, SLOT(onFinished()));

  if (mProgressHandler){
    connect(mMultiProcess, SIGNAL(finished()),             mProgressHandler,    SLOT(onFinish()));
    connect(mMultiProcess, SIGNAL(statusChangedNext()),    mProgressHandler,    SLOT(onNextPosition()));
    connect(mMultiProcess, SIGNAL(error(int, QString)),    mProgressHandler,    SLOT(onFinish()));

    mProgressHandler->setRange(0, 0/*mMultiProcess->count()*/);
    mProgressHandler->setValue(0);
    mProgressHandler->setTitle("Computing Matches...");
    mProgressHandler->setDescription("Computing Matches...");
    mProgressHandler->onInit();
  }

  mView->hide();

  //msgInfo("Starting Feature Matching");

  emit running();

  mMultiProcess->start();
}

void FeatureMatchingPresenterImp::setCurrentMatchMethod(const QString &matchMethod)
{
  mView->setCurrentMatchMethod(matchMethod);
}

} // End namespace ui

} // End namespace inspector

