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

void MatchViewerPresenterImp::setLeftImage(const QString &image)
{
  mView->setLeftImage(image);
  std::vector<QString> imagesRight = mModel->imagePairs(image);
  if (imagesRight.empty() == false){
    mView->setRightImageList(imagesRight);
    mView->setRightImage(QFileInfo(imagesRight[0]).baseName());
    loadMatches(image, QFileInfo(imagesRight[0]).baseName());
  }
}

void MatchViewerPresenterImp::setRightImage(const QString &image)
{
  mView->setRightImage(image);
}

void MatchViewerPresenterImp::loadMatches(const QString &imageLeft, const QString &imageRight)
{
  try {
    std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> matches = mModel->loadMatches(imageLeft, imageRight);
    mView->setMatches(matches);
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

  /// Se cargan las imágenes despues de mostrar el Dialog porque si se hace antes
  /// el QGraphicView no tiene el tamaño adecuado
  std::vector<QString> imagesLeft = mModel->images();
  if (imagesLeft.empty() == false) {
    mView->setLeftImageList(imagesLeft);
    setLeftImage(QFileInfo(imagesLeft[0]).baseName());
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
  connect(mView, SIGNAL(leftImageChange(QString)),         this, SLOT(setLeftImage(QString)));
  connect(mView, SIGNAL(rightImageChange(QString)),        this, SLOT(setRightImage(QString)));
  connect(mView, SIGNAL(loadMatches(QString, QString)),    this, SLOT(loadMatches(QString, QString)));

  connect(mView, SIGNAL(help()),     this, SLOT(help()));
}

} // namespace graphos
