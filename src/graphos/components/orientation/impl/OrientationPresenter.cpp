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
#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/posesio.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/components/orientation/OrientationModel.h"
#include "graphos/components/orientation/OrientationView.h"
#include "graphos/components/orientation/impl/RelativeOrientationProcess.h"
#include "graphos/components/orientation/impl/AbsoluteOrientationProcess.h"
#include "graphos/components/orientation/impl/ImportOrientationProcess.h"
#include "graphos/components/HelpDialog.h"

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
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

OrientationPresenterImp::~OrientationPresenterImp()
{
}

void OrientationPresenterImp::help()
{
  if (mHelp){
mHelp->setPage("orientation.html");
mHelp->setModal(true);
mHelp->showMaximized();
  }
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

void OrientationPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void OrientationPresenterImp::init()
{
}

void OrientationPresenterImp::initSignalAndSlots()
{
  connect(mView, &OrientationView::run, this, &OrientationPresenterImp::run);
  connect(mView, &OrientationView::help, this, &OrientationPresenterImp::help);
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

    bool fix_calibration = mView->fixCalibration();
  bool fix_poses = mView->fixPoses();
  QString database = mModel->database();

  if (mModel->rtkOrientations()) {

    std::vector<Image> images;
    for(const auto &image : mModel->images()){
      images.push_back(image.second);
    }

    orientation_process = std::make_unique<ImportOrientationProcess>(images,
                                                                     mModel->cameras(),
                                                                     mModel->projectPath(),
                                                                     mModel->database(),
                                                                     mView->fixCalibration(),
                                                                     mView->fixPoses());

    connect(dynamic_cast<ImportOrientationProcess *>(orientation_process.get()), &ImportOrientationProcess::importOrientationFinished,
            this, &OrientationPresenterImp::onAbsoluteOrientationFinished);

  } else {

    orientation_process = std::make_unique<tl::TaskList>();

    QString ori_relative_path = mModel->projectPath() + "/ori/relative/";

    std::shared_ptr<RelativeOrientationProcess> relativeOrientationProcess(new RelativeOrientationProcess(database,
                                                                                                          ori_relative_path,
                                                                                                          mView->fixCalibration()));

    connect(relativeOrientationProcess.get(), SIGNAL(orientation_finished()), this, SLOT(onRelativeOrientationFinished()));

    dynamic_cast<tl::TaskList *>(orientation_process.get())->push_back(relativeOrientationProcess);

    if (mView->absoluteOrientation()) {
      QString ori_absolute = mModel->projectPath() + "/ori/absolute/";
      std::map<QString, std::array<double, 3>> camera_positions = mModel->cameraPositions();
 
      std::shared_ptr<AbsoluteOrientationProcess> absoluteOrientationProcess(new AbsoluteOrientationProcess(ori_relative_path,
                                                                                                            camera_positions,
                                                                                                            ori_absolute));

      connect(absoluteOrientationProcess.get(), 
              SIGNAL(absoluteOrientationFinished()),
              this, 
              SLOT(onAbsoluteOrientationFinished()));

      dynamic_cast<tl::TaskList *>(orientation_process.get())->push_back(absoluteOrientationProcess);
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

void OrientationPresenterImp::onRelativeOrientationFinished()
{
  /// Se comprueba que se han generado todos los productos
  QString ori_relative_path = mModel->projectPath() + "/ori/relative/";
  QString sparse_model = ori_relative_path + "sparse.ply";

  if (QFileInfo(sparse_model).exists()){

    mModel->setReconstructionPath(ori_relative_path);
    mModel->setSparseModel(sparse_model);
    mModel->setOffset("");

    ReadCameraPoses readPhotoOrientations;
    readPhotoOrientations.open(ori_relative_path);
    int oriented_images = 0;

    for (const auto &image : mModel->images()) {
      QString image_oriented = QFileInfo(image.second.path()).fileName();
      CameraPose photoOrientation = readPhotoOrientations.orientation(image_oriented);
      if (photoOrientation.position() != tl::Point3D()) {
        mModel->addPhotoOrientation(image.first, photoOrientation);
        oriented_images++;
      } else {
        QByteArray ba = image_oriented.toLocal8Bit();
        const char *msg = ba.constData();
        msgWarning("Image %s not oriented", msg);
      }
    }

    msgInfo("Oriented %i images", oriented_images);

    ReadCalibration readCalibration;
    readCalibration.open(ori_relative_path);
    std::shared_ptr<Calibration> calibration;
    for(const auto &camera : mModel->cameras()){
      calibration = readCalibration.calibration(camera.first);
      if (calibration){
        Camera _camera = camera.second;
        _camera.setCalibration(calibration);
        mModel->updateCamera(camera.first, _camera);
      }
    }

    emit orientation_finished();
  }
}

void OrientationPresenterImp::onAbsoluteOrientationFinished()
{
  QString ori_absolute_path = mModel->projectPath() + "/ori/absolute/";
  QString sparse_model = ori_absolute_path + "/sparse.ply";
  if (QFileInfo::exists(sparse_model)){
    mModel->setReconstructionPath(ori_absolute_path);
    mModel->setSparseModel(ori_absolute_path + "/sparse.ply");
    mModel->setOffset(ori_absolute_path + "/offset.txt");

    ReadCameraPoses readPhotoOrientations;
    readPhotoOrientations.open(ori_absolute_path);
    for (const auto &image : mModel->images()){
      CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image.second.path()).fileName());
      if (photoOrientation.position() != tl::Point3D()) {
        mModel->addPhotoOrientation(image.first, photoOrientation);
      }
    }

    ReadCalibration readCalibration;
    readCalibration.open(ori_absolute_path);
    std::shared_ptr<Calibration> calibration;
    for (const auto &camera : mModel->cameras()) {
      calibration = readCalibration.calibration(camera.first);
      if (calibration) {
        Camera _camera = camera.second;
        _camera.setCalibration(calibration);
        mModel->updateCamera(camera.first, _camera);
      }
    }

    emit orientation_finished();

  } else {
    msgError("Orientation failed");
  }
}

} // namespace graphos
