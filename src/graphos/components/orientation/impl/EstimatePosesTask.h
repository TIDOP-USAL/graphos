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

#pragma once

#include <map>
#include <unordered_map>
#include <stop_token>

#include <tidop/core/base/Path.h>
#include <tidop/core/task/Task.h>
#include <tidop/geometry/primitives/Point.h>

#include "graphos/core/sfm/OrientationReport.h"

namespace tl
{
class GeoTools;
}

namespace colmap
{
class Reconstruction;
//class ReconstructionManager;
class BaseController;
}

namespace graphos
{

class Image;
class Camera;
class ImageRepository;
class CameraRepository;

class EstimatePosesTask
  : public tl::Task
{

public:

    enum class Options
    {
        reset_calibration = (1 << 0),
        use_prior_calibration = (1 << 1),
        use_adjusted_calibration = (1 << 2),
        fix_calibration = (1 << 3),
        orientation_global = (1 << 9),
        absolute_orientation = (1 << 10),
        use_rtk_positioning_accuracy = (1 << 11),
        use_poses = (1 << 12),
        use_gcp = (1 << 13)
    };

public:

    /*!
     * \brief Constructor.
     *
     * Constructs a `ReconstructionTask` object with the given database path, output path, images, cameras,
     * and calibration fixing option.
     *
     * \param[in] database Path to the COLMAP database.
     * \param[in] outputPath Path to the output directory.
     * \param[in] images Images to be used for 3d reconstruction.
     * \param[in] cameras Camera.
     * \param[in] fixCalibration Flag indicating whether to fix the calibration parameters.
     * \param[in] absoluteOrientation Absolute orientation.
     * \param[in] groundControlPoints Ground control points file
     */
    EstimatePosesTask(tl::Path database,
                      tl::Path outputPath,
                      const ImageRepository &imageRepo,
                      CameraRepository &cameraRepo,
                      Options options,
                      tl::Path groundControlPoints = tl::Path());

    ~EstimatePosesTask() override;

    /*!
     * \brief Get the cameras used in the task.
     *
     * \return A map of camera IDs to camera objects.
     */
    //auto cameras() const->std::map<int, Camera>;

    auto cameraPosesErrors() const -> std::unordered_map<size_t, double>;

    /*!
     * \brief Get the orientation report after task execution.
     *
     * \return An `OrientationReport` containing information about the orientation task.
     */
    auto report() const -> OrientationReport;

    //auto enuCrs() const -> std::string;

    void setMinCommonImages(int minCommonImages);
    void setRobustAlignment(bool robustAlignment);
    void setRobustAlignmentMaxError(double robustAlignmentMaxError);

    void clear();

// tl::TaskBase

public:

    void stop() override;

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

private:

    tl::Path mDatabase;
    tl::Path mOutputPath;
    const ImageRepository &mImageRepo;
    CameraRepository &mCameraRepo;
    tl::EnumFlags<Options> mOptions;
    tl::Path mGroundControlPoints;
    int mMinCommonImages;
    bool mRobustAlignment;
    double mRobustAlignmentMaxError;
    colmap::BaseController *mMapper;
    //std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
    tl::GeoTools *mGeoTools;
    std::unordered_map<size_t, double> mCameraPosesErrors;
    OrientationReport mOrientationReport;
    std::string mEnuCrs;
};
ALLOW_BITWISE_FLAG_OPERATIONS(EstimatePosesTask::Options)

} // namespace graphos
