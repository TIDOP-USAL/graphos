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

#include "graphos/core/dense/DensificationProperties.h"

namespace graphos
{

/*!
 * \brief Properties for CMVS/PMVS densification.
 *
 * This class represents properties related to Clustering Views for Multi-view Stereo (CMVS) and
 * Patch-based Multi-view Stereo Software (PMVS). It inherits functionality for densification from
 * the Densification class.
 *
 * CMVS: Clustering Views for Multi-view Stereo
 * Website: https://www.di.ens.fr/cmvs/
 *
 * PMVS: Patch-based Multi-view Stereo Software (Version 2)
 * Website: https://www.di.ens.fr/pmvs/
 */
class CmvsPmvsProperties
  : public DensificationProperties
{

public:

    /*!
     * \brief Default constructor for CMVS/PMVS properties.
     * Constructs a CmvsPmvs object with default values.
     */
    CmvsPmvsProperties();

    /*!
     * \brief Copy constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by copying from another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to copy from.
     */
    CmvsPmvsProperties(const CmvsPmvsProperties &cmvsPmvs) = default;

    /*!
     * \brief Move constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by moving the resources of another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to move from.
     */
    CmvsPmvsProperties(CmvsPmvsProperties &&cmvsPmvs) noexcept = default;

    ~CmvsPmvsProperties() override = default;

    /*!
     * \brief Move constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by moving the resources of another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to move from.
     */
    auto operator =(const CmvsPmvsProperties& cmvsPmvs) -> CmvsPmvsProperties& = default;

    /*!
     * \brief Move constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by moving the resources of another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to move from.
     */
    auto operator =(CmvsPmvsProperties&& cmvsPmvs) noexcept -> CmvsPmvsProperties& = default;

    auto useVisibilityInformation() const -> bool;
    auto imagesPerCluster() const -> int;
    auto level() const -> int;
    auto cellSize() const -> int;
    auto threshold() const -> double;
    auto windowSize() const -> int;
    auto minimunImageNumber() const -> int;

    void setUseVisibilityInformation(bool useVisibilityInformation);
    void setImagesPerCluster(int imagesPerCluster);
    void setLevel(int level);
    void setCellSize(int cellSize);
    void setThreshold(double threshold);
    void setWindowSize(int windowSize);
    void setMinimunImageNumber(int minimunImageNumber);

// DensificationProperties interface

public:

    void clear() override;

};

} // namespace graphos
