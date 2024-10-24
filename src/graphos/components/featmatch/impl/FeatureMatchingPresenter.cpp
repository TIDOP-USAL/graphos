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
#include "graphos/core/task/Progress.h"
#include "graphos/widgets/FeatureMatchingWidget.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/task.h>

#include <QMessageBox>

namespace graphos
{

FeatureMatchingPresenterImp::FeatureMatchingPresenterImp(FeatureMatchingView *view,
                                                         FeatureMatchingModel *model)
  : FeatureMatchingPresenter(),
    mView(view),
    mModel(model),
    mFeatureMatchingWidget(new FeatureMatchingWidget)
{
    FeatureMatchingPresenterImp::init();
    FeatureMatchingPresenterImp::initSignalAndSlots();
}

FeatureMatchingPresenterImp::~FeatureMatchingPresenterImp()
{
    if (mFeatureMatchingWidget) {
        delete mFeatureMatchingWidget;
        mFeatureMatchingWidget = nullptr;
    }
}

void FeatureMatchingPresenterImp::open()
{
    this->setMatchingProperties();

    mView->setSpatialMatching(mModel->spatialMatching());
    mView->enabledSpatialMatching(mModel->spatialMatching());

    mView->exec();
}

void FeatureMatchingPresenterImp::init()
{
    mView->addMatchMethod(mFeatureMatchingWidget);
    mView->setCurrentMatchMethod(mFeatureMatchingWidget->windowTitle());
}

void FeatureMatchingPresenterImp::initSignalAndSlots()
{
    connect(mView, &FeatureMatchingView::matchMethodChange, this, &FeatureMatchingPresenterImp::setCurrentMatchMethod);
    connect(mView, &FeatureMatchingView::run, this, &FeatureMatchingPresenterImp::run);

    connect(mView, &DialogView::help, 
            [&]() {
                emit help("feature_matching.html");
            });
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
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Feature Matching error"));
    }
}

void FeatureMatchingPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Feature detection and description finished"));
    }

    mModel->writeMatchPairs();
}

std::unique_ptr<tl::Task> FeatureMatchingPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> featmatching_task;

    if (std::shared_ptr<FeatureMatching> feature_matcher = mModel->featureMatching()) {
        int i_ret = QMessageBox(QMessageBox::Warning,
                                tr("Previous results"),
                                tr("The previous results will be overwritten. Do you wish to continue?"),
                                QMessageBox::Yes | QMessageBox::No).exec();
        if (i_ret == QMessageBox::No) {
            tl::Message::warning("Process canceled by user");
            return featmatching_task;
        }
    }

    mModel->cleanProject();
    emit matches_deleted();

    QString current_match_method = mView->currentMatchMethod();

    std::shared_ptr<FeatureMatching> feature_matching;
    if (current_match_method.compare("Feature Matching Colmap") == 0) {
        feature_matching = std::make_shared<FeatureMatching>();
        feature_matching->setRatio(mFeatureMatchingWidget->ratio());
        feature_matching->setDistance(mFeatureMatchingWidget->distance());
        feature_matching->setMaxError(mFeatureMatchingWidget->maxError());
        feature_matching->setConfidence(mFeatureMatchingWidget->confidence());
        feature_matching->enableCrossCheck(mFeatureMatchingWidget->crossCheck());
    } else {
        throw std::runtime_error("Invalid Feature Matching method");
    }

    mModel->setFeatureMatching(feature_matching);

    if (mView->spatialMatching()) {
        featmatching_task = std::make_unique<SpatialMatchingTask>(mModel->database(),
                                                                  mModel->useCuda(),
                                                                  feature_matching,
                                                                  mModel->geodeticCoordinates());

        featmatching_task->subscribe([&](const tl::TaskFinalizedEvent *event) {

            auto report = dynamic_cast<SpatialMatchingTask const*>(event->task())->report();

            mModel->setFeatureMatchingReport(report);

        });

    } else {
        featmatching_task = std::make_unique<FeatureMatchingTask>(mModel->database(),
                                                                  mModel->useCuda(),
                                                                  feature_matching);

        featmatching_task->subscribe([&](const tl::TaskFinalizedEvent *event) {

            auto report = dynamic_cast<FeatureMatchingTask const*>(event->task())->report();

            mModel->setFeatureMatchingReport(report);

        });
    }

    if (progressHandler()) {
        size_t size = mModel->imagesSize();
        size_t max = size;
        if (!mView->spatialMatching()) {
            size_t block_size = 50;
            size_t num_blocks = static_cast<size_t>(std::ceil(static_cast<double>(size) / static_cast<double>(block_size)));
            max = num_blocks * num_blocks;
        }
        progressHandler()->setRange(0, max);
        progressHandler()->setTitle("Computing Matches...");
        progressHandler()->setDescription("Computing Matches...");
    }

    mView->hide();

    return featmatching_task;
}

void FeatureMatchingPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

void FeatureMatchingPresenterImp::setCurrentMatchMethod(const QString &matchMethod)
{
    mView->setCurrentMatchMethod(matchMethod);
}

} // End namespace graphos

