/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#include "DensificationPresenter.h"

#include "graphos/components/dense/DensificationModel.h"
#include "graphos/components/dense/DensificationView.h"
#include "graphos/components/dense/impl/CmvsPmvsTask.h"
#include "graphos/components/dense/impl/SmvsTask.h"
#include "graphos/components/dense/impl/MvsTask.h"
#include "graphos/framework/task/ProgressHandler.h"
#include "graphos/widgets/CmvsPmvsWidget.h"
#include "graphos/widgets/SmvsWidget.h"
#include "graphos/widgets/MvsWidget.h"

#include <tidop/core/app/Message.h>

#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>

#include <memory>


namespace graphos
{

DensificationPresenterImp::DensificationPresenterImp(DensificationView *view,
                                                     DensificationModel *model)
  : mView(view),
    mModel(model)/*,
    mCmvsPmvs(new CmvsPmvsWidget),
    mSmvs(new SmvsWidget),
    mMVS(new MvsWidget)*/
{
    DensificationPresenterImp::init();
    DensificationPresenterImp::initSignalAndSlots();
}

DensificationPresenterImp::~DensificationPresenterImp() = default;

void DensificationPresenterImp::open()
{
    this->configureCmvsPmvsProperties();
    this->configureSmvsProperties();
    this->configureMvsProperties();

    mView->setCurrentDensificationMethod(mMVS->windowTitle());

    mView->exec();
}

void DensificationPresenterImp::init()
{

}

void DensificationPresenterImp::initSignalAndSlots()
{
    connect(mView, &DensificationView::densificationChanged, this, &DensificationPresenterImp::onDensificationChanged);
    connect(mView, &DensificationView::run, this, &DensificationPresenterImp::run);

    connect(mView, &DialogView::help, [&]() {
        emit help("densification.html");
            });
}

void DensificationPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Task canceled by the user");
}

void DensificationPresenterImp::setCurrentDensifier(const QString &densifier)
{
    mView->setCurrentDensificationMethod(densifier);
}


void DensificationPresenterImp::configureCmvsPmvsProperties() const
{
    if (auto properties = mModel->densificationConfig()) {
        if (properties->name() == "PMVS") {
            auto cmvs_pmvs = dynamic_cast<CmvsPmvsProperties *>(properties.get());
            if (cmvs_pmvs && mCmvsPmvs) {
                mCmvsPmvs->setLevel(cmvs_pmvs->level());
                mCmvsPmvs->setCellSize(cmvs_pmvs->cellSize());
                mCmvsPmvs->setThreshold(cmvs_pmvs->threshold());
                mCmvsPmvs->setWindowSize(cmvs_pmvs->windowSize());
                mCmvsPmvs->setImagesPerCluster(cmvs_pmvs->imagesPerCluster());
                mCmvsPmvs->setMinimunImageNumber(cmvs_pmvs->minimunImageNumber());
                mCmvsPmvs->setUseVisibilityInformation(cmvs_pmvs->useVisibilityInformation());
            }
        }
    } else {
        TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
    }
}

void DensificationPresenterImp::configureSmvsProperties() const
{
    if (auto properties = mModel->densificationConfig()) {
        if (properties->name() == "SMVS") {
            auto smvs = dynamic_cast<SmvsProperties *>(properties.get());
            if (smvs && mSmvs) {
                mSmvs->setInputImageScale(smvs->inputImageScale());
                mSmvs->setOutputDepthScale(smvs->outputDepthScale());
                mSmvs->setSemiGlobalMatching(smvs->semiGlobalMatching());
                mSmvs->setSurfaceSmoothingFactor(smvs->surfaceSmoothingFactor());
                mSmvs->setShadingBasedOptimization(smvs->shadingBasedOptimization());
            }
        }
    } else {
        TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
    }
}

void DensificationPresenterImp::configureMvsProperties() const
{
    if (auto properties = mModel->densificationConfig()) {
        if (properties->name() == "MVS") {
            auto mvs = dynamic_cast<MvsProperties *>(properties.get());
            if (mvs && mMVS) {
                mMVS->setQuality(mvs->resolutionLevel());
                //mMVS->setResolutionLevel(mvs->resolutionLevel());
                //mMVS->setMinResolution(mvs->minResolution());
                //mMVS->setMaxResolution(mvs->maxResolution());
                mMVS->setNumberViews(mvs->numberViews());
                mMVS->setNumberViewsFuse(mvs->numberViewsFuse());
            }
        }
    } else {
        TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
    }
}

