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

#include "graphos/core/dense/SmvsProperties.h"
#include "graphos/components/dense/impl/DenseTaskBase.h"

namespace graphos
{

/*!
 * \brief Class for SMVS densification.
 *
 * This class represents a densifier using Shading-aware Multi-view Stereo (SMVS) reconstruction.
 * It inherits properties related to SMVS reconstruction from the Smvs class and functionality
 * for densification from the DensifierBase class.
 */
class SmvsTask
  : public DenseTaskBase
{

public:

    /*!
     * \brief Constructor for SMVS densifier.
     * Constructs an SmvsDensifier object with provided parameters.
     * \param[in] images Images used for densification.
     * \param[in] cameras Cameras associated with the images.
     * \param[in] poses Camera poses associated with the images.
     * \param[in] groundPoints Ground points.
     * \param[in] outputPath The output path for storing densification results.
     * \param[in] cuda Flag indicating whether to use CUDA for computation (default is false).
     * \param[in] autoSegmentation Flag indicating whether to enable auto-segmentation (default is false).
     */
    SmvsTask(const std::shared_ptr<SmvsProperties> &properties,
             const ImageRepository &imageRepo,
             const CameraRepository &cameraRepo,
             const CameraPosesRepository &poses,
             const std::vector<GroundPoint> &groundPoints,
             const tl::Path &outputPath,
             bool cuda = false,
             bool autoSegmentation = false);
    ~SmvsTask() override;

    TL_DISABLE_COPY(SmvsTask)
    TL_DISABLE_MOVE(SmvsTask)

private:

    void clearPreviousModel() const;
    void writeMveFile();
    void densify();

// DensifierBase

protected:

    void copyUndistortedImages() const override;

// TaskBase

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

private:

    std::shared_ptr<SmvsProperties> mProperties;
    std::unordered_map<size_t, size_t> mGraphosToMveIds;
    bool mAutoSegmentation;

};


} // namespace graphos