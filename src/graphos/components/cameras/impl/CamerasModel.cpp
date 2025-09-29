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

#include <tidop/core/msg/message.h>

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
    mActiveCameraId(0),
    mCameraCache()
{
    CamerasModelImp::init();
}

auto CamerasModelImp::cameras() const -> const std::map<int, Camera>&
{
    return mProject->cameras();
}

auto CamerasModelImp::addCamera(const Camera &camera) -> int
{
    return mProject->addCamera(camera);
}

auto CamerasModelImp::cameraID(const Camera &camera) const -> int
{
    return cameraID(camera.make().c_str(), camera.model().c_str());
}

auto CamerasModelImp::cameraID(const QString &make,
                               const QString &model) const -> int
{
    int id_camera = 0;

    QString camera_make;
    QString camera_model;

    for (const auto &camera : mProject->cameras()) {

        camera_make.fromStdString(camera.second.make());
        camera_model.fromStdString(camera.second.model());

        if (make.compare(camera_make) == 0 &&
            model.compare(camera_model) == 0) {
            id_camera = camera.first;
            break;
        }
    }

    return id_camera;
}

auto CamerasModelImp::camera(int id) const -> Camera
{
    Camera camera;

    auto it = mCameraCache.find(id);
    if (it != mCameraCache.end()) {
        camera = it->second;
    } else {
        camera = mProject->findCamera(id);
        mCameraCache[id] = camera;
    }

    mActiveCameraId = id;

    return camera;
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
    return mProject->updateCamera(id, camera);
}

void CamerasModelImp::updateCurrentCameraMake(const QString &make)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setMake(make.toStdString());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraModel(const QString &model)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setModel(model.toStdString());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraSensorSize(const QString &sensorSize)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setSensorSize(sensorSize.toDouble());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraFocal(const QString &focal)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setFocal(focal.toDouble());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraType(const QString &type)
{
    try {

        auto it = mCameraCache.find(mActiveCameraId);
        if (it != mCameraCache.end()) {
            mCameraCache[mActiveCameraId].setType(type.toStdString());
            mModifiedProject = true;
            if (auto calibration_old = mCameraCache[mActiveCameraId].calibration()) {

                auto calibration = CalibrationFactory::create(type.toStdString());

                for (const auto &param : *calibration){
                    Calibration::Parameters param_name = param.first;

                    if (param_name == Calibration::Parameters::focal && !calibration_old->existParameter(param_name)) {
                        double focal = calibration_old->parameter(Calibration::Parameters::focalx) + calibration_old->parameter(Calibration::Parameters::focaly) / 2.;
                        calibration->setParameter(param_name, focal);
                    } else if ((param_name == Calibration::Parameters::focalx || param_name == Calibration::Parameters::focaly) &&
                               calibration_old->existParameter(Calibration::Parameters::focal)) {
                        calibration->setParameter(param_name, calibration_old->parameter(Calibration::Parameters::focal));
                    } else if (calibration_old->existParameter(param_name)) {
                        calibration->setParameter(param_name, calibration_old->parameter(param_name));
                    }

                }

                mCameraCache[mActiveCameraId].setCalibration(calibration);

            }
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
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
        auto calibration_reader = CalibrationReaderFactory::create(format.toStdString());
        calibration_reader->read(tl::Path(file.toStdString()), mCameraCache[mActiveCameraId], true);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to import calibration to file: {}", file.toStdString());
    }
}

void CamerasModelImp::priorCalibrationExport(const QString &file,
                                             const QString &format)
{
    try {
        auto calibration_writer = CalibrationWriterFactory::create(format.toStdString());
        calibration_writer->write(tl::Path(file.toStdString()), mCameraCache[mActiveCameraId], true);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to export calibration to file: {}", file.toStdString());
    }
}

void CamerasModelImp::adjustCalibrationExport(const QString &file,
                                              const QString &format)
{
    try {
        auto calibration_writer = CalibrationWriterFactory::create(format.toStdString());
        calibration_writer->write(tl::Path(file.toStdString()), mCameraCache[mActiveCameraId]);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to export calibration to file: {}", file.toStdString());
    }
}

auto CamerasModelImp::removeCamera(int id) -> bool
{
    return mProject->removeCamera(id);
}

auto CamerasModelImp::removeCamera(const Camera &camera) -> bool
{
    int id_camera = cameraID(camera.make().c_str(), camera.model().c_str());
    return mProject->removeCamera(id_camera);
}

auto CamerasModelImp::imagesFromCamera(int id) const -> QStringList
{
    QStringList images_list;
    auto &images = mProject->images();
    for (const auto &image : images) {
        if (image.second.cameraId() == id) {
            images_list.push_back(image.second.path());
        }
    }

    return images_list;
}

void CamerasModelImp::save()
{
    if (mModifiedProject) {

        for (auto &camera : mCameraCache) {
            mProject->updateCamera(camera.first, camera.second);
        }

        mModifiedProject = false;
    }
}

bool CamerasModelImp::modified()
{
    return mModifiedProject;
}

void CamerasModelImp::init()
{

}

void CamerasModelImp::updateCalibrationParameter(Calibration::Parameters param, double value)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        auto calibration = mCameraCache[mActiveCameraId].priorCalibration();
        if (!calibration) {
            calibration = CalibrationFactory::create(mCameraCache[mActiveCameraId].type());
            mCameraCache[mActiveCameraId].setPriorCalibration(calibration);
        }
        calibration->setParameter(param, value);
        mModifiedProject = true;
    }
}

void CamerasModelImp::clear()
{
    mActiveCameraId = 0;
    mCameraCache.clear();
    mModifiedProject = false;
}

} // namespace graphos


