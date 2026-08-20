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
 * \brief Shading-aware Multi-view Stereo.
 *
 * Shading-aware Multi-view Stereo (SMVS) is a reconstruction method that incorporates shading-based
 * optimization and semi-global matching (SGM) to generate accurate 3D reconstructions from multiple
 * images. This class represents properties related to SMVS reconstruction.
 *
 * References:
 * - Paper: http://www.kalyans.org/research/2016/ShadingAwareMVS_ECCV16.pdf
 * - GitHub Repository: https://github.com/flanggut/smvs
 */
class SmvsProperties
  : public DensificationProperties
{

public:

    /*!
     * \brief Default constructor for SMVS properties.
     * Constructs an Smvs object with default values.
     */
    SmvsProperties();

    /*!
     * \brief Copy constructor for SMVS properties.
     * Constructs a new Smvs object by copying from another Smvs object.
     * \param smvs The Smvs object to copy from.
     */
    SmvsProperties(const SmvsProperties &smvs) = default;

    /*!
     * \brief Move constructor for SMVS properties.
     * Constructs a new Smvs object by moving the resources of another Smvs object.
     * \param smvs The Smvs object to move from.
     */
    SmvsProperties(SmvsProperties &&smvs) noexcept = default;

    ~SmvsProperties() override = default;

    /*!
     * \brief Copy assignment operator for SMVS properties.
     * Assigns values from another Smvs object to this object.
     * \param smvs The Smvs object to assign from.
     * \return Reference to the assigned Smvs object.
     */
    auto operator =(const SmvsProperties& smvs) -> SmvsProperties& = default;

    /*!
     * \brief Move assignment operator for SMVS properties.
     * Moves the resources from another Smvs object to this object.
     * \param smvs The Smvs object to move from.
     * \return Reference to the moved Smvs object.
     */
    auto operator =(SmvsProperties&& smvs) noexcept -> SmvsProperties& = default;


    /*!
     * \brief Get the scale factor for input images.
     * \return The scale factor for input images.
     */
    auto inputImageScale() const -> int;

    /*!
     * \brief Get the scale factor for output depth maps.
     * \return The scale factor for output depth maps.
     */
    auto outputDepthScale() const -> int;

    /*!
     * \brief Check if shading-based optimization is enabled.
     * \return True if shading-based optimization is enabled, false otherwise.
     */
    auto shadingBasedOptimization() const -> bool;

    /*!
     * \brief Check if semi-global matching (SGM) is enabled.
     * \return True if semi-global matching (SGM) is enabled, false otherwise.
     */
    auto semiGlobalMatching() const -> bool;

    /*!
     * \brief Get the surface smoothing factor.
     * \return The surface smoothing factor.
     */
    auto surfaceSmoothingFactor() const -> double;

    /*!
     * \brief Set the scale factor for input images.
     * \param inputImageScale The scale factor for input images.
     */
    void setInputImageScale(int inputImageScale);

    /*!
     * \brief Set the scale factor for output depth maps.
     * \param outputDepthScale The scale factor for output depth maps.
     */
    void setOutputDepthScale(int outputDepthScale);

    /*!
     * \brief Enable or disable shading-based optimization.
     * \param shadingBasedOptimization True to enable shading-based optimization, false to disable it.
     */
    void setShadingBasedOptimization(bool shadingBasedOptimization);

    /*!
     * \brief Enable or disable semi-global matching (SGM).
     * \param semiGlobalMatching True to enable semi-global matching (SGM), false to disable it.
     */
    void setSemiGlobalMatching(bool semiGlobalMatching);

    /*!
     * \brief Set the surface smoothing factor.
     * \param surfaceSmoothingFactor The surface smoothing factor.
     */
    void setSurfaceSmoothingFactor(double surfaceSmoothingFactor);

// DensificationProperties interface

public:

    void clear() override;

};

} // namespace graphos
