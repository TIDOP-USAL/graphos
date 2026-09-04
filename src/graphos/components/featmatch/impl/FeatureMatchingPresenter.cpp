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

#include "graphos/core/matching/MatchingProperties.h"
#include "graphos/components/featmatch/impl/MatchFeaturesTask.h"
#include "graphos/components/featmatch/impl/MatchSpatiallyTask.h"
#include "graphos/components/featmatch/FeatureMatchingView.h"
#include "graphos/components/featmatch/FeatureMatchingModel.h"
#include "graphos/framework/task/ProgressHandler.h"
#include "graphos/widgets/FeatureMatchingWidget.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/task/Task.h>

#include <QMessageBox>
#include <QApplication>
#include <cmath>

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
    try {
        if (auto feature_matcher = mModel->featureMatchingConfig()) {
            mFeatureMatchingWidget->setConfidence(feature_matcher->confidence());
            mFeatureMatchingWidget->enableCrossCheck(feature_matcher->crossCheck());
            mFeatureMatchingWidget->setDistance(feature_matcher->distance());
            mFeatureMatchingWidget->setMaxError(feature_matcher->maxError());
            mFeatureMatchingWidget->setRatio(feature_matcher->ratio());
        }
    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void FeatureMatchingPresenterImp::onError(tl::TaskErrorEvent &event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("FeatureMatchingComponent", "Task error"));
    }
}

void FeatureMatchingPresenterImp::onFinished(tl::TaskFinalizedEvent &event)
{
    TaskPresenter::onFinished(event);

    if (auto *spatial_task = dynamic_cast<const MatchSpatiallyTask *>(event.task())) {
        mModel->setMatchingReport(spatial_task->report());
    } else if (auto *match_task = dynamic_cast<const MatchFeaturesTask *>(event.task())) {
        mModel->setMatchingReport(match_task->report());
    }

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("FeatureMatchingComponent", "Task finished"));
    }
}

std::unique_ptr<tl::Task> FeatureMatchingPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> featmatching_task;

    if (mModel->featureMatchingConfig()) {
        int i_ret = QMessageBox(QMessageBox::Warning,
                                QApplication::translate("FeatureMatchingComponent", "Previous results"),
                                QApplication::translate("FeatureMatchingComponent", "The previous results will be overwritten. Do you wish to continue?"),
                                QMessageBox::Yes | QMessageBox::No).exec();
        if (i_ret == QMessageBox::No) {
            tl::Message::warning("Process canceled by user");
            return featmatching_task;
        }
    }

    mModel->cleanProject();
    emit matches_deleted();

    QString current_match_method = mView->currentMatchMethod();

    std::shared_ptr<MatchingProperties> feature_matching;
    if (current_match_method.compare("Feature Matching Colmap") == 0) {
        feature_matching = std::make_shared<MatchingProperties>();
        feature_matching->setRatio(mFeatureMatchingWidget->ratio());
        feature_matching->setDistance(mFeatureMatchingWidget->distance());
        feature_matching->setMaxError(mFeatureMatchingWidget->maxError());
        feature_matching->setConfidence(mFeatureMatchingWidget->confidence());
        feature_matching->enableCrossCheck(mFeatureMatchingWidget->crossCheck());
    } else {
        throw std::runtime_error("Invalid Feature Matching method");
    }

    mModel->setFeatureMatchingConfig(feature_matching);

    if (mView->spatialMatching()) {
        featmatching_task = std::make_unique<MatchSpatiallyTask>(mModel->database(),
                                                                  mModel->useCuda(),
                                                                  feature_matching,
                                                                  mModel->geodeticCoordinates());

    } else {
        featmatching_task = std::make_unique<MatchFeaturesTask>(mModel->database(),
                                                                 mModel->useCuda(),
                                                                 feature_matching);
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
        progressHandler()->setTitle(QApplication::translate("FeatureMatchingComponent", "Feature Matching"));
        progressHandler()->setDescription(QApplication::translate("FeatureMatchingComponent", "Matching search..."));
    }

    mView->hide();

    return featmatching_task;
}

void FeatureMatchingPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Task canceled by the user");
}

void FeatureMatchingPresenterImp::setCurrentMatchMethod(const QString &matchMethod)
{
    mView->setCurrentMatchMethod(matchMethod);
}

} // End namespace graphos

