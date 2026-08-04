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

#include "graphos/graphos_global.h"

#include <memory>
#include <unordered_map>
#include <vector>

//#include <tidop/core/task/Task.h>
//#include <tidop/core/base/Path.h>
//#include <tidop/geometry/primitives/Point.h>

#include "graphos/core/image/Image.h"
#include "graphos/core/orientation/GroundPoint.h"
#include "graphos/core/camera/Camera.h"

namespace tl
{
class Progress;
}

namespace colmap
{
class Reconstruction;
//class ReconstructionManager;
//struct IncrementalMapperOptions;
//class IncrementalMapperController;
}

namespace graphos
{

//class Image;
//class Camera;
//class GroundPoint;
//class CameraPose;
//class Calibration;

/*!
 * \brief COLMAP Reconstruction Converter
 *
 * The `ColmapReconstructionConvert` class is responsible for converting a COLMAP reconstruction into GRAPHOS format.
 *
 * It provides methods to access ground points, camera poses, and calibration information.
 */
class ColmapReconstructionConvert
{

public:

    /*!
     * \brief Constructor.
     *
     * Constructs a `ColmapReconstructionConvert` object with the given COLMAP reconstruction and images.
     *
     * \param[in] reconstruction Pointer to the COLMAP reconstruction.
     * \param[in] images Images associated with the reconstruction.
     */
    ColmapReconstructionConvert(const colmap::Reconstruction *reconstruction,
                                const std::unordered_map<size_t, Image> &images);

    ~ColmapReconstructionConvert()
    {
    }

    /*!
     * \brief Get the ground points from the reconstruction.
     *
     * \return A vector of ground points extracted from the reconstruction.
     */
    auto groundPoints() const -> std::vector<GroundPoint>;

    /*!
     * \brief Get the camera poses from the reconstruction.
     *
     * \return An unordered map containing image IDs and corresponding camera poses.
     */
    auto cameraPoses() const -> std::unordered_map<size_t, CameraPose>;

    /*!
     * \brief Read calibration information for a specific camera.
     *
     * Reads calibration information for the camera with the specified ID.
     *
     * \param[in] cameraId The ID of the camera.
     * \return A shared pointer to the calibration of the specified camera.
     */
    auto readCalibration(size_t cameraId) const -> std::shared_ptr<Calibration>;

private:

    const colmap::Reconstruction *mReconstruction;
    //std::vector<Image> mImages;
    const std::unordered_map<size_t, Image> &mImages;
    std::unordered_map<uint32_t, size_t> mImageIds;
};


} // namespace graphos
