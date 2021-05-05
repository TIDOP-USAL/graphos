#include "DTMPresenter.h"

#include "inspector/ui/dtm/DTMView.h"
#include "inspector/ui/dtm/DTMModel.h"
#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/features/FeatureExtractorProcess.h"
#include "inspector/widgets/DtmInvDistWidget.h"
#include "inspector/widgets/DtmInvDistNNWidget.h"
#include "inspector/core/dtm/invdist.h"
#include "inspector/core/dtm/invdistnn.h"
#include "inspector/process/dtm/DtmProcess.h"
#include "inspector/core/densification/DenseExport.h"

#include <tidop/core/messages.h>

#include <QDir>
#include <QImageReader>
#include <QMessageBox>

namespace inspector
{

namespace ui
{

DtmPresenterImp::DtmPresenterImp(DtmView *view,
                                 DtmModel *model)
  : DtmPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr),
    mDtmInvDistWidget(new DtmInvDistWidgetImp),
    mDtmInvDistNNWidget(new DtmInvDistNNWidgetImp)
{
  this->init();
  this->initSignalAndSlots();
}

DtmPresenterImp::~DtmPresenterImp()
{
  if (mDtmInvDistWidget){
    delete mDtmInvDistWidget;
    mDtmInvDistWidget = nullptr;
  }

  if (mDtmInvDistNNWidget){
    delete mDtmInvDistNNWidget;
    mDtmInvDistNNWidget = nullptr;
  }
}

void DtmPresenterImp::setDtmProperties()
{
  this->setInvDistProperties();
  this->setInvDistNNProperties();
}

void DtmPresenterImp::setInvDistProperties()
{
  DtmInvDistProperties invdist;
  //TODO: Recuperar valores por defecto
  mDtmInvDistWidget->setPower(invdist.power());
  mDtmInvDistWidget->setSmoothing(invdist.smoothing());
  mDtmInvDistWidget->setRadius1(invdist.radius1());
  mDtmInvDistWidget->setRadius2(invdist.radius2());
  mDtmInvDistWidget->setAngle(invdist.angle());
  mDtmInvDistWidget->setMaxPoints(invdist.maxPoints());
  mDtmInvDistWidget->setMinPoints(invdist.minPoints());
}

void DtmPresenterImp::setInvDistNNProperties()
{
  DtmInvDistNNProperties invdistnn;
  //TODO: Recuperar valores por defecto
  mDtmInvDistNNWidget->setPower(invdistnn.power());
  mDtmInvDistNNWidget->setSmoothing(invdistnn.smoothing());
  mDtmInvDistNNWidget->setRadius(invdistnn.radius());
  mDtmInvDistNNWidget->setMaxPoints(invdistnn.maxPoints());
  mDtmInvDistNNWidget->setMinPoints(invdistnn.minPoints());
}

void DtmPresenterImp::help()
{
  if (mHelp){
    //mHelp->setPage("dtm.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void DtmPresenterImp::open()
{
  this->setDtmProperties();

  mView->exec();
}

void DtmPresenterImp::setHelp(inspector::ui::HelpDialog *help)
{
  mHelp = help;
}

void DtmPresenterImp::init()
{
  //mView->addDtmMethod(mDtmInvDistWidget);
  mView->addDtmMethod(mDtmInvDistNNWidget);
  mView->setCurrentDtmMethod(mDtmInvDistNNWidget->windowTitle());
}

void DtmPresenterImp::initSignalAndSlots()
{
  connect(mView, &DtmView::dtmMethodChange,  this, &DtmPresenterImp::setCurrentDtmMethod);
  connect(mView, &DtmView::run,   this, &DtmPresenterImp::run);
  connect(mView, &DtmView::help,  this, &DtmPresenterImp::help);
}

void DtmPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("DTM error"));
  }
}

void DtmPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("DTM finished"));
  }
}

bool DtmPresenterImp::createProcess()
{
  QString currentDtmMethod = mView->currentDtmMethod();
  std::shared_ptr<DtmAlgorithm> dtm_algorithm;

  if (currentDtmMethod.compare("Inverse distance to a power") == 0){
    dtm_algorithm = std::make_shared<DtmInvDistAlgorithm>(mDtmInvDistWidget->power(),
                                                          mDtmInvDistWidget->smoothing(),
                                                          mDtmInvDistWidget->radius1(),
                                                          mDtmInvDistWidget->radius2(), 
                                                          mDtmInvDistWidget->angle(),
                                                          mDtmInvDistWidget->maxPoints(),
                                                          mDtmInvDistWidget->minPoints());
  } else if (currentDtmMethod.compare("Inverse distance to a power with nearest neighbor searching") == 0){
    dtm_algorithm = std::make_shared<DtmInvDistNNAlgorithm>(mDtmInvDistNNWidget->power(),
                                                          mDtmInvDistNNWidget->smoothing(),
                                                          mDtmInvDistNNWidget->radius(),
                                                          mDtmInvDistNNWidget->maxPoints(),
                                                          mDtmInvDistNNWidget->minPoints());
  } else {
    mView->hide();
    throw std::runtime_error("Invalid DTM Method");
  }

  //mModel->setDtmMethod(std::dynamic_pointer_cast<Dtm>(dtm_algorithm));

  if (mProgressHandler){
    mProgressHandler->setRange(0, 0);
    mProgressHandler->setTitle("DTM generation...");
    mProgressHandler->setDescription("DTM generation...");
  }

  QString dtm_file = mModel->projectPath();
  dtm_file.append("\\dtm\\dtm.tif");

  std::shared_ptr<DenseExport> denseExport(new DenseExport(mModel->denseModel()));
  std::array<double, 3> offset = mModel->offset();
  denseExport->setOffset(offset[0], offset[1], offset[2]);

  std::shared_ptr<DtmProcess> dtm_process(new DtmProcess(dtm_algorithm,
                                                         denseExport,
                                                         dtm_file,
                                                         mView->gsd()),
                                                         mView->isDSM());

  //connect(dtm_process.get(), &DtmProcess::dtmFinished, 
  //        this, &DtmPresenterImp::onFinishDtm);

  mMultiProcess->appendProcess(dtm_process);

  mView->hide();

  return true;
}

void DtmPresenterImp::setCurrentDtmMethod(const QString &method)
{
  mView->setCurrentDtmMethod(method);
}

void inspector::ui::DtmPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

} // namespace ui

} // namespace inspector



