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

#ifndef GRAPHOS_CORE_RECONSTRUCTION_COLMAP_H
#define GRAPHOS_CORE_RECONSTRUCTION_COLMAP_H

#include <map>
#include <unordered_map>

#include <tidop/core/path.h>
#include <tidop/core/task.h>

#include "graphos/core/sfm/OrientationReport.h"


namespace colmap
{
class Reconstruction;
class ReconstructionManager;
struct IncrementalMapperOptions;
class IncrementalMapperController;
}

namespace graphos
{

class Image;
class Camera;

class ReconstructionTask
  : public tl::TaskBase
{

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
     */
    ReconstructionTask(tl::Path database,
                       tl::Path outputPath,
                       const std::vector<Image> &images,
                       const std::map<int, Camera> &cameras,
                       bool fixCalibration,
                       bool absoluteOrientation,
                       bool gps,
                       bool controlPoints);

    ~ReconstructionTask() override;

    /*!
     * \brief Get the cameras used in the task.
     *
     * \return A map of camera IDs to camera objects.
     */
    auto cameras() const->std::map<int, Camera>;

    auto cameraPosesErrors() const->std::unordered_map<size_t, double>;

    /*!
     * \brief Get the orientation report after task execution.
     *
     * \return An `OrientationReport` containing information about the orientation task.
     */
    auto report() const -> OrientationReport;

    void setMinCommonImages(int minCommonImages);
    void setRobustAlignment(bool robustAlignment);
    void setRobustAlignmentMaxError(double robustAlignmentMaxError);


    void clear();

// tl::TaskBase

public:

    void stop() override;

protected:

    void execute(tl::Progress *progressBar = nullptr) override;

private:

    bool mFixCalibration;
    bool mAbsoluteOrientation;
    bool mGPS;
    bool mControlPoints;

    int mMinCommonImages;
    bool mRobustAlignment;
    double mRobustAlignmentMaxError;

    tl::Path mDatabase;
    tl::Path mOutputPath;
    std::vector<Image> mImages;
    std::map<int, Camera> mCameras;
    colmap::IncrementalMapperOptions *mIncrementalMapper;
    colmap::IncrementalMapperController *mMapper;
    std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
    std::unordered_map<size_t, double> mCameraPosesErrors;
    OrientationReport mOrientationReport;
};


} // namespace graphos

#endif // GRAPHOS_CORE_RECONSTRUCTION_COLMAP_H
