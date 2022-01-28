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

#include "CamerasPresenter.h"

#include "graphos/components/cameras/CamerasModel.h"
#include "graphos/components/cameras/CamerasView.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/core/image.h"

#include <tidop/core/messages.h>
#include <tidop/geospatial/camera.h>

namespace graphos
{

CamerasPresenterImp::CamerasPresenterImp(CamerasView *view,
                                         CamerasModel *model)
  : CamerasPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

void CamerasPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("cameras.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void CamerasPresenterImp::open()
{
  mView->clear();

  loadCameras();

  mView->exec();
}

void CamerasPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void CamerasPresenterImp::init()
{
}

void CamerasPresenterImp::initSignalAndSlots()
{
  connect(mView, &CamerasView::cameraChange, this, &CamerasPresenterImp::activeCamera);
  connect(mView, &CamerasView::calibrationImport, this, &CamerasPresenterImp::calibrationImport);
  connect(mView, &CamerasView::calibrationExport, mModel, &CamerasModel::calibrationExport);
  connect(mView, &CamerasView::fixCalibration, this, &CamerasPresenterImp::fixCalibration);

  connect(mView, &CamerasView::typeChange, mModel, &CamerasModel::updateCurrentCameraType);
  connect(mView, &CamerasView::makeChanged, mModel, &CamerasModel::updateCurrentCameraMake);
  connect(mView, &CamerasView::modelChanged, mModel, &CamerasModel::updateCurrentCameraModel);
  connect(mView, &CamerasView::sensorSizeChange, mModel, &CamerasModel::updateCurrentCameraSensorSize);
  connect(mView, &CamerasView::focalChange, mModel, &CamerasModel::updateCurrentCameraFocal);

  connect(mView, &CamerasView::calibCxChange, mModel, &CamerasModel::updateCurrentCameraCalibCx);
  connect(mView, &CamerasView::calibCyChange, mModel, &CamerasModel::updateCurrentCameraCalibCy);
  connect(mView, &CamerasView::calibFChange, mModel,  &CamerasModel::updateCurrentCameraCalibF);
  connect(mView, &CamerasView::calibFxChange, mModel, &CamerasModel::updateCurrentCameraCalibFx);
  connect(mView, &CamerasView::calibFyChange, mModel, &CamerasModel::updateCurrentCameraCalibFy);
  connect(mView, &CamerasView::calibK1Change, mModel, &CamerasModel::updateCurrentCameraCalibK1);
  connect(mView, &CamerasView::calibK2Change, mModel, &CamerasModel::updateCurrentCameraCalibK2);
  connect(mView, &CamerasView::calibK3Change, mModel, &CamerasModel::updateCurrentCameraCalibK3);
  connect(mView, &CamerasView::calibK4Change, mModel, &CamerasModel::updateCurrentCameraCalibK4);
  connect(mView, &CamerasView::calibK5Change, mModel, &CamerasModel::updateCurrentCameraCalibK5);
  connect(mView, &CamerasView::calibK6Change, mModel, &CamerasModel::updateCurrentCameraCalibK6);
  connect(mView, &CamerasView::calibP1Change, mModel, &CamerasModel::updateCurrentCameraCalibP1);
  connect(mView, &CamerasView::calibP2Change, mModel, &CamerasModel::updateCurrentCameraCalibP2);

  connect(mView, &QDialog::accepted, this, &CamerasPresenterImp::save);
  connect(mView, &QDialog::rejected, this, &CamerasPresenterImp::discart);
  connect(mView, &DialogView::help, this, &CamerasPresenterImp::help);
}

void CamerasPresenterImp::activeCamera(int id)
{
  try {

    mView->setActiveCamera(id);

    tl::Camera camera = mModel->camera(id);

    mView->setMake(camera.make().c_str());
    mView->setModel(camera.model().c_str());
    mView->setFocal(QString::number(camera.focal()));
    mView->setWidth(camera.width());
    mView->setHeight(camera.height());
    mView->setType(camera.type().c_str());
    mView->setSensorSize(QString::number(camera.sensorSize()));
    
    std::shared_ptr<tl::Calibration> calibration = camera.calibration();
    if (calibration) {
      for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++) {
        tl::Calibration::Parameters parameter = param->first;
        double value = param->second;
        switch (parameter) {
          case tl::Calibration::Parameters::focal:
            mView->setCalibF(value);
            break;
          case tl::Calibration::Parameters::focalx:
            mView->setCalibFx(value);
            break;
          case tl::Calibration::Parameters::focaly:
            mView->setCalibFy(value);
            break;
          case tl::Calibration::Parameters::cx:
            mView->setCalibCx(value);
            break;
          case tl::Calibration::Parameters::cy:
            mView->setCalibCy(value);
            break;
          case tl::Calibration::Parameters::k1:
            mView->setCalibK1(value);
            break;
          case tl::Calibration::Parameters::k2:
            mView->setCalibK2(value);
            break;
          case tl::Calibration::Parameters::k3:
            mView->setCalibK3(value);
            break;
          case tl::Calibration::Parameters::k4:
            mView->setCalibK4(value);
            break;
          case tl::Calibration::Parameters::k5:
            mView->setCalibK5(value);
            break;
          case tl::Calibration::Parameters::k6:
            mView->setCalibK6(value);
            break;
          case tl::Calibration::Parameters::p1:
            mView->setCalibP1(value);
            break;
          case tl::Calibration::Parameters::p2:
            mView->setCalibP2(value);
            break;
          default:
            break;
        }
      }
    }

    mView->setImages(mModel->imagesFromCamera(id));

  } catch (std::exception &e) {
    msgError(e.what());
  }
}

void CamerasPresenterImp::save()
{
  mModel->save();
  clear();
  emit updateCameras();
}

void CamerasPresenterImp::discart()
{
  clear();
}

void CamerasPresenterImp::clear()
{
  mView->clear();
  mModel->clear();
}

void CamerasPresenterImp::loadCameras()
{
  for(auto it = mModel->begin(); it != mModel->end(); it++) {
    int camera_id = it->first;
    tl::Camera camera = it->second;
    QString camera_name = QString(camera.make().c_str()).append("-").append(camera.model().c_str());
    mView->addCamera(camera_id, camera_name);
  }

  auto it = mModel->begin();
  if (it != mModel->end())
    activeCamera(it->first);
}

void CamerasPresenterImp::calibrationImport(const QString &file, 
                                            const QString &format)
{
  try {
    mModel->calibrationImport(file, format);
    int camera_id = mModel->currentCameraID();
    if (camera_id != 0)
      activeCamera(camera_id);
  } catch (const std::exception &e) 	{
    msgError(e.what());
  }
}

} // namespace graphos
