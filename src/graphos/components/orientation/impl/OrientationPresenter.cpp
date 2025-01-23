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

#include "OrientationPresenter.h"

#include "graphos/core/task/Progress.h"
#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/sfm/Reconstruction.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/components/orientation/OrientationModel.h"
#include "graphos/components/orientation/OrientationView.h"

#include <tidop/core/messages.h>
#include <tidop/core/task.h>

#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>


namespace graphos
{

OrientationPresenterImp::OrientationPresenterImp(OrientationView *view,
                                                 OrientationModel *model)
  : OrientationPresenter(),
    mView(view),
    mModel(model)
{
    OrientationPresenterImp::init();
    OrientationPresenterImp::initSignalAndSlots();
}

void OrientationPresenterImp::open()
{
    TL_TODO("mSettingsModel->refinePrincipalPoint();")

    mView->setCalibration(mModel->calibratedCamera());
    mView->enabledCalibration(mModel->calibratedCamera());

    if (mModel->hasCameraPoses() || mModel->hasGroundControlPoints()) {
        mView->enabledAbsoluteOrientation(true);
        mView->setAbsoluteOrientation(true);
    } else {
        mView->enabledAbsoluteOrientation(false);
        mView->setAbsoluteOrientation(false);
        mView->enabledRtkPositioningAccuracy(false);
        mView->setRtkPositioningAccuracy(false);
    }

    if (mModel->hasRtkPoses()) {
        mView->enabledRtkPositioningAccuracy(true);
        mView->setRtkPositioningAccuracy(true);
    } else {
        mView->enabledRtkPositioningAccuracy(false);
        mView->setRtkPositioningAccuracy(false);
    }

    mView->exec();
}

void OrientationPresenterImp::init()
{
}

void OrientationPresenterImp::initSignalAndSlots()
{
    connect(mView, &OrientationView::run, this, &OrientationPresenterImp::run);
    connect(mView, &DialogView::help, [&]() {
                emit help("orientation.html");
            });
}

void OrientationPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::info("Processing has been canceled by the user");
}

void OrientationPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("OrientationComponent", "Orientation task error"));
    }
}

void OrientationPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("OrientationComponent", "Orientation finished"));
    }

    if (mView->absoluteOrientation()) {

        tl::Path path = mModel->projectFolder();
        path.append("sfm");

        path.append("cameras.bin");
        tl::Path::removeFile(path);

        path.replaceBaseName("images");
        tl::Path::removeFile(path);

        path.replaceBaseName("points3D");
        tl::Path::removeFile(path);

    }

}

auto OrientationPresenterImp::createTask() -> std::unique_ptr<tl::Task>
{

    std::unique_ptr<tl::Task> orientation_task;

    if (mModel->existReconstruction()) {
        int i_ret = QMessageBox(QMessageBox::Warning,
                               QApplication::translate("OrientationComponent", "Previous results"),
                               QApplication::translate("OrientationComponent", "The previous results will be overwritten. Do you wish to continue?"),
                               QMessageBox::Yes | QMessageBox::No).exec();
        if (i_ret == QMessageBox::No) {
            return orientation_task;
        }
    }

    mModel->clearProject();
    emit orientation_deleted();

    TL_TODO("Establecer propiedades")

    std::vector<Image> images;
    for (const auto &image : mModel->images()) {
        images.push_back(image.second);
    }

    tl::Path sfm_path = mModel->projectFolder();
    sfm_path.append("sfm");

    orientation_task = std::make_unique<ReconstructionTask>(mModel->database(),
                                                            sfm_path,
                                                            images,
                                                            mModel->cameras(),
                                                            reconstructionOptions(),
                                                            mModel->groundControlPointsFile());

    orientation_task->subscribe([&](const tl::TaskFinalizedEvent *event) {

        try {

            auto task = dynamic_cast<ReconstructionTask const *>(event->task());
            auto cameras = task->cameras();
            auto report = task->report();

            /// Se comprueba que se han generado todos los productos
            tl::Path path = mModel->projectFolder();
            path.append("sfm");

            tl::Path sparse_model_path = path;
            sparse_model_path.append("sparse.ply");

            tl::Path ground_points_path = path;
            ground_points_path.append("ground_points.bin");

            tl::Path poses_path = path;
            poses_path.append("poses.bin");

            TL_ASSERT(sparse_model_path.exists(), "3D reconstruction fail");
            TL_ASSERT(ground_points_path.exists(), "3D reconstruction fail");
            TL_ASSERT(poses_path.exists(), "3D reconstruction fail");

            mModel->setSparseModel(sparse_model_path);
            mModel->setEnuCrs(QString::fromStdString(task->enuCrs()));
            mModel->setGroundPoints(ground_points_path);
            if (mView->absoluteOrientation())
                mModel->setEnuCrs(QString::fromStdString(task->enuCrs()));

            auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
            poses_reader->read(poses_path);
            auto poses = poses_reader->cameraPoses();

            for (const auto &camera_pose : poses) {
                mModel->addPhotoOrientation(camera_pose.first, camera_pose.second);
            }

            tl::Message::info("Oriented {} images", poses.size());

            double oriented_percent = (static_cast<double>(poses.size()) / static_cast<double>(mModel->images().size())) * 100.;
            if (oriented_percent < 90.) {
                // Menos del 90% de imagenes orientadas
                tl::Message::warning("{} percent of images oriented. Increase image size and number of points in Feature detector.", tl::roundToInteger(oriented_percent));
            }

            for (const auto &camera : cameras) {
                mModel->updateCamera(camera.first, camera.second);
            }

            report.orientedImages = static_cast<int>(poses.size());
            report.type = mView->absoluteOrientation() ? "Absolute" : "Relative";
            mModel->setOrientationReport(report);

        } catch (const std::exception &e) {
            tl::printException(e);
        }
    });

    if (progressHandler()) {
        progressHandler()->setRange(0, 1);
        progressHandler()->setTitle(QApplication::translate("OrientationComponent", "Orientation"));
        progressHandler()->setDescription(QApplication::translate("OrientationComponent", "Computing Orientation..."));
    }

    mView->hide();

    return orientation_task;
}

ReconstructionTask::Options OrientationPresenterImp::reconstructionOptions() const
{
    ReconstructionTask::Options options = {};

    if (mView->absoluteOrientation()) {

        options |= ReconstructionTask::Options::absolute_orientation;

        if (mModel->hasGroundControlPoints()) {
            options |= ReconstructionTask::Options::use_gcp;
        }

        if (mModel->hasCameraPoses()) {
            options |= ReconstructionTask::Options::use_poses;

            if (mView->rtkPositioningAccuracy()) {
                options |= ReconstructionTask::Options::use_rtk_positioning_accuracy;
            }
        }
    }

    if (mView->fixCalibration()) {
        options |= ReconstructionTask::Options::fix_calibration;
    }

    return options;
}

} // namespace graphos
