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

#include "graphos/core/process/Progress.h"
#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/components/orientation/OrientationModel.h"
#include "graphos/components/orientation/OrientationView.h"

#include <tidop/core/messages.h>
#include <tidop/core/task.h>

#include <QFileInfo>
#include <QMessageBox>

namespace graphos
{

OrientationPresenterImp::OrientationPresenterImp(OrientationView *view,
                                                 OrientationModel *model)
  : OrientationPresenter(),
    mView(view),
    mModel(model)
{
  this->init();
  this->initSignalAndSlots();
}

OrientationPresenterImp::~OrientationPresenterImp()
{
}

void OrientationPresenterImp::open()
{
  TL_TODO("mSettingsModel->refinePrincipalPoint();")

  mView->setCalibration(mModel->calibratedCamera());
  mView->enabledCalibration(mModel->calibratedCamera());

  if (mModel->rtkOrientations()) {
    mView->enabledAbsoluteOrientation(true);
    mView->setAbsoluteOrientation(true);
    mView->enabledPoses(true);
    mView->setPoses(true);
  } else if (mModel->gpsPositions()) {
    mView->enabledAbsoluteOrientation(true);
    mView->setAbsoluteOrientation(true);
    mView->enabledPoses(false);
    mView->setPoses(false);
  } else {
    mView->enabledAbsoluteOrientation(false);
    mView->setAbsoluteOrientation(false);
    mView->enabledPoses(false);
    mView->setPoses(false);
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
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

void OrientationPresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Orientation process error"));
  }
}

void OrientationPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Orientation finished"));
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

std::unique_ptr<tl::Task> OrientationPresenterImp::createProcess()
{

  std::unique_ptr<tl::Task> orientation_process;

  tl::Path reconstruction_path = mModel->reconstructionPath();
  if (!reconstruction_path.empty()) {
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes | QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      return orientation_process;
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

  if (mModel->rtkOrientations()) {

    orientation_process = std::make_unique<ImportPosesTask>(images,
                                                            mModel->cameras(),
                                                            sfm_path,
                                                            mModel->database(),
                                                            mView->fixCalibration(),
                                                            mView->fixPoses());

    orientation_process->subscribe([&](tl::TaskFinalizedEvent *event) {

      auto cameras = dynamic_cast<ImportPosesTask const *>(event->task())->cameras();

      tl::Path sfm_path = mModel->projectFolder();
      sfm_path.append("sfm");

      tl::Path offset_path = sfm_path;
      offset_path.append("offset.txt");

      tl::Path poses_path = sfm_path;
      poses_path.append("poses.bin");

      tl::Path sparse_model_path = sfm_path;
      sparse_model_path.append("sparse.ply");

      tl::Path ground_points_path = sfm_path;
      ground_points_path.append("ground_points.bin");

      TL_ASSERT(sparse_model_path.exists(), "3D reconstruction fail");
      TL_ASSERT(ground_points_path.exists(), "3D reconstruction fail");
      TL_ASSERT(poses_path.exists(), "3D reconstruction fail");

      mModel->setReconstructionPath(sfm_path);
      mModel->setSparseModel(sparse_model_path);
      mModel->setOffset(offset_path);
      mModel->setGroundPoints(ground_points_path);

      auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
      poses_reader->read(poses_path);
      auto poses = poses_reader->cameraPoses();

      for (const auto &camera_pose : poses) {
        mModel->addPhotoOrientation(camera_pose.first, camera_pose.second);
      }

      msgInfo("Oriented %i images", poses.size());

      for (const auto &camera : cameras) {
        mModel->updateCamera(camera.first, camera.second);
      }

    });

  } else {

    orientation_process = std::make_unique<tl::TaskList>();
    
    auto relative_orientation_task = std::make_shared<RelativeOrientationColmapTask>(mModel->database(),
                                                                                     sfm_path,
                                                                                     images,
                                                                                     mModel->cameras(),
                                                                                     mView->fixCalibration());

    relative_orientation_task->subscribe([&](tl::TaskFinalizedEvent *event) {

      try {

        auto cameras = dynamic_cast<RelativeOrientationColmapTask const *>(event->task())->cameras();

        /// Se comprueba que se han generado todos los productos
        tl::Path sfm_path = mModel->projectFolder();
        sfm_path.append("sfm");

        tl::Path sparse_model_path = sfm_path;
        sparse_model_path.append("sparse.ply");

        tl::Path ground_points_path = sfm_path;
        ground_points_path.append("ground_points.bin");

        tl::Path poses_path = sfm_path;
        poses_path.append("poses.bin");

        TL_ASSERT(sparse_model_path.exists(), "3D reconstruction fail");
        TL_ASSERT(ground_points_path.exists(), "3D reconstruction fail");
        TL_ASSERT(poses_path.exists(), "3D reconstruction fail");

        mModel->setReconstructionPath(sfm_path);
        mModel->setSparseModel(sparse_model_path);
        mModel->setOffset(tl::Path(""));
        mModel->setGroundPoints(ground_points_path);

        auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
        poses_reader->read(poses_path);
        auto poses = poses_reader->cameraPoses();

        for (const auto &camera_pose : poses) {
          mModel->addPhotoOrientation(camera_pose.first, camera_pose.second);
        }

        msgInfo("Oriented %i images", poses.size());

        for (const auto &camera : cameras) {
          mModel->updateCamera(camera.first, camera.second);
        }

      } catch (const std::exception &e) {
        tl::printException(e);
      }

    });

    dynamic_cast<tl::TaskList *>(orientation_process.get())->push_back(relative_orientation_task);

    if (mView->absoluteOrientation()) {
      
      auto absolute_orientation_task = std::make_shared<AbsoluteOrientationColmapTask>(sfm_path,
                                                                                       images);

      absolute_orientation_task->subscribe([&](tl::TaskFinalizedEvent *event) {

        tl::Path sfm_path = mModel->projectFolder();
        sfm_path.append("sfm");

        tl::Path offset_path = sfm_path;
        offset_path.append("offset.txt");
        tl::Path poses_path = sfm_path;
        poses_path.append("poses.bin");

        if (offset_path.exists()) {
          mModel->setOffset(offset_path);
        }

        if (poses_path.exists()) {
          auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
          poses_reader->read(poses_path);
          auto poses = poses_reader->cameraPoses();

          for (const auto &camera_pose : poses) {
            mModel->addPhotoOrientation(camera_pose.first, camera_pose.second);
          }
        }

      });

      dynamic_cast<tl::TaskList *>(orientation_process.get())->push_back(absolute_orientation_task);
    }
  }

  if (progressHandler()){
    progressHandler()->setRange(0, 1);
    progressHandler()->setTitle("Computing Orientation...");
    progressHandler()->setDescription("Computing Orientation...");
  }

  mView->hide();

  return orientation_process;
}


} // namespace graphos
