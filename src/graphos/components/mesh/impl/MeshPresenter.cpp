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


#include "MeshPresenter.h"

#include "graphos/core/mesh/PoissonRecon.h"
#include "graphos/core/task/Progress.h"
#include "graphos/components/mesh/impl/MeshModel.h"
#include "graphos/components/mesh/impl/MeshView.h"
#include "graphos/core/utils.h"

#include <tidop/core/defs.h>

namespace graphos
{

MeshPresenterImp::MeshPresenterImp(MeshView *view,
                                   MeshModel *model)
  : MeshPresenter(),
    mView(view),
    mModel(model)
{
  this->init();
  this->initSignalAndSlots();
}

MeshPresenterImp::~MeshPresenterImp()
{

}

void MeshPresenterImp::open()
{
  mModel->loadSettings();
  PoissonReconParameters *parameters = mModel->parameters();

  mView->setBoundaryType(parameters->boundaryType());
  mView->setDepth(parameters->depth());
  //mView->setFullDepth(parameters->fullDepth());
  mView->setSolveDepth(parameters->solveDepth());
  mView->setWidth(parameters->width());
  
  mView->exec();
}

void MeshPresenterImp::init()
{

}

void MeshPresenterImp::initSignalAndSlots()
{
  connect(mView, &TaskView::run,     this,   &TaskPresenter::run);
  connect(mView, &DialogView::help, [&]() {
    emit help("cameras.html");
  });
}

void MeshPresenterImp::onError(tl::TaskErrorEvent *event)
{
  TaskPresenter::onError(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Process error"));
  }
}

void MeshPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  TaskPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Process finished"));
  }

  tl::Path mesh = mModel->projectDir();
  mesh.append("dense").append("mesh.pr.ply");

  mModel->setMesh(mesh);
}

std::unique_ptr<tl::Task> MeshPresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> process;

  tl::Path point_cloud = mModel->denseModel();
  tl::Path mesh = mModel->projectDir();
  mesh.append("dense").append("mesh.pr.ply");

  auto parameters = mModel->parameters();
  parameters->setBoundaryType(mView->boundaryType());
  parameters->setDepth(mView->depth());
  //parameters->setFullDepth(mView->fullDepth());
  parameters->setSolveDepth(mView->solveDepth());
  parameters->setWidth(mView->width());

  process = std::make_unique<PoissonReconTask>(point_cloud,
                                               mesh);

  auto task_parameters = dynamic_cast<PoissonReconParameters *>(process.get());
  task_parameters->setBoundaryType(parameters->boundaryType());
  task_parameters->setDepth(parameters->depth());
  //task_parameters->setFullDepth(parameters->fullDepth());
  task_parameters->setSolveDepth(parameters->solveDepth());
  task_parameters->setWidth(parameters->width());

  if(progressHandler()) {
    progressHandler()->setRange(0, 1);
    progressHandler()->setTitle("Computing PoissonRecon...");
    progressHandler()->setDescription("Computing PoissonRecon...");
  }

  mView->hide();
  
  return process;
}

void MeshPresenterImp::cancel()
{
  TaskPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

} // namespace graphos
