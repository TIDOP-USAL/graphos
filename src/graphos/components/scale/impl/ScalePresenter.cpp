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

#include "graphos/core/task/Progress.h"
#include "graphos/core/Application.h"
#include "graphos/widgets/Viewer3d.h"
#include "graphos/components/scale/impl/ScaleModel.h"
#include "graphos/components/scale/impl/ScaleView.h"
#include "graphos/components/scale/impl/ScaleTask.h"
#include "graphos/core/utils.h"

#include <ccPointCloud.h>
#include <ccGenericMesh.h>

#include <tidop/core/defs.h>

#include <QMessageBox>

namespace graphos
{

ScalePresenterImp::ScalePresenterImp(ScaleView *view,
                                     ScaleModel *model)
  : ScalePresenter(),
    mView(view),
    mModel(model),
    mPoints(0)
{
    this->init();
    this->initSignalAndSlots();
}

ScalePresenterImp::~ScalePresenterImp()
{

}

void ScalePresenterImp::measure(bool active)
{
    if (auto viewer_3d = dynamic_cast<Application *>(qApp)->viewer3D()) {

        if (auto ccviewer = dynamic_cast<CCViewer3D *>(viewer_3d)) {
            const QSignalBlocker blocker2(ccviewer);
            if (active) {
                connect(ccviewer, SIGNAL(mouseClicked(QVector3D)), this, SLOT(pointClicked(QVector3D)));
                ccviewer->activatePicker(CCViewer3D::PickingMode::distance);
            } else {
                disconnect(ccviewer, SIGNAL(mouseClicked(QVector3D)), this, SLOT(pointClicked(QVector3D)));
                ccviewer->deactivatePicker();
            }
        }

    } else if (active) {
        QMessageBox msgBox(QMessageBox::Warning,
                           "A 3D model is required for the measurement",
                           "Do you want to open the 3D model??",
                           QMessageBox::Yes | QMessageBox::No, mView);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            emit open_3d_model();
        }

    }
}

void ScalePresenterImp::pointClicked(const QVector3D &point)
{
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
    connect(mView, &DialogView::help, [&]() {
        emit help("Scale.html");
            });
}

void ScalePresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Process error"));
    }
}

void ScalePresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Process finished"));
    }
}

std::unique_ptr<tl::Task> ScalePresenterImp::createTask()
{
    std::unique_ptr<tl::Task> process;

    ccHObject *model = nullptr;

    if (auto viewer_3d = dynamic_cast<Application *>(qApp)->viewer3D()) {

        if (auto ccviewer = dynamic_cast<CCViewer3D *>(viewer_3d)) {
            ccHObject *root = ccviewer->object();
            ccHObject::Container clouds;
            // Nubes de puntos o malla
            root->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

            /// Sólo se permite una nube de puntos en el visor
            //TL_ASSERT(clouds.size() == 1, "Error");

            model = clouds.at(0);
        }
    }

    double scale = mView->distanceReal() / mView->distance();

    process = std::make_unique<ScaleTask>(scale, model);

    process->subscribe([&](tl::TaskFinalizedEvent *event) {

        try {

            auto transform = dynamic_cast<ScaleTask const *>(event->task())->transform();
            mModel->setTransform(transform);

        } catch (const std::exception &e) {
            tl::printException(e);
        }

    });

    if (progressHandler()) {
        progressHandler()->setRange(0, 0);
        progressHandler()->setTitle("Computing Scale...");
        progressHandler()->setDescription("Computing Scale...");
    }

    measure(false);

    mView->hide();

    return process;
}

void ScalePresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

} // namespace graphos
