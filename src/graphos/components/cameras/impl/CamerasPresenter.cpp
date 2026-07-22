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

#include "CamerasPresenter.h"

#include "graphos/components/cameras/CamerasModel.h"
#include "graphos/components/cameras/CamerasView.h"
#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

#include <tidop/core/app/Message.h>

namespace graphos
{

CamerasPresenterImp::CamerasPresenterImp(CamerasView *view,
                                         CamerasModel *model)
  : CamerasPresenter(),
    mView(view),
    mModel(model)
{
    CamerasPresenterImp::init();
    CamerasPresenterImp::initSignalAndSlots();
}

void CamerasPresenterImp::open()
{
    mView->clear();

    loadCameras();

    mView->exec();
}

void CamerasPresenterImp::init()
{
}

void CamerasPresenterImp::initSignalAndSlots()
{
    connect(mView, &CamerasView::cameraChange, this, &CamerasPresenterImp::activeCamera);
    connect(mView, &CamerasView::priorCalibrationImport, this, &CamerasPresenterImp::priorCalibrationImport);
    connect(mView, &CamerasView::priorCalibrationExport, mModel, &CamerasModel::priorCalibrationExport);
    connect(mView, &CamerasView::adjustCalibrationExport, mModel, &CamerasModel::adjustCalibrationExport);
    //connect(mView, &CamerasView::fixCalibration, this, &CamerasPresenterImp::fixCalibration);

    connect(mView, &CamerasView::typeChange, mModel, &CamerasModel::updateCurrentCameraType);
    connect(mView, &CamerasView::makeChanged, mModel, &CamerasModel::updateCurrentCameraMake);
    connect(mView, &CamerasView::modelChanged, mModel, &CamerasModel::updateCurrentCameraModel);
    connect(mView, &CamerasView::sensorSizeChange, mModel, &CamerasModel::updateCurrentCameraSensorSize);
    connect(mView, &CamerasView::focalChange, mModel, &CamerasModel::updateCurrentCameraFocal);

    connect(mView, &CamerasView::calibPriorCxChange, mModel, &CamerasModel::updateCurrentCameraCalibCx);
    connect(mView, &CamerasView::calibPriorCyChange, mModel, &CamerasModel::updateCurrentCameraCalibCy);
    connect(mView, &CamerasView::calibPriorFChange, mModel, &CamerasModel::updateCurrentCameraCalibF);
    connect(mView, &CamerasView::calibPriorFxChange, mModel, &CamerasModel::updateCurrentCameraCalibFx);
    connect(mView, &CamerasView::calibPriorFyChange, mModel, &CamerasModel::updateCurrentCameraCalibFy);
    connect(mView, &CamerasView::calibPriorK1Change, mModel, &CamerasModel::updateCurrentCameraCalibK1);
    connect(mView, &CamerasView::calibPriorK2Change, mModel, &CamerasModel::updateCurrentCameraCalibK2);
    connect(mView, &CamerasView::calibPriorK3Change, mModel, &CamerasModel::updateCurrentCameraCalibK3);
    connect(mView, &CamerasView::calibPriorK4Change, mModel, &CamerasModel::updateCurrentCameraCalibK4);
    connect(mView, &CamerasView::calibPriorK5Change, mModel, &CamerasModel::updateCurrentCameraCalibK5);
    connect(mView, &CamerasView::calibPriorK6Change, mModel, &CamerasModel::updateCurrentCameraCalibK6);
    connect(mView, &CamerasView::calibPriorP1Change, mModel, &CamerasModel::updateCurrentCameraCalibP1);
    connect(mView, &CamerasView::calibPriorP2Change, mModel, &CamerasModel::updateCurrentCameraCalibP2);

    connect(mView, &QDialog::accepted, this, &CamerasPresenterImp::save);
    connect(mView, &QDialog::rejected, this, &CamerasPresenterImp::discard);
    connect(mView, &DialogView::help, [&]() {
                emit help("cameras.html");
            });
}

void CamerasPresenterImp::activeCamera(int id)
{
    try {

        mView->setActiveCamera(id);

        Camera camera = mModel->camera(id);

        mView->setMake(camera.make().c_str());
        mView->setModel(camera.model().c_str());
        mView->setFocal(QString::number(camera.focal()));
        mView->setWidth(camera.width());
        mView->setHeight(camera.height());
        mView->setType(camera.type().c_str());
        mView->setSensorSize(QString::number(camera.sensorSize()));

        std::shared_ptr<Calibration> prior_calibration = camera.priorCalibration();
        if (prior_calibration) {
            for (const auto &param : *prior_calibration)
            {
                Calibration::Parameters parameter = param.first;
                double value = param.second;
                switch (parameter) {
                case Calibration::Parameters::focal:
                    mView->setPriorCalibF(value);
                    break;
                case Calibration::Parameters::focalx:
                    mView->setPriorCalibFx(value);
                    break;
                case Calibration::Parameters::focaly:
                    mView->setPriorCalibFy(value);
                    break;
                case Calibration::Parameters::cx:
                    mView->setPriorCalibCx(value);
                    break;
                case Calibration::Parameters::cy:
                    mView->setPriorCalibCy(value);
                    break;
                case Calibration::Parameters::k1:
                    mView->setPriorCalibK1(value);
                    break;
                case Calibration::Parameters::k2:
                    mView->setPriorCalibK2(value);
                    break;
                case Calibration::Parameters::k3:
                    mView->setPriorCalibK3(value);
                    break;
                case Calibration::Parameters::k4:
                    mView->setPriorCalibK4(value);
                    break;
                case Calibration::Parameters::k5:
                    mView->setPriorCalibK5(value);
                    break;
                case Calibration::Parameters::k6:
                    mView->setPriorCalibK6(value);
                    break;
                case Calibration::Parameters::p1:
                    mView->setPriorCalibP1(value);
                    break;
                case Calibration::Parameters::p2:
                    mView->setPriorCalibP2(value);
                    break;
                }
            }

        }

        std::shared_ptr<Calibration> adjust_calibration = camera.calibration();
        if (adjust_calibration) {
            for (const auto &param : *adjust_calibration) {
                Calibration::Parameters parameter = param.first;
                double value = param.second;
                switch (parameter) {
                case Calibration::Parameters::focal:
                    mView->setAdjustCalibF(value);
                    break;
                case Calibration::Parameters::focalx:
                    mView->setAdjustCalibFx(value);
                    break;
                case Calibration::Parameters::focaly:
                    mView->setAdjustCalibFy(value);
                    break;
                case Calibration::Parameters::cx:
                    mView->setAdjustCalibCx(value);
                    break;
                case Calibration::Parameters::cy:
                    mView->setAdjustCalibCy(value);
                    break;
                case Calibration::Parameters::k1:
                    mView->setAdjustCalibK1(value);
                    break;
                case Calibration::Parameters::k2:
                    mView->setAdjustCalibK2(value);
                    break;
                case Calibration::Parameters::k3:
                    mView->setAdjustCalibK3(value);
                    break;
                case Calibration::Parameters::k4:
                    mView->setAdjustCalibK4(value);
                    break;
                case Calibration::Parameters::k5:
                    mView->setAdjustCalibK5(value);
                    break;
                case Calibration::Parameters::k6:
                    mView->setAdjustCalibK6(value);
                    break;
                case Calibration::Parameters::p1:
                    mView->setAdjustCalibP1(value);
                    break;
                case Calibration::Parameters::p2:
                    mView->setAdjustCalibP2(value);
                    break;
                }
            }
        }

        mView->setImages(mModel->imagesFromCamera(id));

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void CamerasPresenterImp::save()
{
    if (mModel->modified()) {
        mModel->save();
        clear();
        emit updateCameras();
    }
}

void CamerasPresenterImp::discard()
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
    QSignalBlocker blocker(mView);

    int first_camera = -1;
    for (const auto &map_camera : mModel->cameras()) {

        int camera_id = map_camera.first;

        if (first_camera == -1) {
            first_camera = camera_id;
        }

        Camera camera = map_camera.second;
        QString camera_name = QString(camera.make().c_str()).append("-").append(camera.model().c_str());
        mView->addCamera(camera_id, camera_name);
    }

    activeCamera(first_camera);
}

void CamerasPresenterImp::priorCalibrationImport(const QString &file,
                                                 const QString &format)
{
    try {

        mModel->priorCalibrationImport(file, format);
        int camera_id = mModel->currentCameraID();
        if (camera_id != 0)
            activeCamera(camera_id);

    } catch (const std::exception &e) {
        tl::printException(e);
    }
}

} // namespace graphos
