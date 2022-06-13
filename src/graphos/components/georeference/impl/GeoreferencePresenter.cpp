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

#include "GeoreferencePresenter.h"

#include "graphos/components/georeference/impl/GeoreferenceModel.h"
#include "graphos/components/georeference/impl/GeoreferenceView.h"
#include "graphos/components/georeference/impl/GeoreferenceProcess.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/core/process/Progress.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/image.h"

#include <tidop/core/defs.h>
#include <tidop/core/messages.h>

#include <QFileInfo>
#include <QAbstractItemModel>
#include <QStandardItemModel>

namespace graphos
{

GeoreferencePresenterImp::GeoreferencePresenterImp(GeoreferenceView *view,
                                                   GeoreferenceModel *model)
  : mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

GeoreferencePresenterImp::~GeoreferencePresenterImp()
{
}

void GeoreferencePresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Georeference process error"));
  }
}

void GeoreferencePresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  //QString ori_absolute_path = mModel->projectPath() + "/ori/absolute/";
  //QString sparse_model = ori_absolute_path + "/sparse.ply";
  //if (QFileInfo::exists(sparse_model)) {
  //  mModel->setReconstructionPath(ori_absolute_path);
  //  mModel->setSparseModel(sparse_model);
  //  mModel->setOffset(ori_absolute_path + "/offset.txt");

  //  ReadCameraPoses readPhotoOrientations;
  //  readPhotoOrientations.open(ori_absolute_path);
  //  for (const auto &image : mModel->images()) {
  //    CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image.second.path()).fileName());
  //    if (photoOrientation.position() != tl::Point3D()) {
  //      mModel->addPhotoOrientation(image.first, photoOrientation);
  //    }
  //  }

  //}

  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Georeference finished"));
  }
}

std::unique_ptr<tl::Task> GeoreferencePresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> georeference_process;

  //QString ori_relative = mModel->projectPath() + "/ori/relative/";
  //QString ori_absolute = mModel->projectPath() + "/ori/absolute/";

  georeference_process = std::make_unique<GeoreferenceProcess>(mModel->images(),
                                                               mModel->cameras(),
                                                               mModel->poses(),
                                                               mModel->groundPoints(),
                                                               mModel->groundControlPoints(), 
                                                               mModel->database());

  if (progressHandler()){
    progressHandler()->setRange(0, 0);
    progressHandler()->setTitle("Computing Georeference...");
    progressHandler()->setDescription("Computing Georeference...");
  }

  return georeference_process;
}

void GeoreferencePresenterImp::cancel()
{
  ProcessPresenter::cancel();
  
  msgWarning("Processing has been canceled by the user");
}

void GeoreferencePresenterImp::help()
{
  if (mHelp){
    TL_TODO("Añadir ayuda")
    mHelp->setPage("");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void GeoreferencePresenterImp::open()
{
  mView->clear();
  
  mModel->loadGroundControlPoints();
  mView->setCrs(mModel->crs());
  mView->showMaximized();
  
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

void GeoreferencePresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void GeoreferencePresenterImp::init()
{
  mView->setItemModelGroundControlPoints(mModel->itemModelGroundControlPoints());
  mView->setItemModelImagePoints(mModel->itemModelImagePoints());
}

void GeoreferencePresenterImp::initSignalAndSlots()
{
  connect(mView, &GeoreferenceView::image_changed, this, &GeoreferencePresenterImp::setImageActive);
  connect(mView, &GeoreferenceView::addGroundControlPoint, mModel, &GeoreferenceModel::addGroundControlPoint);
  connect(mView, &GeoreferenceView::accepted,     mModel, &GeoreferenceModel::save);
  connect(mView, &DialogView::help,                    this, &GeoreferencePresenterImp::help);
  connect(mView, &GeoreferenceView::removeGroundControlPoint, mModel, &GeoreferenceModel::removeGroundControlPoint);
  connect(mView, &GeoreferenceView::add_image_point, mModel, &GeoreferenceModel::addImagePoint);
  connect(mView, &GeoreferenceView::remove_image_point, mModel, &GeoreferenceModel::removeImagePoint);
  connect(mView, &GeoreferenceView::crsChange, mModel, &GeoreferenceModel::setCrs);
  connect(mView, &GeoreferenceView::georeference, this, &ProcessPresenter::run);
}

void GeoreferencePresenterImp::setImageActive(size_t imageId)
{
  auto image = mModel->image(imageId);
  mView->setCurrentImage(image.path());
  std::list<std::pair<QString, QPointF>> points = mModel->points(imageId);
  mView->setPoints(points);
}

} // namespace graphos
