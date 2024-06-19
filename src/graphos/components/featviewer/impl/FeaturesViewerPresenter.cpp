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

#include "FeaturesViewerPresenter.h"

#include <tidop/core/msg/message.h>

#include "graphos/components/featviewer/FeaturesViewerModel.h"
#include "graphos/components/featviewer/FeaturesViewerView.h"

#include <QFileInfo>

namespace graphos
{

FeaturesViewerPresenterImp::FeaturesViewerPresenterImp(FeaturesViewerView *view,
                                                       FeaturesViewerModel *model)
  : FeaturesViewerPresenter(),
    mView(view),
    mModel(model)
{
    FeaturesViewerPresenterImp::init();
    FeaturesViewerPresenterImp::initSignalAndSlots();
}

FeaturesViewerPresenterImp::~FeaturesViewerPresenterImp()
{

}

void FeaturesViewerPresenterImp::open()
{
    mView->clear();

    mView->setBackgroundColor(mModel->backgroundColor());
    mView->setSelectedMarkerStyle(mModel->selectedMarkerColor(),
                                  mModel->selectedMarkerWidth());
    mView->setMarkerStyle(mModel->markerColor(),
                          mModel->markerWidth(),
                          mModel->markerType(),
                          mModel->markerSize());

    mView->show();

    bool active_image = false;
    std::vector<std::pair<size_t, QString>> images;
    for (const auto &image : mModel->images()) {
        images.emplace_back(image.first, image.second.path());
    }

    if (!images.empty()) {
        mView->setImageList(images);
        setImageActive(images[0].first);
    }
}

void FeaturesViewerPresenterImp::init()
{
}

void FeaturesViewerPresenterImp::initSignalAndSlots()
{
    connect(mView, &FeaturesViewerView::image_changed,
            this, &FeaturesViewerPresenterImp::setImageActive);
    connect(mView, &DialogView::help, [&]() {
        emit help("keypoints_viewer.html");
    });
}

void FeaturesViewerPresenterImp::setImageActive(size_t imageId)
{
    auto image = mModel->image(imageId);
    mView->setCurrentImage(image.path());
    loadKeypoints(image.id());
}

void FeaturesViewerPresenterImp::loadKeypoints(size_t imageId)
{
    try {

        auto keypoints = mModel->loadKeypoints(imageId);
        mView->setKeyPoints(keypoints);

    } catch (const std::exception &e) {
        tl::printException(e);
    }
}

} // namespace graphos
