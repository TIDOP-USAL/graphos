#include "FeatureMatchingPresenter.h"

#include "graphos/core/features/matching.h"
#include "graphos/ui/featmatch/FeatureMatchingView.h"
#include "graphos/ui/featmatch/FeatureMatchingModel.h"
#include "graphos/ui/SettingsModel.h"
#include "graphos/ui/utils/Progress.h"
#include "graphos/ui/HelpDialog.h"
#include "graphos/process/MultiProcess.h"
#include "graphos/process/features/FeatureMatchingProcess.h"
#include "graphos/widgets/FeatureMatchingWidget.h"

#include <tidop/core/messages.h>

#include <QMessageBox>

namespace graphos
{

namespace ui
{


FeatureMatchingPresenterImp::FeatureMatchingPresenterImp(FeatureMatchingView *view,
                                                         FeatureMatchingModel *model)
  : FeatureMatchingPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr),
    mFeatureMatchingWidget(new FeatureMatchingWidgetImp)
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
  connect(mView, &DialogView::help,                       this, &FeatureMatchingPresenterImp::help);
}

void FeatureMatchingPresenterImp::setMatchingProperties()
{
  if (std::shared_ptr<FeatureMatching> feature_matcher = mModel->featureMatching()) {
    mFeatureMatchingWidget->setConfidence(feature_matcher->confidence());
    mFeatureMatchingWidget->enableCrossCheck(feature_matcher->crossCheck());
    mFeatureMatchingWidget->setDistance(feature_matcher->distance());
    mFeatureMatchingWidget->setMaxError(feature_matcher->maxError());
    mFeatureMatchingWidget->setRatio(feature_matcher->ratio());
  }
}

void FeatureMatchingPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  QByteArray ba = msg.toLocal8Bit();
  msgError(ba.data());

  if (mProgressHandler){
    mProgressHandler->setDescription(tr("Feature Matching error"));
  }
}

void FeatureMatchingPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Feature detection and description finished"));
  }
}

bool FeatureMatchingPresenterImp::createProcess()
{
    /// Se comprueba si ya se había ejecutado previante y se borran los datos
  if (std::shared_ptr<FeatureMatching> feature_matcher = mModel->featureMatching()){
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes|QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      //throw std::runtime_error("Canceled by user");
      msgWarning("Process canceled by user");
      return false;
    }
  }

  mModel->clearProject();
  emit matchesDeleted();

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
    throw std::runtime_error("Invalid Feature Matching method");
  }

  mModel->setFeatureMatching(featureMatching);

  std::shared_ptr<FeatureMatchingProcess> featMatchingProcess(new FeatureMatchingProcess(mModel->database(),
                                                                                         mModel->useCuda(),
                                                                                         mModel->spatialMatching(),
                                                                                         featureMatching));

  connect(featMatchingProcess.get(), SIGNAL(featureMatchingFinished()), this, SLOT(onFinishMatching()));

  mMultiProcess->appendProcess(featMatchingProcess);

  if (mProgressHandler){
    mProgressHandler->setRange(0, 0);
    mProgressHandler->setTitle("Computing Matches...");
    mProgressHandler->setDescription("Computing Matches...");
  }

  mView->hide();
}

void FeatureMatchingPresenterImp::onFinishMatching()
{
  mModel->writeMatchPairs();
  emit matchingFinished();
}

void FeatureMatchingPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

void FeatureMatchingPresenterImp::setCurrentMatchMethod(const QString &matchMethod)
{
  mView->setCurrentMatchMethod(matchMethod);
}

} // End namespace ui

} // End namespace graphos

