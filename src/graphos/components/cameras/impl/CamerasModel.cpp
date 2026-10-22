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

#include "CamerasModel.h"
#include "graphos/core/camera/io/CalibrationWriter.h"
#include "graphos/core/camera/io/CalibrationReader.h"

#include <tidop/core/app/Message.h>

#include <QXmlStreamReader>
#include <QDir>

#include <fstream>

namespace graphos
{

CamerasModelImp::CamerasModelImp(Project *project,
                                 QObject *parent)
  : CamerasModel(parent),
    mProject(project),
    mModifiedProject(false),
    mActiveCameraId(0)
{
    CamerasModelImp::init();
}

auto CamerasModelImp::cameras() const -> const CameraRepository &
{
    return mProject->cameras();
}

auto CamerasModelImp::addCamera(const Camera &camera) -> int
{
    mModifiedProject = true;
    return mCameraCache.add(camera);
}

auto CamerasModelImp::cameraID(const Camera &camera) const -> int
{
    return cameraID(camera.make().c_str(), camera.model().c_str());
}

auto CamerasModelImp::cameraID(const QString &make,
                               const QString &model) const -> int
{
    auto optId = mCameraCache.findIdBySignature(make.toStdString(), model.toStdString());

    return optId.value_or(0);
}

auto CamerasModelImp::camera(int id) const -> Camera
{
    if (auto *cam = mCameraCache.find(id)) {
        mActiveCameraId = id;
        return *cam;
    }

    mActiveCameraId = 0;
    return Camera{};
}

auto CamerasModelImp::camera(const QString &make,
                             const QString &model) const -> Camera
{
    int camera_id = cameraID(make, model);
    return camera(camera_id);
}

auto CamerasModelImp::currentCameraID() const -> int
{
    return mActiveCameraId;
}

auto CamerasModelImp::updateCamera(int id, const Camera &camera) -> bool
{
    if (mCameraCache.update(id, camera)) {
        mModifiedProject = true;
        return true;
    }
    return false;
}

void CamerasModelImp::updateCurrentCameraMake(const QString &make)
{
    if (auto *camera = mCameraCache.find(mActiveCameraId)) {
        camera->setMake(make.toStdString());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraModel(const QString &model)
{
    if (auto *camera = mCameraCache.find(mActiveCameraId)) {
        camera->setModel(model.toStdString());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraSensorSize(const QString &sensorSize)
{
    if (auto *camera = mCameraCache.find(mActiveCameraId)) {
        camera->setSensorSize(sensorSize.toDouble());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraFocal(const QString &focal)
{
    if (auto *camera = mCameraCache.find(mActiveCameraId)) {
        camera->setFocal(focal.toDouble());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraType(const QString &type)
{
    try {

        if (auto *camera = mCameraCache.find(mActiveCameraId)) {
            camera->setType(type.toStdString());
            mModifiedProject = true;

            const auto convertCalibration = [&type](const std::shared_ptr<Calibration> &oldCalibration) {
                if (!oldCalibration) {
                    return std::shared_ptr<Calibration>{};
                }

                auto calibration = CalibrationFactory::create(type.toStdString());
                for (const auto &[parameter, value] : *calibration) {
                    if (parameter == Calibration::Parameters::focal &&
                        !oldCalibration->existParameter(parameter) &&
                        oldCalibration->existParameter(Calibration::Parameters::focalx) &&
                        oldCalibration->existParameter(Calibration::Parameters::focaly)) {
                        const double focal = (oldCalibration->parameter(Calibration::Parameters::focalx) +
                                              oldCalibration->parameter(Calibration::Parameters::focaly)) / 2.;
                        calibration->setParameter(parameter, focal);
                    } else if ((parameter == Calibration::Parameters::focalx ||
                                parameter == Calibration::Parameters::focaly) &&
                               oldCalibration->existParameter(Calibration::Parameters::focal)) {
                        calibration->setParameter(parameter, oldCalibration->parameter(Calibration::Parameters::focal));
                    } else if (oldCalibration->existParameter(parameter)) {
                        calibration->setParameter(parameter, oldCalibration->parameter(parameter));
                    }
                }

                return calibration;
            };

            if (auto calibration = camera->calibration()) {
                camera->setCalibration(convertCalibration(calibration));
            }
            if (auto priorCalibration = camera->priorCalibration()) {
                camera->setPriorCalibration(convertCalibration(priorCalibration));
            }
        }

    } catch (std::exception &e) {
        tl::Message::error("Error updating the camera: {}", e.what());
    } catch (...) {
        tl::Message::error("Error updating the camera: Unknown error");
    }
}

void CamerasModelImp::updateCurrentCameraCalibCx(double cx)
{
    updateCalibrationParameter(Calibration::Parameters::cx, cx);
}

void CamerasModelImp::updateCurrentCameraCalibCy(double cy)
{
    updateCalibrationParameter(Calibration::Parameters::cy, cy);
}

void CamerasModelImp::updateCurrentCameraCalibF(double f)
{
    updateCalibrationParameter(Calibration::Parameters::focal, f);
}

void CamerasModelImp::updateCurrentCameraCalibFx(double fx)
{
    updateCalibrationParameter(Calibration::Parameters::focalx, fx);
}

void CamerasModelImp::updateCurrentCameraCalibFy(double fy)
{
    updateCalibrationParameter(Calibration::Parameters::focaly, fy);
}

void CamerasModelImp::updateCurrentCameraCalibK1(double k1)
{
    updateCalibrationParameter(Calibration::Parameters::k1, k1);
}

void CamerasModelImp::updateCurrentCameraCalibK2(double k2)
{
    updateCalibrationParameter(Calibration::Parameters::k2, k2);
}

void CamerasModelImp::updateCurrentCameraCalibK3(double k3)
{
    updateCalibrationParameter(Calibration::Parameters::k3, k3);
}

void CamerasModelImp::updateCurrentCameraCalibK4(double k4)
{
    updateCalibrationParameter(Calibration::Parameters::k4, k4);
}

void CamerasModelImp::updateCurrentCameraCalibK5(double k5)
{
    updateCalibrationParameter(Calibration::Parameters::k5, k5);
}

void CamerasModelImp::updateCurrentCameraCalibK6(double k6)
{
    updateCalibrationParameter(Calibration::Parameters::k6, k6);
}

void CamerasModelImp::updateCurrentCameraCalibP1(double p1)
{
    updateCalibrationParameter(Calibration::Parameters::p1, p1);
}

void CamerasModelImp::updateCurrentCameraCalibP2(double p2)
{
    updateCalibrationParameter(Calibration::Parameters::p2, p2);
}

void CamerasModelImp::priorCalibrationImport(const QString &file,
                                             const QString &format)
{
    try {
        if (auto *camera = mCameraCache.find(mActiveCameraId)) {
            auto calibration_reader = CalibrationReaderFactory::create(format.toStdString());
            calibration_reader->read(tl::Path(file.toStdWString()), *camera, true);
            mModifiedProject = true;
        }
    } catch (const std::exception &e) {
        tl::Message::error("Failed to import calibration to file: {}", file.toStdString());
        tl::printException(e);
    } catch (...) {
        tl::Message::error("Failed to import calibration to file: {}", file.toStdString());
    }
}

void CamerasModelImp::priorCalibrationExport(const QString &file,
                                             const QString &format)
{
    try {
        if (auto *camera = mCameraCache.find(mActiveCameraId)) {
            auto calibration_writer = CalibrationWriterFactory::create(format.toStdString());
            calibration_writer->write(tl::Path(file.toStdWString()), *camera, true);
        }
    } catch (const std::exception &e) {
        tl::Message::error("Failed to export calibration to file: {}", file.toStdString());
        tl::printException(e);
    } catch (...) {
        tl::Message::error("Failed to export calibration to file: {}", file.toStdString());
    }
}

void CamerasModelImp::adjustCalibrationExport(const QString &file,
                                              const QString &format)
{
    try {
        if (auto *camera = mCameraCache.find(mActiveCameraId)) {
            auto calibration_writer = CalibrationWriterFactory::create(format.toStdString());
            calibration_writer->write(tl::Path(file.toStdWString()), *camera);
        }
    } catch (const std::exception &e) {
        tl::Message::error("Failed to export calibration to file: {}", file.toStdString());
        tl::printException(e);
    } catch (...) {
        tl::Message::error("Failed to export calibration to file: {}", file.toStdString());
    }
}

auto CamerasModelImp::removeCamera(int id) -> bool
{
    if (mCameraCache.erase(id)) {
        mModifiedProject = true;
        return true;
    }
    return false;
}

auto CamerasModelImp::removeCamera(const Camera &camera) -> bool
{
    int id_camera = cameraID(camera);
    return removeCamera(id_camera);
}

auto CamerasModelImp::imagesFromCamera(int id) const -> QStringList
{
    QStringList images_list;
    for (const auto &[imageId, image] : mProject->images()) {
        if (image.cameraId() == id) {
            images_list.push_back(QString::fromStdWString(image.path().toWString()));
        }
    }
    return images_list;
}

void CamerasModelImp::save()
{
    if (mModifiedProject) {
        mProject->cameras() = mCameraCache;
        mModifiedProject = false;
    }
}

bool CamerasModelImp::modified()
{
    return mModifiedProject;
}

void CamerasModelImp::init()
{
    if (mProject) {
        mCameraCache = mProject->cameras();
    }
    mModifiedProject = false;
}

void CamerasModelImp::updateCalibrationParameter(Calibration::Parameters param, double value)
{
    if (auto *cam = mCameraCache.find(mActiveCameraId)) {
        auto calibration = cam->priorCalibration();
        if (!calibration) {
            calibration = CalibrationFactory::create(cam->type());
            cam->setPriorCalibration(calibration);
        }
        calibration->setParameter(param, value);
        mModifiedProject = true;
    }
}

void CamerasModelImp::clear()
{
    mActiveCameraId = 0;
    init();
}

} // namespace graphos

