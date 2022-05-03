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

#include <tidop/core/messages.h>

#include "graphos/components/matchviewer/impl/MatchViewerModel.h"
#include "graphos/components/matchviewer/impl/MatchViewerView.h"
#include "graphos/components/HelpDialog.h"

#include <QFileInfo>

namespace graphos
{

MatchViewerPresenterImp::MatchViewerPresenterImp(MatchViewerView *view,
                                                 MatchViewerModel *model)
  : MatchViewerPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
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

  if (!pairs.empty()){
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
    msgError(e.what());
  }
}

void MatchViewerPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("matches_viewer.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void MatchViewerPresenterImp::open()
{
  mView->clear();

  mView->setBGColor(mModel->viewerBGColor());
  mView->setSelectedMarkerStyle(mModel->viewerSelectMarkerColor(),
                                mModel->viewerSelectMarkerWidth());
  mView->setMarkerStyle(mModel->viewerMarkerColor(),
                        mModel->viewerMarkerWidth(),
                        mModel->viewerMarkerType(),
                        mModel->viewerMarkerSize());
  mView->setLineStyle(mModel->viewerLineColor(),
                      mModel->viewerLineWidth());

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

void MatchViewerPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void MatchViewerPresenterImp::init()
{
}

void MatchViewerPresenterImp::initSignalAndSlots()
{
  connect(mView, &MatchViewerView::leftImageChange,         this, &MatchViewerPresenterImp::setLeftImage);
  connect(mView, &MatchViewerView::rightImageChange,        this, &MatchViewerPresenterImp::setRightImage);
  connect(mView, &MatchViewerView::loadMatches,             this, &MatchViewerPresenterImp::loadMatches);

  connect(mView, &MatchViewerView::help,     this, &MatchViewerPresenterImp::help);
}

} // namespace graphos
