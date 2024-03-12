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

#include "graphos/core/camera/Colmap.h"

#include "graphos/core/camera/Calibration.h"
#include "graphos/core/camera/Camera.h"

#include <colmap/base/reconstruction.h>


namespace graphos
{


ReadCalibration::ReadCalibration()
    : mReconstruction(new colmap::Reconstruction)
{

}

ReadCalibration::~ReadCalibration()
{
    if (mReconstruction) {
        delete mReconstruction;
        mReconstruction = nullptr;
    }
}

void ReadCalibration::open(const QString &path)
{
    mReconstruction->ReadBinary(path.toStdString());
}

auto ReadCalibration::calibration(int cameraId) const -> std::shared_ptr<Calibration>
{
    std::shared_ptr<Calibration> calibration;
    if (mReconstruction->ExistsCamera(static_cast<colmap::image_t>(cameraId))) {

        auto &camera = mReconstruction->Camera(static_cast<colmap::image_t>(cameraId));
        auto &params = camera.Params();

        std::string model_name = camera.ModelName();

        if (model_name == "SIMPLE_PINHOLE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::simple_pinhole);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);

        } else if (model_name == "PINHOLE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::pinhole);
            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
            calibration->setParameter(Calibration::Parameters::cx, params[2]);
            calibration->setParameter(Calibration::Parameters::cy, params[3]);

        } else if (model_name == "SIMPLE_RADIAL") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::radial1);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);

        } else if (model_name == "RADIAL") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::radial2);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);
            calibration->setParameter(Calibration::Parameters::k2, params[4]);

        } else if (model_name == "OPENCV") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
            calibration->setParameter(Calibration::Parameters::cx, params[2]);
            calibration->setParameter(Calibration::Parameters::cy, params[3]);
            calibration->setParameter(Calibration::Parameters::k1, params[4]);
            calibration->setParameter(Calibration::Parameters::k2, params[5]);
            calibration->setParameter(Calibration::Parameters::p1, params[6]);
            calibration->setParameter(Calibration::Parameters::p2, params[7]);

        } else if (model_name == "OPENCV_FISHEYE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
            calibration->setParameter(Calibration::Parameters::cx, params[2]);
            calibration->setParameter(Calibration::Parameters::cy, params[3]);
            calibration->setParameter(Calibration::Parameters::k1, params[4]);
            calibration->setParameter(Calibration::Parameters::k2, params[5]);
            calibration->setParameter(Calibration::Parameters::k3, params[6]);
            calibration->setParameter(Calibration::Parameters::k4, params[7]);

        } else if (model_name == "FULL_OPENCV") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_full);
            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
            calibration->setParameter(Calibration::Parameters::cx, params[2]);
            calibration->setParameter(Calibration::Parameters::cy, params[3]);
            calibration->setParameter(Calibration::Parameters::k1, params[4]);
            calibration->setParameter(Calibration::Parameters::k2, params[5]);
            calibration->setParameter(Calibration::Parameters::p1, params[6]);
            calibration->setParameter(Calibration::Parameters::p2, params[7]);
            calibration->setParameter(Calibration::Parameters::k3, params[8]);
            calibration->setParameter(Calibration::Parameters::k4, params[9]);
            calibration->setParameter(Calibration::Parameters::k5, params[10]);
            calibration->setParameter(Calibration::Parameters::k6, params[11]);

        } else if (model_name == "SIMPLE_RADIAL_FISHEYE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::simple_radial_fisheye);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);

        } else if (model_name == "RADIAL_FISHEYE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::radial_fisheye);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);
            calibration->setParameter(Calibration::Parameters::k2, params[4]);

        } /*else if (model_name.compare("FULL_RADIAL") == 0){

          calibration = CalibrationFactory::create(Calibration::CameraModel::radial3);
          calibration->setParameter(Calibration::Parameters::focal, params[0]);
          calibration->setParameter(Calibration::Parameters::cx, params[1]);
          calibration->setParameter(Calibration::Parameters::cy, params[2]);
          calibration->setParameter(Calibration::Parameters::k1, params[3]);
          calibration->setParameter(Calibration::Parameters::k2, params[4]);
          calibration->setParameter(Calibration::Parameters::k3, params[5]);
          calibration->setParameter(Calibration::Parameters::p1, params[6]);
          calibration->setParameter(Calibration::Parameters::p2, params[7]);

        }*/ else {
            TL_TODO("camara no soportada")
                return nullptr;
        }

    }

    return calibration;
}



auto cameraToColmapType(const Camera& camera) -> QString
{
    QString colmap_camera;
    auto graphos_camera = camera.type();

    if (graphos_camera == "Pinhole 1") {
        colmap_camera = "SIMPLE_PINHOLE";
    } else if (graphos_camera == "Pinhole 2") {
        colmap_camera = "PINHOLE";
    } else if (graphos_camera == "Radial 1") {
        colmap_camera = "SIMPLE_RADIAL";
    } else if (graphos_camera == "Radial 2") {
        colmap_camera = "RADIAL";
    } else if (graphos_camera == "OpenCV 1") {
        colmap_camera = "OPENCV";
    } else if (graphos_camera == "OpenCV Fisheye") {
        colmap_camera = "OPENCV_FISHEYE";
    } else if (graphos_camera == "OpenCV 2") {
        colmap_camera = "FULL_OPENCV";
    } else if (graphos_camera == "Radial Fisheye 1") {
        colmap_camera = "SIMPLE_RADIAL_FISHEYE";
    } else if (graphos_camera == "Radial Fisheye 2") {
        colmap_camera = "RADIAL_FISHEYE";
    } /*else if (graphos_camera "Radial 3"){
      colmap_camera = "FULL_RADIAL";
    }*/

    return colmap_camera;
}



} // namespace graphos
