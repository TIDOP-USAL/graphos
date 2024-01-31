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

#include "graphos/core/features/features.h"
#include "graphos/core/features/sift.h"
#include "graphos/core/features/featextract.h"
#include "graphos/components/featextract/FeatureExtractorView.h"
#include "graphos/components/featextract/FeatureExtractorModel.h"
#include "graphos/core/task/Progress.h"
#include "graphos/widgets/SiftWidget.h"
#include "graphos/core/camera/Camera.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/task.h>

#include <QDir>
#include <QImageReader>
#include <QMessageBox>

namespace graphos
{

FeatureExtractorPresenterImp::FeatureExtractorPresenterImp(FeatureExtractorView *view,
                                                           FeatureExtractorModel *model)
  : FeatureExtractorPresenter(),
    mView(view),
    mModel(model),
    mSift(new SiftWidgetImp)
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

    tl::Message::warning("Processing has been canceled by the user");
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
    std::shared_ptr<Sift> sift;
    if (std::shared_ptr<Feature> feature_extractor = mModel->featureExtractor()) {
        if (feature_extractor->type() == Feature::Type::sift) {
            sift = std::dynamic_pointer_cast<Sift>(feature_extractor);
        }
    } else {
        TL_TODO("Sift *sift = mSettingsModel->sift();")
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

void FeatureExtractorPresenterImp::onError(tl::TaskErrorEvent* event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Feature detection and description error"));
    }
}

void FeatureExtractorPresenterImp::onFinished(tl::TaskFinalizedEvent* event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Feature detection and description finished"));
    }
}

std::unique_ptr<tl::Task> FeatureExtractorPresenterImp::createProcess()
{
    std::unique_ptr<tl::Task> feat_extract_task;

    if (std::shared_ptr<Feature> feature_extractor = mModel->featureExtractor()) {

        int i_ret = QMessageBox(QMessageBox::Warning,
                                tr("Previous results"),
                                tr("The previous results will be overwritten. Do you wish to continue?"),
                                QMessageBox::Yes | QMessageBox::No).exec();

        if (i_ret == QMessageBox::No) {
            tl::Message::warning("Process canceled by user");
            return feat_extract_task;
        }

    }

    mModel->clearProject();
    emit features_deleted();

    QString currentKeyPointDetector = mView->currentDetectorDescriptor();
    std::shared_ptr<FeatureExtractor> feature_extractor;

    if (currentKeyPointDetector.compare("SIFT") == 0) {

        if (mModel->useCuda()) {
            feature_extractor = std::make_shared<SiftCudaDetectorDescriptor>(mSift->featuresNumber(),
                                                                             mSift->octaveLayers(),
                                                                             mSift->edgeThreshold(),
                                                                             mSift->constrastThresholdAuto() ?
                                                                             0. : mSift->contrastThreshold());
        } else {
            feature_extractor = std::make_shared<SiftCPUDetectorDescriptor>(mSift->featuresNumber(),
                                                                            mSift->octaveLayers(),
                                                                            mSift->edgeThreshold(),
                                                                            mSift->constrastThresholdAuto() ?
                                                                            0. : mSift->contrastThreshold());
        }

    } else {
        mView->hide();
        throw std::runtime_error("Invalid Keypoint Detector");
    }

    mModel->setFeatureExtractor(std::dynamic_pointer_cast<Feature>(feature_extractor));

    const std::unordered_map<size_t, Image> &images = mModel->images();

    int maxSize = -1;
    if (mView->fullImageSize() == false) {
        maxSize = mView->maxImageSize();
    }

    feat_extract_task = std::make_unique<FeatureExtractorTask>(images,
                                                                  mModel->cameras(),
                                                                  mModel->database(),
                                                                  maxSize,
                                                                  mModel->useCuda(),
                                                                  feature_extractor);

    connect(dynamic_cast<FeatureExtractorTask*>(feat_extract_task.get()), &FeatureExtractorTask::features_extracted,
            this, &FeatureExtractorPresenterImp::onFeaturesExtracted);

    feat_extract_task->subscribe([&](tl::TaskFinalizedEvent *event) {

        auto report = dynamic_cast<FeatureExtractorTask const*>(event->task())->report();
    
        mModel->setFeatureExtractorReport(report);

    });

    if (progressHandler()) {
        progressHandler()->setRange(0, images.size());
        progressHandler()->setTitle("Computing Features...");
        progressHandler()->setDescription("Computing Features...");
    }

    mView->hide();

    return feat_extract_task;
}

void FeatureExtractorPresenterImp::onFeaturesExtracted(size_t imageId,
                                                       const QString &featuresFile)
{
    mModel->addFeatures(imageId, featuresFile);
    emit features_extracted(imageId);
}

} // namespace graphos
