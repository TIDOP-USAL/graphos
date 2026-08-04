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

#include "config_graphos.h"

#include <stop_token>

#include <tidop/core/base/Path.h>

#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/dense/MvsProperties.h"
#include "graphos/components/dense/impl/DenseTaskBase.h"

namespace colmap
{
struct FeatureKeypoint;
}

namespace graphos
{


/*!
 * \brief Class for Multi-View Stereo (MVS) densification.
 *
 * This class represents a densifier using Multi-View Stereo (MVS) reconstruction.
 * It inherits properties related to MVS reconstruction from Mvs and functionality
 * for densification from DensifierBase.
 */
class MvsTask
  : public DenseTaskBase
{

public:

    /*!
     * \brief Constructor for MVS densifier.
     * Constructs an MvsDensifier object with provided parameters.
     * \param[in] images Images used for densification.
     * \param[in] cameras Cameras associated with the images.
     * \param[in] poses Camera poses associated with the images.
     * \param[in] groundPoints Ground points
     * \param[in] outputPath The output path for storing densification results.
     * \param[in] database The path to the database.
     * \param[in] cuda Flag indicating whether to use CUDA for computation (default is false).
     * \param[in] autoSegmentation Flag indicating whether to enable auto-segmentation (default is false).
     */
    MvsTask(const std::shared_ptr<MvsProperties> &properties,
            const ImageRepository &imageRepo,
            const CameraRepository &cameraRepo,
            const CameraPosesRepository &poses,
            const std::vector<GroundPoint> &groundPoints,
            const tl::Path &outputPath,
            tl::Path database,
            bool cuda = false,
            bool autoSegmentation = false);

    ~MvsTask() override;

    TL_DISABLE_COPY(MvsTask)
    TL_DISABLE_MOVE(MvsTask)

private:

    void clearTemporalFiles() const;
    void exportCameras(const std::map<int, Undistort> &undistortMap, 
                       const tl::Path &colmapSparsePath) const;
    void exportImages(const std::unordered_map<size_t, uint32_t> &graphosToColmapImageIds,
                      const std::unordered_map<size_t, std::vector<colmap::FeatureKeypoint>> &keypoints,
                      const tl::Path &colmapSparsePath) const;
    void exportPoints(const std::unordered_map<size_t, uint32_t> &graphosToColmapImageIds,
                      const tl::Path &colmapSparsePath) const;
    void exportToColmap() const;
    void writeNvmFile() const;
    void exportToMvs() const;
    void densify();

// DenseTaskBase

protected:

    void copyUndistortedImages() const override;

// TaskBase

protected:

    /*!
     * \brief Executes the densification task.
     * Overrides the execute method from the TaskBase class.
     * \param progressBar Pointer to a progress bar for tracking progress (default is nullptr).
     */
    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

private:

    std::shared_ptr<MvsProperties> mProperties;
    tl::Path mDatabase;
    bool mAutoSegmentation;

};


} // namespace graphos