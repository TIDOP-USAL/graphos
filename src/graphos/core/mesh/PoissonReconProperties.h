/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#include <string>

#include <tidop/core/base/Property.h>

namespace graphos
{


/*!
 * \brief Properties for Poisson Surface Reconstruction.
 *
 * This class provides properties and settings for Poisson Surface Reconstruction algorithm.
 *
 * For more information on Poisson Surface Reconstruction, refer to:
 * - Paper: https://hhoppe.com/poissonrecon.pdf
 * - GitHub Repository: https://github.com/mkazhdan/PoissonRecon
 */
class PoissonReconProperties
{

protected:

    tl::Properties mProperties;

public:

    enum class BoundaryType : uint8_t
    {
        free,
        dirichlet,
        neumann
    };

public:

    PoissonReconProperties();
    PoissonReconProperties(const PoissonReconProperties &properties) noexcept = default;
    PoissonReconProperties(PoissonReconProperties &&properties) noexcept = default;
    virtual ~PoissonReconProperties() = default;
    
    auto operator=(const PoissonReconProperties &properties) -> PoissonReconProperties & = default;
    auto operator=(PoissonReconProperties &&properties) noexcept -> PoissonReconProperties & = default;

    /*!
     * \brief Gets the depth parameter.
     * This is the maximum depth of the tree that will be used for surface
     * reconstruction. The default value for this parameter is 8.
     * \return The depth parameter.
     */
    virtual auto depth() const -> int;

    /*!
     * \brief Gets the solve depth parameter.
     * \return The solve depth parameter.
     */
    //virtual auto solveDepth() const -> int;

    /*!
     * \brief Gets the boundary type parameter.
     * This parameter specifies the boundary type for the finite elements. Valid values are:
     * - free: Free boundary constraints
     * - dirichlet: Dirichlet boundary constraints
     * - neumann: Neumann boundary constraints
     * The default value for this parameter is neumann.
     * \return The boundary type parameter.
     * \see BoundaryType
     */
    virtual auto boundaryType() const -> BoundaryType;

    virtual auto boundaryTypeAsText() const -> std::string;

    /*!
     * \brief Sets the depth parameter.
     * \param[in] depth Depth parameter value.
     */
    virtual void setDepth(int depth);

    /*!
     * \brief Sets the solve depth parameter.
     * \param[in] solveDepth The solve depth parameter to set.
     */
    //virtual void setSolveDepth(int solveDepth);

    /*!
     * \brief Sets the boundary type parameter.
     * \param[in] boundaryType The boundary type parameter to set.
     * \see BoundaryType
     */
    virtual void setBoundaryType(BoundaryType boundaryType);

    /*!
     * \brief Recover the default values
     */
    void clear();

    [[nodiscard]]
    auto name() const -> std::string
    {
        return mProperties.name();
    }

    auto begin() const { return mProperties.begin(); }

    auto end() const { return mProperties.end(); }

    void setProperty(const std::string &key, const std::string &value);

    template<typename T>
    void setProperty(const std::string &key, T value)
    {
        mProperties.setProperty(key, value);
    }
//private:
//
//    int mDepth;
//    //int mSolveDepth;
//    BoundaryType mBoundaryType;

};


} // namespace graphos