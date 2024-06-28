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

namespace graphos
{


// Extraido de Colmap para añadir los puntos de control

class BundleAdjustmentConfig
  : public colmap::BundleAdjustmentConfig
{

private:

    std::unordered_map<colmap::image_t, double> cam_position_errors_;

public:

    BundleAdjustmentConfig() : colmap::BundleAdjustmentConfig()
    {
    }

    void setCamPositionError(colmap::image_t image_id, double position_error)
    {
        cam_position_errors_[image_id] = position_error;
    }

    double getCamPositionError(colmap::image_t image_id) const
    {
        auto it = cam_position_errors_.find(image_id);
        return (it != cam_position_errors_.end()) ? it->second : std::numeric_limits<double>::max();
    }
};


class BundleAdjuster
{

public:

    BundleAdjuster(const colmap::BundleAdjustmentOptions &options,
                   const BundleAdjustmentConfig &config);

    bool solve(colmap::Reconstruction *reconstruction);

    // Get the Ceres solver summary for the last call to `Solve`.
    const ceres::Solver::Summary &summary() const;

private:

    void setUp(colmap::Reconstruction *reconstruction,
               ceres::LossFunction *lossFunction);
    //void tearDown(colmap::Reconstruction *reconstruction);

    void addImageToProblem(colmap::image_t imageId, 
                           colmap::Reconstruction *reconstruction,
                           ceres::LossFunction *lossFunction);

    void addPointToProblem(colmap::point3D_t point3DId,
                           colmap::Reconstruction *reconstruction,
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
