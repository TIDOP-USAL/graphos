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

#include "graphos/core/sfm/ColmapReconstructionConvert.h"
//#include "graphos/core/sfm/orientationexport.h"
//#include "graphos/core/sfm/groundpoint.h"
//#include "graphos/core/sfm/posesio.h"
//#include "graphos/core/sfm/BundleAdjuster.h"
//#include "graphos/core/camera/Camera.h"
//#include "graphos/core/image.h"
//#include "graphos/core/camera/Colmap.h"
//
//#include <tidop/core/app/Message.h>
//#include <tidop/core/base/Path.h>
//#include <tidop/core/base/Chrono.h>
//#include <tidop/core/task/Progress.h>
//#include <tidop/math/algebra/rotations/RotationMatrix.h>
//#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/geometry/primitives/Point.h>

#include <colmap/scene/reconstruction.h>
//#include <colmap/util/option_manager.h>
//#include <colmap/util/misc.h>
//#include <colmap/controllers/hierarchical_mapper.h>
//#include <colmap/base/cost_functions.h>

namespace graphos
{


ColmapReconstructionConvert::ColmapReconstructionConvert(const colmap::Reconstruction *reconstruction, 
                                                         const std::unordered_map<size_t, Image> &images)
    : mReconstruction(reconstruction),
      mImages(images)
{
    for (const auto &[image_id, image] : mImages) {

        auto &image_path = image.path();

        for (const auto &[colmap_image_id, colmap_image] : mReconstruction->Images()) {

            tl::Path colmap_image_path(colmap_image.Name());

            if (image_path.equivalent(colmap_image_path)) {
                mImageIds[colmap_image_id] = image_id;
                break;
            }
        }

    }
}

auto ColmapReconstructionConvert::groundPoints() const -> std::vector<GroundPoint>
{
    std::vector<GroundPoint> ground_points(mReconstruction->NumPoints3D());

    size_t i = 0;

    for (auto &[_,points_3d] : mReconstruction->Points3D()) {

        GroundPoint ground_point;
        ground_point.setPoint(tl::Point3d(points_3d.xyz.x(),
                                          points_3d.xyz.y(),
                                          points_3d.xyz.z()));
        tl::ColorRGB rgb(points_3d.color(0),
                         points_3d.color(1),
                         points_3d.color(2));
        ground_point.setColor(rgb.toColor());

        for (auto &pair : points_3d.track.Elements()) {

            ground_point.addPairToTrack(mImageIds.at(pair.image_id), pair.point2D_idx);

        }

        ground_points[i] = ground_point;

        ++i;
    }

    return ground_points;
}

auto ColmapReconstructionConvert::cameraPoses() const -> std::unordered_map<size_t, CameraPose>
{
    std::unordered_map<size_t, CameraPose> camera_poses;

    for (auto &[colmap_image_id, colmap_image] : mReconstruction->Images()) {

        CameraPose photoOrientation;

        tl::Path colmap_image_path(colmap_image.Name());

        //const Eigen::Matrix<double, 3, 4> inv_proj_matrix = colmap_image.ProjectionCenter();
        //const Eigen::Vector3d pc = inv_proj_matrix.rightCols<1>();
        //photoOrientation.setPosition(tl::Point3d(pc(0), pc(1), pc(2)));
        Eigen::Vector3d pc = colmap_image.ProjectionCenter();
        photoOrientation.setPosition(tl::Point3d(pc.x(), pc.y(), pc.z()));

        //auto &qvec = colmap_image.Qvec();
        //Eigen::Vector4d normalized_qvec = colmap::NormalizeQuaternion(qvec);

        //Eigen::Matrix3d rot = colmap_image.RotationMatrix();
        Eigen::Matrix3d rot = colmap_image.CamFromWorld().rotation().toRotationMatrix();
        tl::RotationMatrix<double> rotation_matrix;
        rotation_matrix.at(0, 0) = rot(0, 0);
        rotation_matrix.at(0, 1) = rot(0, 1);
        rotation_matrix.at(0, 2) = rot(0, 2);
        rotation_matrix.at(1, 0) = rot(1, 0);
        rotation_matrix.at(1, 1) = rot(1, 1);
        rotation_matrix.at(1, 2) = rot(1, 2);
        rotation_matrix.at(2, 0) = rot(2, 0);
        rotation_matrix.at(2, 1) = rot(2, 1);
        rotation_matrix.at(2, 2) = rot(2, 2);
        photoOrientation.setRotationMatrix(rotation_matrix);

        camera_poses[mImageIds.at(colmap_image_id)] = photoOrientation;

    }

    return camera_poses;
}

auto ColmapReconstructionConvert::readCalibration(size_t cameraId) const -> std::shared_ptr<Calibration>
{
    std::shared_ptr<Calibration> calibration;
    if (mReconstruction->ExistsCamera(static_cast<colmap::image_t>(cameraId))) {

        auto &camera = mReconstruction->Camera(static_cast<colmap::image_t>(cameraId));
        auto &params = camera.params;

        auto &model_name = camera.ModelName();

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

        } else if (model_name == "FULL_RADIAL") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::radial3);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);
            calibration->setParameter(Calibration::Parameters::k2, params[4]);
            calibration->setParameter(Calibration::Parameters::k3, params[5]);
            calibration->setParameter(Calibration::Parameters::p1, params[6]);
            calibration->setParameter(Calibration::Parameters::p2, params[7]);

        } else {
            TL_TODO("camara no soportada")
                return nullptr;
        }

    }

    return calibration;
}

} // namespace graphos