void DensificationPresenterImp::setCmvsPmvsWidget(std::shared_ptr<CmvsPmvsWidget> cmvsPmvs)
{
    mCmvsPmvs = cmvsPmvs;

    mView->addDensification(mCmvsPmvs.get());
    mView->setCurrentDensificationMethod(mCmvsPmvs->windowTitle());
}

void DensificationPresenterImp::setSmvsWidget(std::shared_ptr<SmvsWidget> smvs)
{
    mSmvs = smvs;

    mView->addDensification(mSmvs.get());
    mView->setCurrentDensificationMethod(mSmvs->windowTitle());
}

void DensificationPresenterImp::setMvsWidget(std::shared_ptr<MvsWidget> mvs)
{
    mMVS = mvs;

    mView->addDensification(mMVS.get());
    mView->setCurrentDensificationMethod(mMVS->windowTitle());
}

void DensificationPresenterImp::onDensificationChanged(const QString &densification)
{
    mView->setCurrentDensificationMethod(densification);
}

void DensificationPresenterImp::onError(tl::TaskErrorEvent &event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("DensificationComponent", "Task error"));
    }
}

void DensificationPresenterImp::onFinished(tl::TaskFinalizedEvent &event)
{
    if (auto *task = dynamic_cast<const DenseTaskBase *>(event.task())) {
        try {

            mModel->setDensificationReport(task->report());
            mModel->setDenseModel(task->denseModel());

        } catch (const std::exception &e) {
            tl::printException(e);
        }
    }

    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("DensificationComponent", "Task finished"));
    }

}

