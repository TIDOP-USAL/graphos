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
#include "graphos/core/dense/CmvsPmvs.h"
#include "graphos/core/dense/Smvs.h"
#include "graphos/core/dense/mvs.h"
#include "graphos/core/task/Progress.h"
#include "graphos/widgets/CmvsPmvsWidget.h"
#include "graphos/widgets/SmvsWidget.h"
#include "graphos/widgets/MvsWidget.h"

#include <tidop/core/msg/message.h>

#include <QMessageBox>
#include <QFileInfo>

#include <memory>


namespace graphos
{

DensificationPresenterImp::DensificationPresenterImp(DensificationView *view,
                                                     DensificationModel *model)
  : mView(view),
    mModel(model),
    mCmvsPmvs(new CmvsPmvsWidget),
    mSmvs(new SmvsWidget),
    mMVS(new MvsWidget)
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

    tl::Message::warning("Processing has been canceled by the user");
}

void DensificationPresenterImp::setCurrentDensifier(const QString &densifier)
{
    mView->setCurrentDensificationMethod(densifier);
}


void DensificationPresenterImp::configureCmvsPmvsProperties() const
{
    CmvsPmvs *cmvs_pmvs = nullptr;
    if (std::shared_ptr<Densification> densification = mModel->densification()) {
        if (densification->method() == Densification::Method::cmvs_pmvs) {
            cmvs_pmvs = dynamic_cast<CmvsPmvs*>(densification.get());
        }
    } else {
        TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
    }

    if (cmvs_pmvs) {
        mCmvsPmvs->setLevel(cmvs_pmvs->level());
        mCmvsPmvs->setCellSize(cmvs_pmvs->cellSize());
        mCmvsPmvs->setThreshold(cmvs_pmvs->threshold());
        mCmvsPmvs->setWindowSize(cmvs_pmvs->windowSize());
        mCmvsPmvs->setImagesPerCluster(cmvs_pmvs->imagesPerCluster());
        mCmvsPmvs->setMinimunImageNumber(cmvs_pmvs->minimunImageNumber());
        mCmvsPmvs->setUseVisibilityInformation(cmvs_pmvs->useVisibilityInformation());
    }
}

void DensificationPresenterImp::configureSmvsProperties() const
{
    Smvs *smvs = nullptr;
    if (std::shared_ptr<Densification> densification = mModel->densification()) {
        if (densification->method() == Densification::Method::smvs) {
            smvs = dynamic_cast<Smvs *>(densification.get());
        }
    } else {
        TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
    }

    if (smvs) {
        mSmvs->setInputImageScale(smvs->inputImageScale());
        mSmvs->setOutputDepthScale(smvs->outputDepthScale());
        mSmvs->setSemiGlobalMatching(smvs->semiGlobalMatching());
        mSmvs->setSurfaceSmoothingFactor(smvs->surfaceSmoothingFactor());
        mSmvs->setShadingBasedOptimization(smvs->shadingBasedOptimization());
    }
}

void DensificationPresenterImp::configureMvsProperties() const
{
    Mvs *mvs = nullptr;
    if (std::shared_ptr<Densification> densification = mModel->densification()) {
        if (densification->method() == Densification::Method::mvs) {
            mvs = dynamic_cast<Mvs *>(densification.get());
        }
    } else {
        TL_TODO("std::shared_ptr<Densification> densification = mSettingsModel->densification();")
    }

    if (mvs) {
        mMVS->setQuality(mvs->resolutionLevel());
        //mMVS->setResolutionLevel(mvs->resolutionLevel());
        //mMVS->setMinResolution(mvs->minResolution());
        //mMVS->setMaxResolution(mvs->maxResolution());
        mMVS->setNumberViews(mvs->numberViews());
        mMVS->setNumberViewsFuse(mvs->numberViewsFuse());
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
    mView->setCurrentDensificationMethod(mCmvsPmvs->windowTitle());
}

void DensificationPresenterImp::setMvsWidget(std::shared_ptr<MvsWidget> mvs)
{
    mMVS = mvs;

    mView->addDensification(mMVS.get());
    mView->setCurrentDensificationMethod(mCmvsPmvs->windowTitle());
}

void DensificationPresenterImp::onDensificationChanged(const QString &densification)
{
    mView->setCurrentDensificationMethod(densification);
}

void DensificationPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Densification error"));
    }
}

void DensificationPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{

    tl::Path dense_path = dynamic_cast<DensifierBase const *>(event->task())->denseModel();

    mModel->setDenseModel(dense_path);

    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Densification finished"));
    }

}

