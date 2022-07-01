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
}

std::unique_ptr<tl::Task> OrientationPresenterImp::createProcess()
{

  std::unique_ptr<tl::Task> orientation_process;

  QString reconstruction_path = mModel->reconstructionPath();
  if (!reconstruction_path.isEmpty()) {
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

  if (mModel->rtkOrientations()) {



    orientation_process = std::make_unique<ImportOrientationTask>(images,
                                                                  mModel->cameras(),
                                                                  mModel->projectPath() + "/sfm",
                                                                  mModel->database(),
                                                                  mView->fixCalibration(),
                                                                  mView->fixPoses());

    orientation_process->subscribe([&](tl::TaskFinalizedEvent *event) {

      auto cameras = dynamic_cast<ImportOrientationTask const *>(event->task())->cameras();

      tl::Path sfm_path = mModel->projectPath().toStdWString();
      sfm_path.append("sfm");

      tl::Path offset_path = sfm_path;
      offset_path.append("offset.txt");

      tl::Path poses_path = sfm_path;
      poses_path.append("poses.bin");

      tl::Path sparse_model_path = sfm_path;
      sparse_model_path.append("sparse.ply");

      tl::Path ground_points_path = sfm_path;
      ground_points_path.append("ground_points.bin");

      if (sparse_model_path.exists() &&
          ground_points_path.exists() &&
          poses_path.exists()) {

        mModel->setReconstructionPath(QString::fromStdWString(sfm_path.toWString()));
        mModel->setSparseModel(QString::fromStdWString(sparse_model_path.toWString()));
        mModel->setOffset(QString::fromStdWString(offset_path.toWString()));
      }

      if (poses_path.exists()) {
        auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
        poses_reader->read(poses_path);
        auto poses = poses_reader->cameraPoses();

        for (const auto &camera_pose : poses) {
          mModel->addPhotoOrientation(camera_pose.first, camera_pose.second);
        }

        msgInfo("Oriented %i images", poses.size());

      }

      for (const auto &camera : cameras) {
        mModel->updateCamera(camera.first, camera.second);
      }

    });

  } else {

    orientation_process = std::make_unique<tl::TaskList>();

    QString ori_relative_path = mModel->projectPath() + "/sfm/";
    
    auto relative_orientation_task = std::make_shared<RelativeOrientationColmapTask>(mModel->database(),
                                                                                     ori_relative_path,
                                                                                     images,
                                                                                     mModel->cameras(),
                                                                                     mView->fixCalibration());

    relative_orientation_task->subscribe([&](tl::TaskFinalizedEvent *event) {

      auto cameras = dynamic_cast<RelativeOrientationColmapTask const*>(event->task())->cameras();
      /// Se comprueba que se han generado todos los productos
      tl::Path sfm_path = mModel->projectPath().toStdWString();// +"/sfm";
      sfm_path.append("sfm");

      tl::Path sparse_model_path = sfm_path;
      sparse_model_path.append("sparse.ply");

      tl::Path ground_points_path = sfm_path;
      ground_points_path.append("ground_points.bin");

      tl::Path poses_path = sfm_path;
      poses_path.append("poses.bin");

      if (sparse_model_path.exists() && 
          ground_points_path.exists() &&
          poses_path.exists()) {

        mModel->setReconstructionPath(QString::fromStdWString(sfm_path.toWString()));
        mModel->setSparseModel(QString::fromStdWString(sparse_model_path.toWString()));
        mModel->setOffset("");

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
      }

    });

    dynamic_cast<tl::TaskList *>(orientation_process.get())->push_back(relative_orientation_task);

    if (mView->absoluteOrientation()) {
      
      auto absolute_orientation_task = std::make_shared<AbsoluteOrientationColmapTask>(ori_relative_path,
                                                                                       images);

      absolute_orientation_task->subscribe([&](tl::TaskFinalizedEvent *event) {

        tl::Path sfm_path = mModel->projectPath().toStdWString();
        sfm_path.append("sfm");
        tl::Path offset_path = sfm_path;
        offset_path.append("offset.txt");
        tl::Path poses_path = sfm_path;
        poses_path.append("poses.bin");

        if (offset_path.exists()) {
          mModel->setOffset(QString::fromStdWString(offset_path.toWString()));
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

void OrientationPresenterImp::onAbsoluteOrientationFinished()
{
  //QString ori_absolute_path = mModel->projectPath() + "/ori/absolute/";
  //QString sparse_model = ori_absolute_path + "/sparse.ply";
  //if (QFileInfo::exists(sparse_model)){
  //  mModel->setReconstructionPath(ori_absolute_path);
  //  mModel->setSparseModel(ori_absolute_path + "/sparse.ply");
  //  mModel->setOffset(ori_absolute_path + "/offset.txt");

  //  ReadCameraPoses readPhotoOrientations;
  //  readPhotoOrientations.open(ori_absolute_path);
  //  for (const auto &image : mModel->images()){
  //    CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image.second.path()).fileName());
  //    if (photoOrientation.position() != tl::Point3D()) {
  //      mModel->addPhotoOrientation(image.first, photoOrientation);
  //    }
  //  }

  //  ReadCalibration readCalibration;
  //  readCalibration.open(ori_absolute_path);
  //  std::shared_ptr<Calibration> calibration;
  //  for (const auto &camera : mModel->cameras()) {
  //    calibration = readCalibration.calibration(camera.first);
  //    if (calibration) {
  //      Camera _camera = camera.second;
  //      _camera.setCalibration(calibration);
  //      mModel->updateCamera(camera.first, _camera);
  //    }
  //  }

  //  //emit orientation_finished();

  //} else {
  //  msgError("Orientation failed");
  //}
}

} // namespace graphos
