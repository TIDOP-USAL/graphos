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

#include "tidop/core/base/Property.h"

#include "graphos/core/features/Features.h"


namespace graphos
{


/*!
 * \brief Class representing properties for SIFT detector/descriptor.
 *
 * This class encapsulates properties related to the SIFT (Scale-Invariant Feature Transform) detector/descriptor.
 */
class Sift
  : public Feature
{

public:

    /*!
     * \brief Default constructor for SIFT properties.
     * Constructs a Sift object with default values.
     */
    Sift();

    /*!
     * \brief Copy constructor for SIFT properties.
     * Constructs a new Sift object by copying from another Sift object.
     * \param[in] sift The Sift object to copy from.
     */
    Sift(const Sift &sift) = default;

    Sift(Sift &&sift) noexcept = default;

    /*!
     * \brief Destructor for SIFT.
     */
    ~Sift() override = default;

    auto operator=(const Sift &sift) -> Sift & = default;

    /*!
     * \brief Move assignment operator.
     *
     * Moves the properties from another `SiftProperties` instance.
     *
     * \param[in] properties The object to move.
     * \return Reference to the updated object.
     */
    auto operator=(Sift &&sift) noexcept -> Sift & = default;

public:

    /*!
     * \brief Get the number of features for SIFT.
     * The features are ranked by their scores (measured in
     * SIFT algorithm as the local contrast)
     * \return The number of best features to retain.
     */
    [[nodiscard]]
    auto featuresNumber() const -> int;

    /*!
     * \brief Get the number of layers per octave.
     * 3 is the value used in D. Lowe paper. The number of octaves is computed
     * automatically from the image resolution.
     * \return The number of layers per octave.
     */
    [[nodiscard]]
    auto octaveLayers() const -> int;

    /*!
     * \brief Check if automatic contrast thresholding is enabled.
     * \return True if automatic contrast thresholding is enabled, false otherwise.
     */
    [[nodiscard]]
    auto constrastThresholdAuto() const -> bool;

    /*!
     * \brief Get the contrast threshold used to filter out weak features in semi-uniform (low-contrast) regions.
     * The larger the threshold, the less features are produced by the detector.
     * \return The contrast threshold value.
     */
    [[nodiscard]]
    auto contrastThreshold() const -> double;

    /*!
     * \brief Get the edge threshold value for SIFT.
     * \return The edge threshold value.
     */
    [[nodiscard]]
    auto edgeThreshold() const -> double;

    /*!
     * \brief Set the number of best features to retain
     * \param[in] featuresNumber The number of features.
     */
    void setFeaturesNumber(int featuresNumber);

    /*!
     * \brief Set the number of layers per octave.
     * \param[in] octaveLayers The number of layers per octave (3 by default).
     */
    void setOctaveLayers(int octaveLayers);

    /*!
     * \brief Enable or disable automatic contrast thresholding.
     * \param[in] active True to enable automatic contrast thresholding, false to disable it.
     */
    void setContrastThresholdAuto(bool active);

    /*!
     * \brief Set the contrast threshold value.
     * \param[in] contrastThreshold The contrast threshold value.
     */
    void setContrastThreshold(double contrastThreshold);

    /*!
     * \brief Set the threshold used to filter out edge-like features
     * \param[in] edgeThreshold The edge threshold value.
     */
    void setEdgeThreshold(double edgeThreshold);

// Feature interface methods

public:

    void clear() override;

};

} // namespace graphos
