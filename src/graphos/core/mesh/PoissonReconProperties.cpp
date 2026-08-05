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

#include "graphos/core/mesh/PoissonReconProperties.h"

namespace graphos
{


/* PoissonReconProperties */

PoissonReconProperties::PoissonReconProperties()
  : mProperties("PoissonRecon")
{
    clear();
}

auto PoissonReconProperties::depth() const -> int
{
    return mProperties.getProperty<int>("Depth");
}

//auto PoissonReconProperties::solveDepth() const -> int
//{
//    return mSolveDepth;
//}

auto PoissonReconProperties::boundaryType() const -> BoundaryType
{
    return static_cast<BoundaryType>(mProperties.getProperty<uint8_t>("BoundaryType"));
}

auto PoissonReconProperties::boundaryTypeAsText() const -> std::string
{
    std::string boundary_type;

    switch (boundaryType()) {
        case BoundaryType::free:
            boundary_type = "Free";
            break;
        case BoundaryType::dirichlet:
            boundary_type = "Dirichlet";
            break;
        case BoundaryType::neumann:
            boundary_type = "Neumann";
            break;
    }

    return boundary_type;
}

void PoissonReconProperties::setDepth(int depth)
{
    mProperties.setProperty("Depth", depth);
}

//void PoissonReconProperties::setSolveDepth(int solveDepth)
//{
//    mSolveDepth = solveDepth;
//}

void PoissonReconProperties::setBoundaryType(BoundaryType boundaryType)
{
    mProperties.setProperty("BoundaryType", static_cast<uint8_t>(boundaryType));
}


void PoissonReconProperties::clear()
{
    setDepth(14);
    //mSolveDepth = default_poisson_recon_solve_depth;
    setBoundaryType(PoissonReconProperties::BoundaryType::neumann);
}

void PoissonReconProperties::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}

} // namespace graphos
