/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "DTMPresenter.h"

#include "graphos/components/dtm/DTMView.h"
#include "graphos/components/dtm/DTMModel.h"
#include "graphos/components/dtm/impl/DTMProcess.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/core/process/Progress.h"
#include "graphos/widgets/DtmInvDistWidget.h"
#include "graphos/widgets/DtmInvDistNNWidget.h"
#include "graphos/core/dtm/invdist.h"
#include "graphos/core/dtm/invdistnn.h"

#include <tidop/core/messages.h>

#include <QDir>
#include <QImageReader>
#include <QMessageBox>

namespace graphos
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
  std::shared_ptr<DtmInvDist> invdist = std::make_shared<DtmInvDistProperties>();
  if (std::shared_ptr<Dtm> dtm_properties = mModel->dtmMethod()) {
    if (dtm_properties->interpolation() == Dtm::Interpolation::inv_dist) {
      invdist = std::dynamic_pointer_cast<DtmInvDist>(dtm_properties);
    }
  }

  mDtmInvDistWidget->setPower(invdist->power());
  mDtmInvDistWidget->setSmoothing(invdist->smoothing());
  mDtmInvDistWidget->setRadius1(invdist->radius1());
  mDtmInvDistWidget->setRadius2(invdist->radius2());
  mDtmInvDistWidget->setAngle(invdist->angle());
  mDtmInvDistWidget->setMaxPoints(invdist->maxPoints());
  mDtmInvDistWidget->setMinPoints(invdist->minPoints());
}

void DtmPresenterImp::setInvDistNNProperties()
{
  std::shared_ptr<DtmInvDistNN> invdistnn = std::make_shared<DtmInvDistNNProperties>();;
  if (std::shared_ptr<Dtm> dtm_properties = mModel->dtmMethod()) {
    if (dtm_properties->interpolation() == Dtm::Interpolation::inv_distnn) {
      invdistnn = std::dynamic_pointer_cast<DtmInvDistNN>(dtm_properties);
    }
  }

  mDtmInvDistNNWidget->setPower(invdistnn->power());
  mDtmInvDistNNWidget->setSmoothing(invdistnn->smoothing());
  mDtmInvDistNNWidget->setRadius(invdistnn->radius());
  mDtmInvDistNNWidget->setMaxPoints(invdistnn->maxPoints());
  mDtmInvDistNNWidget->setMinPoints(invdistnn->minPoints());
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

void DtmPresenterImp::setHelp(HelpDialog *help)
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

void DtmPresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("DTM error"));
  }
}

void DtmPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("DTM finished"));
  }

  QString dtm_file = mModel->projectPath();
  dtm_file.append("\\dtm\\dtm.tif");
  mModel->setDtmPath(dtm_file);
}

std::unique_ptr<tl::Task> DtmPresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> dtm_process;

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

  mModel->setDtmMethod(std::dynamic_pointer_cast<Dtm>(dtm_algorithm));

  if (progressHandler()){
    progressHandler()->setRange(0, 0);
    progressHandler()->setTitle("DTM generation...");
    progressHandler()->setDescription("DTM generation...");
  }

  QString dtm_file = mModel->projectPath();
  dtm_file.append("\\dtm\\dtm.tif");

  dtm_process = std::make_unique<DtmProcess>(dtm_algorithm,
                                             mModel->denseModel(),
                                             mModel->offset(),
                                             dtm_file,
                                             mView->gsd(),
                                             mView->isDSM(),
                                             mModel->crs());

  //connect(dtm_process.get(), &DtmProcess::dtmFinished, 
  //        this, &DtmPresenterImp::onFinishDtm);

  //mMultiProcess->appendProcess(dtm_process);

  mView->hide();

  return dtm_process;
}

void DtmPresenterImp::setCurrentDtmMethod(const QString &method)
{
  mView->setCurrentDtmMethod(method);
}

void DtmPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

} // namespace graphos



