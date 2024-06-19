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

#include "MatchViewerPresenter.h"

#include "graphos/components/matchviewer/impl/MatchViewerModel.h"
#include "graphos/components/matchviewer/impl/MatchViewerView.h"

#include <QFileInfo>

namespace graphos
{

MatchViewerPresenterImp::MatchViewerPresenterImp(MatchViewerView *view,
                                                 MatchViewerModel *model)
  : MatchViewerPresenter(),
    mView(view),
    mModel(model)
{
    MatchViewerPresenterImp::init();
    MatchViewerPresenterImp::initSignalAndSlots();
}

MatchViewerPresenterImp::~MatchViewerPresenterImp()
{

}

void MatchViewerPresenterImp::setLeftImage(size_t imageId)
{
    Image image = mModel->image(imageId);
    mView->setLeftImage(image.path());

    std::vector<std::pair<size_t, QString>> pairs;
    for (const auto &pair_id : mModel->imagePairs(imageId)) {
        pairs.emplace_back(pair_id, mModel->image(pair_id).path());
    }

    if (!pairs.empty()) {
        mView->setRightImageList(pairs);
        mView->setRightImage(pairs[0].second);
        loadMatches(imageId, pairs[0].first);
    }
}

void MatchViewerPresenterImp::setRightImage(size_t imageId)
{
    mView->setRightImage(mModel->image(imageId).path());
}

void MatchViewerPresenterImp::loadMatches(size_t imageId1,
                                          size_t imageId2)
{
    try {

        mView->setMatches(mModel->loadMatches(imageId1, imageId2));

    } catch (const std::exception &e) {
        tl::printException(e);
    }
}

void MatchViewerPresenterImp::open()
{
    mView->clear();

    mView->setBackgroundColor(mModel->backgroundColor());
    mView->setSelectedMarkerStyle(mModel->selectedMarkerColor(),
                                  mModel->viewerSelectMarkerWidth());
    mView->setMarkerStyle(mModel->markerColor(),
                          mModel->markerWidth(),
                          mModel->markerType(),
                          mModel->markerSize());
    mView->setLineStyle(mModel->lineColor(),
                        mModel->lineWidth());

    mView->show();

    std::vector<std::pair<size_t, QString>> images;
    for (const auto &pair_id_image : mModel->images()) {
        images.emplace_back(pair_id_image.first, pair_id_image.second.path());
    }

    if (!images.empty()) {
        mView->setLeftImageList(images);
        setLeftImage(images[0].first);
    }

}

void MatchViewerPresenterImp::init()
{
}

void MatchViewerPresenterImp::initSignalAndSlots()
{
    connect(mView, &MatchViewerView::leftImageChange, this, &MatchViewerPresenterImp::setLeftImage);
    connect(mView, &MatchViewerView::rightImageChange, this, &MatchViewerPresenterImp::setRightImage);
    connect(mView, &MatchViewerView::loadMatches, this, &MatchViewerPresenterImp::loadMatches);

    connect(mView, &DialogView::help, [&]() {
        emit help("matches_viewer.html");
    });
}

} // namespace graphos
