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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "FeaturesViewerPresenter.h"

#include <tidop/core/messages.h>

#include "graphos/ui/featviewer/FeaturesViewerModel.h"
#include "graphos/ui/featviewer/FeaturesViewerView.h"
#include "graphos/ui/HelpDialog.h"

#include <QFileInfo>

namespace graphos
{

FeaturesViewerPresenterImp::FeaturesViewerPresenterImp(FeaturesViewerView *view,
                                                       FeaturesViewerModel *model)
  : FeaturesViewerPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

FeaturesViewerPresenterImp::~FeaturesViewerPresenterImp()
{

}

void FeaturesViewerPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("keypoints_viewer.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void FeaturesViewerPresenterImp::open()
{
  mView->clear();

  mView->setBGColor(mModel->viewerBGColor());
  mView->setSelectedMarkerStyle(mModel->viewerSelectMarkerColor(),
                                mModel->viewerSelectMarkerWidth());
  mView->setMarkerStyle(mModel->viewerMarkerColor(),
                        mModel->viewerMarkerWidth(),
                        mModel->viewerMarkerType(),
                        mModel->viewerMarkerSize());

  mView->show();

  std::vector<QString> images = mModel->images();
  if (images.empty() == false) {
    mView->setImageList(images);
    setImageActive(QFileInfo(images[0]).baseName());
  }
}

void FeaturesViewerPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void FeaturesViewerPresenterImp::init()
{
}

void FeaturesViewerPresenterImp::initSignalAndSlots()
{
  connect(mView, &FeaturesViewerView::imageChange, this, &FeaturesViewerPresenterImp::setImageActive);
  connect(mView, &DialogView::help,               this, &FeaturesViewerPresenterImp::help);
}

void FeaturesViewerPresenterImp::setImageActive(const QString &image)
{
  mView->setCurrentImage(image);
  loadKeypoints(image);
}

void FeaturesViewerPresenterImp::loadKeypoints(const QString &image)
{
  try	{
    std::vector<QPointF> keypoints = mModel->loadKeypoints(image);
    mView->setKeyPoints(keypoints);
  } catch (const std::exception &e) 	{
    msgError(e.what());
  }
}

} // namespace graphos
