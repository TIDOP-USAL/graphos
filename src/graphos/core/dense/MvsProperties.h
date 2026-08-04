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

namespace colmap
{
struct FeatureKeypoint;
}

namespace graphos
{


/*!
 * \brief MVS properties for dense reconstruction.
 *
 * This class represents properties related to Multi-View Stereo (MVS) for dense reconstruction.
 * It allows configuring parameters such as resolution levels, number of views, and estimation options.
 *
 * For more information on Multi-View Stereo (MVS), refer to: https://github.com/cdcseacave/openMVS
 */
class MvsProperties
  : public DensificationProperties
{

public:

    /*!
     * \brief Default constructor.
     * Constructs an Mvs object with default values.
     */
    MvsProperties();

    /*!
     * \brief Copy constructor.
     * Constructs a new Mvs object by copying from another Mvs object.
     * \param[in] mvs The Mvs object to copy from.
     */
    MvsProperties(const MvsProperties &mvs) = default;

    /*!
     * \brief Move constructor.
     * Constructs a new Mvs object by moving the resources of another Mvs object.
     * \param[in] mvs The Mvs object to move from.
     */
    MvsProperties(MvsProperties &&mvs) noexcept = default;

    /*!
     * \brief Destructor.
     */
    ~MvsProperties() override = default;

    /*!
     * \brief Copy assignment operator.
     * Assigns values from another Mvs object to this object.
     * \param[in] mvs The Mvs object to assign from.
     * \return Reference to the assigned Mvs object.
     */
    auto operator =(const MvsProperties& mvs) -> MvsProperties& = default;

    /*!
     * \brief Move assignment operator.
     * Moves the resources from another Mvs object to this object.
     * \param[in] mvs The Mvs object to move from.
     * \return Reference to the moved Mvs object.
     */
    auto operator =(MvsProperties&& mvs) noexcept -> MvsProperties& = default;


// Getter methods

    /*!
     * \brief Get the resolution level.
     * \return The resolution level.
     */
    virtual auto resolutionLevel() const -> int;

    /*!
     * \brief Get the minimum resolution.
     * \return The minimum resolution.
     */
    virtual auto minResolution() const -> int;

    /*!
     * \brief Get the maximum resolution.
     * \return The maximum resolution.
     */
    virtual auto maxResolution() const -> int;

    /*!
     * \brief Get the number of views.
     * \return The number of views.
     */
    virtual auto numberViews() const -> int;

    /*!
     * \brief Get the number of views to fuse.
     * \return The number of views to fuse.
     */
    virtual auto numberViewsFuse() const -> int;

    /*!
     * \brief Check if color estimation is enabled.
     * \return True if color estimation is enabled, false otherwise.
     */
    virtual auto estimateColors() const -> bool;

    /*!
     * \brief Check if normals estimation is enabled.
     * \return True if normals estimation is enabled, false otherwise.
     */
    virtual auto estimateNormals() const -> bool;

// Setter methods

    /*!
     * \brief Set the resolution level.
     * \param[in] resolutionLevel The resolution level to set.
     */
    virtual void setResolutionLevel(int resolutionLevel);

    /*!
     * \brief Set the minimum resolution.
     * \param[in] minResolution The minimum resolution to set.
     */
    virtual void setMinResolution(int minResolution);

    /*!
     * \brief Set the maximum resolution.
     * \param[in] maxResolution The maximum resolution to set.
     */
    virtual void setMaxResolution(int maxResolution);

    /*!
     * \brief Set the number of views.
     * \param[in] numberViews The number of views to set.
     */
    virtual void setNumberViews(int numberViews);

    /*!
     * \brief Set the number of views to fuse.
     * \param[in] numberViewsFuse The number of views to fuse.
     */
    virtual void setNumberViewsFuse(int numberViewsFuse);

    /*!
     * \brief Set whether color estimation is enabled.
     * \param[in] estimateColors True to enable color estimation, false otherwise.
     */
    virtual void setEstimateColors(bool estimateColors);

    /*!
     * \brief Set whether normals estimation is enabled.
     * \param[in] estimateNormals True to enable normals estimation, false otherwise.
     */
    virtual void setEstimateNormals(bool estimateNormals);

// DensificationProperties

public:

    void clear() override;
};

} // namespace graphos
