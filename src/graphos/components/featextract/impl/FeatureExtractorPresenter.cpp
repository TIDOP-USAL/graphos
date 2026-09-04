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

#include "FeatureExtractorPresenter.h"

#include "graphos/core/features/SiftProperties.h"
#include "graphos/components/featextract/impl/ExtractFeaturesTask.h"
#include "graphos/components/featextract/impl/SiftCPUExtractor.h"
#include "graphos/components/featextract/impl/SiftGPUExtractor.h"
#include "graphos/components/featextract/FeatureExtractorView.h"
#include "graphos/components/featextract/FeatureExtractorModel.h"
#include "graphos/framework/task/ProgressHandler.h"
#include "graphos/widgets/SiftWidget.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/image/ImageRepository.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/task/Task.h>

#include <QDir>
#include <QImageReader>
#include <QMessageBox>
#include <QApplication>

namespace graphos
{

FeatureExtractorPresenterImp::FeatureExtractorPresenterImp(FeatureExtractorView *view,
                                                           FeatureExtractorModel *model)
  : FeatureExtractorPresenter(),
    mView(view),
    mModel(model),
    mSift(new SiftWidget)
{
    FeatureExtractorPresenterImp::init();
    FeatureExtractorPresenterImp::initSignalAndSlots();
}

FeatureExtractorPresenterImp::~FeatureExtractorPresenterImp()
{
    if (mSift) {
        delete mSift;
        mSift = nullptr;
    }
}

void FeatureExtractorPresenterImp::open()
{
    this->setDetectorAndDescriptorProperties();

    mView->exec();
}

void FeatureExtractorPresenterImp::init()
{
    mView->addDetectorDescriptor(mSift);
    mView->setCurrentDetectorDescriptor(mSift->windowTitle());
}

void FeatureExtractorPresenterImp::initSignalAndSlots()
{
    connect(mView, &FeatureExtractorView::detectorDescriptorChange,
            this, &FeatureExtractorPresenterImp::setCurrentDetectorDescriptor);
    connect(mView, &FeatureExtractorView::run,
            this, &FeatureExtractorPresenterImp::run);
    connect(mView, &DialogView::help, 
            [&]() {
                emit help("feature_extraction.html");
            });
}

void FeatureExtractorPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Task canceled by the user");
}

void FeatureExtractorPresenterImp::setCurrentDetectorDescriptor(const QString &detectorDescriptor)
{
  mView->setCurrentDetectorDescriptor(detectorDescriptor);
}

void FeatureExtractorPresenterImp::setDetectorAndDescriptorProperties()
{
  this->setSiftProperties();
}

void FeatureExtractorPresenterImp::setSiftProperties()
{
    std::shared_ptr<SiftProperties> sift;
    if (auto config = mModel->featureConfig()) {
        sift = std::dynamic_pointer_cast<SiftProperties>(config);
    }

    if (sift) {
        mSift->setOctaveLayers(sift->octaveLayers());
        bool thresholdAuto = sift->constrastThresholdAuto();
        mSift->setContrastThresholdAuto(thresholdAuto);
        if (!thresholdAuto)
            mSift->setEdgeThreshold(sift->edgeThreshold());
        mSift->setFeaturesNumber(sift->featuresNumber());
        mSift->setContrastThreshold(sift->contrastThreshold());
    }
}

void FeatureExtractorPresenterImp::onError(tl::TaskErrorEvent &event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("FeatureExtractorComponent", "Task error"));
    }
}

void FeatureExtractorPresenterImp::onFinished(tl::TaskFinalizedEvent &event)
{
    TaskPresenter::onFinished(event);

    if (auto *extract_task = dynamic_cast<const ExtractFeaturesTask *>(event.task())) {
        mModel->setFeatureExtractorReport(extract_task->report());
    }

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("FeatureExtractorComponent", "Task finished"));
    }
}

std::unique_ptr<tl::Task> FeatureExtractorPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> feat_extract_task;

    if (mModel->featureConfig()) {

        int i_ret = QMessageBox(QMessageBox::Warning,
                                QApplication::translate("FeatureExtractorComponent", "Previous results"),
                                QApplication::translate("FeatureExtractorComponent", "The previous results will be overwritten. Do you wish to continue?"),
                                QMessageBox::Yes | QMessageBox::No).exec();

        if (i_ret == QMessageBox::No) {
            tl::Message::warning("Process canceled by user");
            return feat_extract_task;
        }

    }

    mModel->clearProject();
    emit features_deleted();

    QString currentKeyPointDetector = mView->currentDetectorDescriptor();

    auto sift_config = std::make_shared<SiftProperties>();
    sift_config->setFeaturesNumber(mSift->featuresNumber());
    sift_config->setOctaveLayers(mSift->octaveLayers());
    sift_config->setEdgeThreshold(mSift->edgeThreshold());
    sift_config->setContrastThresholdAuto(mSift->constrastThresholdAuto());
    sift_config->setContrastThreshold(mSift->constrastThresholdAuto() ? 0.0 : mSift->contrastThreshold());

    std::shared_ptr<FeatureExtractor> feature_extractor;

    if (currentKeyPointDetector.compare("SIFT") == 0) {

        if (mModel->useCuda()) {
            feature_extractor = std::make_shared<SiftGPUExtractor>(*sift_config);
        } else {
            feature_extractor = std::make_shared<SiftCPUExtractor>(*sift_config);
        }

    } else {
        mView->hide();
        throw std::runtime_error("Invalid Keypoint Detector");
    }

    mModel->setFeatureConfig(sift_config);

    int maxSize = -1;
    if (mView->fullImageSize() == false) {
        maxSize = mView->maxImageSize();
    }

    auto extract_task = std::make_unique<ExtractFeaturesTask>(mModel->images(),
                                                              mModel->cameras(),
                                                              mModel->enuCrs(),
                                                              mModel->database(),
                                                              maxSize,
                                                              mModel->useCuda(),
                                                              feature_extractor);

    connect(extract_task.get(), &ExtractFeaturesTask::features_extracted,
            this, &FeatureExtractorPresenterImp::onFeaturesExtracted);

    if (progressHandler()) {
        progressHandler()->setRange(0, mModel->images().size());
        progressHandler()->setTitle(QApplication::translate("FeatureExtractorComponent", "Feature Extraction"));
        progressHandler()->setDescription(QApplication::translate("FeatureExtractorComponent", "Image feature extraction..."));
    }

    mView->hide();

    return extract_task;
}

void FeatureExtractorPresenterImp::onFeaturesExtracted(size_t imageId,
                                                       const QString &featuresFile)
{
    mModel->addFeatures(imageId, featuresFile);
    emit features_extracted(imageId);
}

} // namespace graphos
