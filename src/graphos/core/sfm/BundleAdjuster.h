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

#ifndef GRAPHOS_CORE_BUNDLE_ADJUST_H
#define GRAPHOS_CORE_BUNDLE_ADJUST_H

#include "graphos/graphos_global.h"

#include <colmap/base/reconstruction.h>
#include <colmap/base/cost_functions.h>
#include <colmap/optim/bundle_adjustment.h>

#include "groundpoint.h"

namespace graphos
{


/// La clase GroundControlPoint no me sirve...


struct GCP
{
    tl::Vector3d point;
    GCPTrack track;
    std::string name;
};

// Extraido de Colmap para añadir los puntos de control

class BundleAdjustmentConfig
  : public colmap::BundleAdjustmentConfig
{

private:

    std::unordered_map<colmap::image_t, tl::Vector3d> mCamPositionErrors;
    std::unordered_map<colmap::image_t, tl::Point3d> mCameraPositions;
    std::vector<GCP> mControlPpoints;
    std::unordered_map<size_t, uint32_t> mImageIdsGraphosToColmap;
    std::unordered_map<uint32_t, size_t> mImageIdsColmapToGraphos;

    double mRobustFittingError = 4.;

public:

    BundleAdjustmentConfig();

    void setCameraPositionAccuracy(colmap::image_t imageId, const tl::Point3d &pose, const tl::Vector3d &accuracy);
    auto cameraPositionAccuracy(colmap::image_t imageId) const -> tl::Vector3d;

    auto cameraPosition(colmap::image_t imageId) const -> tl::Point3d;
    auto controlPoints() const -> std::vector<GCP>;
    void setGroundControlPoints(const std::vector<GCP> &controlPoints);
    void setImageIdsGraphosToColmap(const std::unordered_map<size_t, uint32_t> &convert);
    auto colmapId(size_t graphosId) const -> uint32_t;
    void setImageIdsColmapToGraphos(const std::unordered_map<uint32_t, size_t> &convert);
    auto graphosId(uint32_t graphosId) const -> size_t;

    void setRobustFittingError(double robustFittingError);
    auto robustFittingError() const;;
};




class BundleAdjuster
{

public:

    BundleAdjuster(colmap::BundleAdjustmentOptions options,
                   BundleAdjustmentConfig config);

    bool solve(colmap::Reconstruction *reconstruction);

    const ceres::Solver::Summary &summary() const;

private:

    void setUp(colmap::Reconstruction *reconstruction,
               ceres::LossFunction *lossFunction);

    void addImageToProblem(colmap::image_t imageId, 
                           colmap::Reconstruction *reconstruction,
                           ceres::LossFunction *lossFunction);

    void addPointToProblem(colmap::point3D_t point3DId,
                           colmap::Reconstruction *reconstruction,
                           ceres::LossFunction *lossFunction);

    void addControlPointToProblem(GCP &ground_points,
                                  colmap::Reconstruction *reconstruction,
                                  double ground_point_error,
                                  ceres::LossFunction *lossFunction);

protected:

    void parameterizeCameras(colmap::Reconstruction *reconstruction) const;
    void parameterizePoints(colmap::Reconstruction *reconstruction) const;

    const colmap::BundleAdjustmentOptions options_;
    BundleAdjustmentConfig config_;
    std::unique_ptr<ceres::Problem> problem_;
    ceres::Solver::Summary summary_;
    std::unordered_set<colmap::camera_t> camera_ids_;
    std::unordered_map<colmap::point3D_t, size_t> point3D_num_observations_;

};


} // namespace graphos


#endif // GRAPHOS_CORE_BUNDLE_ADJUST_H
