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

#include "FeatureMatchingPresenter.h"

#include "graphos/core/features/matching.h"
#include "graphos/components/featmatch/FeatureMatchingView.h"
#include "graphos/components/featmatch/FeatureMatchingModel.h"
#include "graphos/components/featmatch/impl/FeatureMatchingProcess.h"
#include "graphos/core/process/Progress.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/widgets/FeatureMatchingWidget.h"

#include <tidop/core/messages.h>
#include <tidop/core/task.h>

#include <QMessageBox>

namespace graphos
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

  mView->setSpatialMatching(mModel->spatialMatching());
  mView->enabledSpatialMatching(mModel->spatialMatching());

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
  connect(mView, &FeatureMatchingView::matchMethodChange,  
          this,  &FeatureMatchingPresenterImp::setCurrentMatchMethod);
  connect(mView, &FeatureMatchingView::run,               
          this,  &FeatureMatchingPresenterImp::run);
  connect(mView, &DialogView::help,                     
          this,  &FeatureMatchingPresenterImp::help);
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

void FeatureMatchingPresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if (progressHandler()){
    progressHandler()->setDescription(tr("Feature Matching error"));
  }
}

void FeatureMatchingPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Feature detection and description finished"));
  }

  mModel->writeMatchPairs();
  emit matching_finished();
}

std::unique_ptr<tl::Task> FeatureMatchingPresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> featmatching_process;

  if (std::shared_ptr<FeatureMatching> feature_matcher = mModel->featureMatching()){
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes|QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      msgWarning("Process canceled by user");
      return featmatching_process;
    }
  }

  mModel->clearProject();
  emit matches_deleted();

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
  
  featmatching_process = std::make_unique<FeatureMatchingProcess>(mModel->database(),
                                                                  mModel->useCuda(),
                                                                  mView->spatialMatching(),
                                                                  featureMatching);

  if (progressHandler()){
    progressHandler()->setRange(0, 1);
    progressHandler()->setTitle("Computing Matches...");
    progressHandler()->setDescription("Computing Matches...");
  }

  mView->hide();

  return featmatching_process;
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

} // End namespace graphos