std::unique_ptr<tl::Task> DensificationPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> dense_task;

    if (mModel->existDenseModel()) {
        int i_ret = QMessageBox(QMessageBox::Warning,
                                QApplication::translate("DensificationComponent", "Previous results"),
                                QApplication::translate("DensificationComponent", "The previous results will be overwritten. Do you wish to continue?"),
                                QMessageBox::Yes | QMessageBox::No).exec();
        if (i_ret == QMessageBox::No) {
            tl::Message::warning("Process canceled by user");
            return dense_task;
        }
    }

    mModel->cleanProject();
    emit densification_deleted();

    //std::unordered_map<size_t, Image> images;
    //for (auto &image : mModel->images()) {
    //    auto it = mModel->poses().find(image.first);
    //    if (it != mModel->poses().end()) {
    //        images[image.first] = image.second;
    //    }
    //}

    QString densification_method = mView->currentDensificationMethod();

    tl::Path dense_path(mModel->projectFolder());
    dense_path.append("dense");

    if (densification_method.compare("CMVS/PMVS") == 0) {

        dense_path.append("pmvs");

        auto config = DensificationPropertiesFactory::create("PMVS");

        auto pmvs_properties = std::dynamic_pointer_cast<CmvsPmvsProperties>(config);
        pmvs_properties->setUseVisibilityInformation(mCmvsPmvs->useVisibilityInformation());
        pmvs_properties->setImagesPerCluster(mCmvsPmvs->imagesPerCluster());
        pmvs_properties->setLevel(mCmvsPmvs->level());
        pmvs_properties->setCellSize(mCmvsPmvs->cellSize());
        pmvs_properties->setThreshold(mCmvsPmvs->threshold());
        pmvs_properties->setWindowSize(mCmvsPmvs->windowSize());
        pmvs_properties->setMinimunImageNumber(mCmvsPmvs->minimunImageNumber());

        auto pmvs = std::make_unique<CmvsPmvsTask>(pmvs_properties,
                                                   mModel->images(),
                                                   mModel->cameras(),
                                                   mModel->cameraPoses(),
                                                   mModel->groundPoints(),
                                                   dense_path,
                                                   mModel->database(),
                                                   mModel->useCuda(),
                                                   mView->autoSegmentation());

        //pmvs->setUseVisibilityInformation(mCmvsPmvs->useVisibilityInformation());
        //pmvs->setImagesPerCluster(mCmvsPmvs->imagesPerCluster());
        //pmvs->setLevel(mCmvsPmvs->level());
        //pmvs->setCellSize(mCmvsPmvs->cellSize());
        //pmvs->setThreshold(mCmvsPmvs->threshold());
        //pmvs->setWindowSize(mCmvsPmvs->windowSize());
        //pmvs->setMinimunImageNumber(mCmvsPmvs->minimunImageNumber());

        //auto properties = std::make_shared<CmvsPmvs>(mCmvsPmvs->useVisibilityInformation(),
        //                                                       mCmvsPmvs->imagesPerCluster(),
        //                                                       mCmvsPmvs->level(),
        //                                                       mCmvsPmvs->cellSize(),
        //                                                       mCmvsPmvs->threshold(),
        //                                                       mCmvsPmvs->windowSize(),
        //                                                       mCmvsPmvs->minimunImageNumber());
        mModel->setDensificationConfig(config);

        dense_task = std::move(pmvs);

    } else if (densification_method.compare("Shading-Aware Multi-View Stereo") == 0) {

        dense_path.append("smvs");

        auto config = DensificationPropertiesFactory::create("SMVS");

        auto smvs_properties = std::dynamic_pointer_cast<SmvsProperties>(config);
        smvs_properties->setInputImageScale(mSmvs->inputImageScale());
        smvs_properties->setOutputDepthScale(mSmvs->outputDepthScale());
        smvs_properties->setShadingBasedOptimization(mSmvs->shadingBasedOptimization());
        smvs_properties->setSemiGlobalMatching(mSmvs->semiGlobalMatching());
        smvs_properties->setSurfaceSmoothingFactor(mSmvs->surfaceSmoothingFactor());

        auto smvs = std::make_unique<SmvsTask>(smvs_properties,
                                               mModel->images(),
                                               mModel->cameras(),
                                               mModel->cameraPoses(),
                                               mModel->groundPoints(),
                                               dense_path,
                                               mModel->useCuda(),
                                               mView->autoSegmentation());

        //smvs->setInputImageScale(mSmvs->inputImageScale());
        //smvs->setOutputDepthScale(mSmvs->outputDepthScale());
        //smvs->setShadingBasedOptimization(mSmvs->shadingBasedOptimization());
        //smvs->setSemiGlobalMatching(mSmvs->semiGlobalMatching());
        //smvs->setSurfaceSmoothingFactor(mSmvs->surfaceSmoothingFactor());
        //
        //auto properties = std::make_shared<Smvs>(mSmvs->inputImageScale(),
        //                                                   mSmvs->outputDepthScale(),
        //                                                   mSmvs->shadingBasedOptimization(),
        //                                                   mSmvs->semiGlobalMatching(),
        //                                                   mSmvs->surfaceSmoothingFactor());
        mModel->setDensificationConfig(config);

        dense_task = std::move(smvs);

    } else if (densification_method.compare("MVS") == 0) {

        dense_path.append("mvs");

        auto config = DensificationPropertiesFactory::create("MVS");

        auto mvs_properties = std::dynamic_pointer_cast<MvsProperties>(config);
        mvs_properties->setMaxResolution(3000/*mMVS->maxResolution()*/);
        mvs_properties->setMinResolution(256/*mMVS->minResolution()*/);
        mvs_properties->setNumberViews(mMVS->numberViews());
        mvs_properties->setNumberViewsFuse(mMVS->numberViewsFuse());
        mvs_properties->setResolutionLevel(mMVS->quality());
        mvs_properties->setEstimateColors(mMVS->estimateColors());
        mvs_properties->setEstimateNormals(mMVS->estimateNormals());

        auto mvs = std::make_unique<MvsTask>(mvs_properties,
                                             mModel->images(),
                                             mModel->cameras(),
                                             mModel->cameraPoses(),
                                             mModel->groundPoints(),
                                             dense_path,
                                             mModel->database(),
                                             mModel->useCuda(),
                                             mView->autoSegmentation());

        //mvs->setMaxResolution(3000/*mMVS->maxResolution()*/);
        //mvs->setMinResolution(256/*mMVS->minResolution()*/);
        //mvs->setNumberViews(mMVS->numberViews());
        //mvs->setNumberViewsFuse(mMVS->numberViewsFuse());
        //mvs->setResolutionLevel(mMVS->quality());
        //mvs->setEstimateColors(mMVS->estimateColors());
        //mvs->setEstimateNormals(mMVS->estimateNormals());

        //auto properties = std::make_shared<Mvs>(mMVS->quality(),
        //                                        256/*mMVS->minResolution()*/,
        //                                        3000/*mMVS->maxResolution()*/,
        //                                        mMVS->numberViews(),
        //                                        mMVS->numberViewsFuse());
        mModel->setDensificationConfig(config);

        dense_task = std::move(mvs);

    } else {
        mView->hide();
        throw std::runtime_error("Densification Method not valid");
    }

    //dense_task->subscribe([&](const tl::TaskFinalizedEvent *event) {

    //    auto report = dynamic_cast<DensifierBase const*>(event->task())->report();

    //    mModel->setDenseReport(report);

    //});

    if (progressHandler()) {
        progressHandler()->setRange(0, 1);
        progressHandler()->setTitle(QApplication::translate("DensificationComponent", "Densification"));
        progressHandler()->setDescription(QApplication::translate("DensificationComponent", "Generating dense model..."));
    }

    mView->hide();

    return dense_task;
}

} // End namespace graphos



