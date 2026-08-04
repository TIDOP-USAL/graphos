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

#include "graphos/core/dense/SmvsProperties.h"

namespace graphos
{

SmvsProperties::SmvsProperties()
  : DensificationProperties("SMVS")
{
    clear();
}

auto SmvsProperties::inputImageScale() const -> int
{
    return mProperties.getProperty<int>("InputImageScale");
}

auto SmvsProperties::outputDepthScale() const -> int
{
    return mProperties.getProperty<int>("OutputDepthScale");
}

auto SmvsProperties::shadingBasedOptimization() const -> bool
{
    return mProperties.getProperty<bool>("ShadingBasedOptimization");
}

auto SmvsProperties::semiGlobalMatching() const -> bool
{
    return mProperties.getProperty<bool>("SemiGlobalMatching");
}

auto SmvsProperties::surfaceSmoothingFactor() const -> double
{
    return mProperties.getProperty<bool>("SurfaceSmoothingFactor");
}

void SmvsProperties::setInputImageScale(int inputImageScale)
{
    mProperties.setProperty("InputImageScale", inputImageScale);
}

void SmvsProperties::setOutputDepthScale(int outputDepthScale)
{
    mProperties.setProperty("OutputDepthScale", outputDepthScale);
}

void SmvsProperties::setShadingBasedOptimization(bool shadingBasedOptimization)
{
    mProperties.setProperty("ShadingBasedOptimization", shadingBasedOptimization);
}

void SmvsProperties::setSemiGlobalMatching(bool semiGlobalMatching)
{
    mProperties.setProperty("SemiGlobalMatching", semiGlobalMatching);
}

void SmvsProperties::setSurfaceSmoothingFactor(double surfaceSmoothingFactor)
{
    mProperties.setProperty("SurfaceSmoothingFactor", surfaceSmoothingFactor);
}

void SmvsProperties::clear()
{
    setInputImageScale(1);
    setOutputDepthScale(2);
    setShadingBasedOptimization(false);
    setSemiGlobalMatching(false);
    setSurfaceSmoothingFactor(1.0);
}


} // namespace graphos
