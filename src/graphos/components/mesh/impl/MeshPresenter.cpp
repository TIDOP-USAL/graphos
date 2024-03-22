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
    MeshPresenterImp::init();
    MeshPresenterImp::initSignalAndSlots();
}

MeshPresenterImp::~MeshPresenterImp()
{

}

void MeshPresenterImp::open()
{
    mModel->loadSettings();
    auto properties = mModel->properties();
    if (properties == nullptr) {
        properties = std::make_shared<PoissonReconProperties>();
    }

    mView->setBoundaryType(properties->boundaryTypeAsText());
    mView->setDepth(properties->depth());
    mView->setSolveDepth(properties->solveDepth());

    mView->exec();
}

void MeshPresenterImp::init()
{

}

void MeshPresenterImp::initSignalAndSlots()
{
    connect(mView, &TaskView::run, this, &TaskPresenter::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("mesh.html");
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

std::unique_ptr<tl::Task> MeshPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> mesh_task;

    tl::Path point_cloud = mModel->denseModel();
    tl::Path mesh = mModel->projectDir();
    mesh.append("dense").append("mesh.pr.ply");

    auto properties = mModel->properties();
    if (properties == nullptr) {
        properties = std::make_shared<PoissonReconProperties>();
    }

    QString bt = mView->boundaryType();
    PoissonReconProperties::BoundaryType boundary_type;
    if (bt == "Free") {
        boundary_type = PoissonReconProperties::BoundaryType::free;
    } else if (bt == "Dirichlet") {
        boundary_type = PoissonReconProperties::BoundaryType::dirichlet;
    } else {
        boundary_type = PoissonReconProperties::BoundaryType::neumann;
    }
    properties->setBoundaryType(boundary_type);
    properties->setDepth(mView->depth());
    properties->setSolveDepth(mView->solveDepth());
    mModel->setProperties(properties);

    mesh_task = std::make_unique<PoissonReconTask>(point_cloud,
                                                   mesh);

    auto task_parameters = dynamic_cast<PoissonReconProperties *>(mesh_task.get());
    task_parameters->setBoundaryType(properties->boundaryType());
    task_parameters->setDepth(properties->depth());
    task_parameters->setSolveDepth(properties->solveDepth());

    mesh_task->subscribe([&](tl::TaskFinalizedEvent* event) {

        auto report = dynamic_cast<PoissonReconTask const*>(event->task())->report();

        mModel->setMeshReport(report);

    });

    if (progressHandler()) {
        progressHandler()->setRange(0, 1);
        progressHandler()->setTitle("Poisson surface reconstruction");
        progressHandler()->setDescription("Computing Poisson Reconstruction");
    }

    mView->hide();

    return mesh_task;
}

void MeshPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

} // namespace graphos