std::unique_ptr<tl::Task> DensificationPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> dense_task;

    if (mModel->existDenseModel()) {
        int i_ret = QMessageBox(QMessageBox::Warning,
                                tr("Previous results"),
                                tr("The previous results will be overwritten. Do you wish to continue?"),
                                QMessageBox::Yes | QMessageBox::No).exec();
        if (i_ret == QMessageBox::No) {
            tl::Message::warning("Process canceled by user");
            return dense_task;
        }
    }


    std::unordered_map<size_t, Image> images;
    for (auto &image : mModel->images()) {
        auto it = mModel->poses().find(image.first);
        if (it != mModel->poses().end()) {
            images[image.first] = image.second;
        }
    }

    QString densification_method = mView->currentDensificationMethod();

    tl::Path dense_path(mModel->projectFolder());
    dense_path.append("dense");

    if (densification_method.compare("CMVS/PMVS") == 0) {

        dense_path.append("pmvs");

        auto pmvs = std::make_unique<CmvsPmvsDensifier>(images,
                                                        mModel->cameras(),
                                                        mModel->poses(),
                                                        mModel->groundPoints(),
                                                        dense_path,
                                                        mModel->database(),
                                                        mModel->useCuda(),
                                                        mView->autoSegmentation());

        pmvs->setUseVisibilityInformation(mCmvsPmvs->useVisibilityInformation());
        pmvs->setImagesPerCluster(mCmvsPmvs->imagesPerCluster());
        pmvs->setLevel(mCmvsPmvs->level());
        pmvs->setCellSize(mCmvsPmvs->cellSize());
        pmvs->setThreshold(mCmvsPmvs->threshold());
        pmvs->setWindowSize(mCmvsPmvs->windowSize());
        pmvs->setMinimunImageNumber(mCmvsPmvs->minimunImageNumber());

        auto properties = std::make_shared<CmvsPmvs>(mCmvsPmvs->useVisibilityInformation(),
                                                               mCmvsPmvs->imagesPerCluster(),
                                                               mCmvsPmvs->level(),
                                                               mCmvsPmvs->cellSize(),
                                                               mCmvsPmvs->threshold(),
                                                               mCmvsPmvs->windowSize(),
                                                               mCmvsPmvs->minimunImageNumber());
        mModel->setDensification(properties);

        dense_task = std::move(pmvs);

    } else if (densification_method.compare("Shading-Aware Multi-View Stereo") == 0) {

        dense_path.append("smvs");

        auto smvs = std::make_unique<SmvsDensifier>(images,
                                                    mModel->cameras(),
                                                    mModel->poses(),
                                                    mModel->groundPoints(),
                                                    dense_path,
                                                    mModel->useCuda(),
                                                    mView->autoSegmentation());

        smvs->setInputImageScale(mSmvs->inputImageScale());
        smvs->setOutputDepthScale(mSmvs->outputDepthScale());
        smvs->setShadingBasedOptimization(mSmvs->shadingBasedOptimization());
        smvs->setSemiGlobalMatching(mSmvs->semiGlobalMatching());
        smvs->setSurfaceSmoothingFactor(mSmvs->surfaceSmoothingFactor());
        
        auto properties = std::make_shared<Smvs>(mSmvs->inputImageScale(),
                                                           mSmvs->outputDepthScale(),
                                                           mSmvs->shadingBasedOptimization(),
                                                           mSmvs->semiGlobalMatching(),
                                                           mSmvs->surfaceSmoothingFactor());
        mModel->setDensification(properties);

        dense_task = std::move(smvs);

    } else if (densification_method.compare("MVS") == 0) {

        dense_path.append("mvs");

        auto mvs = std::make_unique<MvsDensifier>(images,
                                                  mModel->cameras(),
                                                  mModel->poses(),
                                                  mModel->groundPoints(),
                                                  dense_path,
                                                  mModel->database(),
                                                  mModel->useCuda(),
                                                  mView->autoSegmentation());

        mvs->setMaxResolution(3000/*mMVS->maxResolution()*/);
        mvs->setMinResolution(256/*mMVS->minResolution()*/);
        mvs->setNumberViews(mMVS->numberViews());
        mvs->setNumberViewsFuse(mMVS->numberViewsFuse());
        mvs->setResolutionLevel(mMVS->quality());
        mvs->setEstimateColors(mMVS->estimateColors());
        mvs->setEstimateNormals(mMVS->estimateNormals());

        auto properties = std::make_shared<Mvs>(mMVS->quality(),
                                                256/*mMVS->minResolution()*/,
                                                3000/*mMVS->maxResolution()*/,
                                                mMVS->numberViews(),
                                                mMVS->numberViewsFuse());
        mModel->setDensification(properties);

        dense_task = std::move(mvs);

    } else {
        mView->hide();
        throw std::runtime_error("Densification Method not valid");
    }

    dense_task->subscribe([&](tl::TaskFinalizedEvent *event) {

        auto report = dynamic_cast<DensifierBase const*>(event->task())->report();

        mModel->setDenseReport(report);

    });

    if (progressHandler()) {
        progressHandler()->setRange(0, 1);
        progressHandler()->setTitle("Generating dense model...");
        progressHandler()->setDescription("Generating dense model...");
    }

    mView->hide();

    return dense_task;
}

} // End namespace graphos



