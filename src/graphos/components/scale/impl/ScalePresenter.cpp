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


#include "ScalePresenter.h"

#include "graphos/framework/task/ProgressHandler.h"
#include "graphos/framework/app/Application.h"
#include "graphos/widgets/viewer3d/Easy3DViewer3D.h"
#include "graphos/components/scale/impl/ScaleModel.h"
#include "graphos/components/scale/impl/ScaleView.h"
#include "graphos/components/scale/impl/ScaleTask.h"
#include "graphos/core/utils.h"

#include <QMetaObject>

namespace graphos
{

ScalePresenterImp::ScalePresenterImp(ScaleView *view,
                                     ScaleModel *model)
  : ScalePresenter(),
    mView(view),
    mModel(model),
    mPoints(0)
{
    ScalePresenterImp::init();
    ScalePresenterImp::initSignalAndSlots();
}

ScalePresenterImp::~ScalePresenterImp()
{

}

void ScalePresenterImp::measure(bool active)
{
    if (active) {
        auto *viewer = dynamic_cast<Easy3DViewer3D *>(Application::instance().viewer3D());
        if (!viewer) {
            emit open_3d_model();
            return;
        }

        mViewer = viewer;
        disconnect(viewer, &Easy3DViewer3D::mouseClicked,
                   this, &ScalePresenterImp::pointClicked);
        connect(viewer, &Easy3DViewer3D::mouseClicked,
                this, &ScalePresenterImp::pointClicked);
        viewer->setPickingMode(Easy3DViewer3D::PickingMode::scale);
    } else if (auto *viewer = dynamic_cast<Easy3DViewer3D *>(mViewer.data())) {
        disconnect(viewer, &Easy3DViewer3D::mouseClicked,
                   this, &ScalePresenterImp::pointClicked);
        if (viewer->pickingMode() == Easy3DViewer3D::PickingMode::scale)
            viewer->setPickingMode(Easy3DViewer3D::PickingMode::none);
    }

    if (!active) {
        mPoints.clear();
        mView->setMeasureEnabled(false);
    }
}

void ScalePresenterImp::pointClicked(const QVector3D &point)
{
    auto *viewer = dynamic_cast<Easy3DViewer3D *>(mViewer.data());
    if (!viewer || viewer->pickingMode() != Easy3DViewer3D::PickingMode::scale)
        return;

    if (mPoints.empty()) {
        mPoints.push_back(point);
    } else {
        mPoints.push_back(point);
        double distante = mPoints.at(0).distanceToPoint(mPoints.at(1));
        mView->setDistance(distante);
        mPoints.resize(0);
    }
}

void ScalePresenterImp::open()
{
    mModel->loadSettings();
    mPoints.resize(0);
    mView->setMeasureEnabled(false);

    /* Configure View here */

    mView->show();
}

void ScalePresenterImp::init()
{

}

void ScalePresenterImp::initSignalAndSlots()
{
    connect(mView, &TaskView::run, this, &TaskPresenter::run);
    connect(mView, &ScaleView::enableMeasure, this, &ScalePresenterImp::measure);
    connect(mView, &QDialog::rejected, this, [this] {
        measure(false);
    });
    connect(mView, &DialogView::help, 
        [&]() {
            emit help("Scale.html");
        });
}

void ScalePresenterImp::onError(tl::TaskErrorEvent &event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("ScaleComponent", "Task error"));
    }
}

void ScalePresenterImp::onFinished(tl::TaskFinalizedEvent &event)
{
    TaskPresenter::onFinished(event);

    if (auto *task = dynamic_cast<const ScaleTask *>(event.task())) {
        const auto transform = task->transform();
        const double scale = transform[0][0];
        const QPointer<QObject> viewer = mViewer;
        QMetaObject::invokeMethod(this, [this, viewer, transform, scale] {
            mModel->setTransform(transform);
            if (auto *easy3d_viewer = dynamic_cast<Easy3DViewer3D *>(viewer.data()))
                easy3d_viewer->applyScale(scale);
        }, Qt::QueuedConnection);
    }

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("ScaleComponent", "Task finished"));
    }
}

auto ScalePresenterImp::createTask() -> std::unique_ptr<tl::Task>
{
    double scale = mView->distanceReal() / mView->distance();

    std::unique_ptr<tl::Task> task = std::make_unique<ScaleTask>(scale);

    if (progressHandler()) {
        progressHandler()->setRange(0, 0);
        progressHandler()->setTitle(QApplication::translate("ScaleComponent", "Model scaling"));
        progressHandler()->setDescription(QApplication::translate("ScaleComponent", "Scaling the model..."));
        progressHandler()->closeAuto(true);
    }

    measure(false);

    mView->hide();

    return task;
}

void ScalePresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Task has been canceled by the user");
}

} // namespace graphos
