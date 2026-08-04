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

#include "graphos/core/dense/MvsProperties.h"

namespace graphos
{

MvsProperties::MvsProperties()
  : DensificationProperties("MVS")
{
    clear();
}

auto MvsProperties::resolutionLevel() const -> int
{
    return mProperties.getProperty<int>("ResolutionLevel");
}

auto MvsProperties::minResolution() const -> int
{
    return mProperties.getProperty<int>("MinResolution");
}

auto MvsProperties::maxResolution() const -> int
{
    return mProperties.getProperty<int>("MaxResolution");
}

auto MvsProperties::numberViews() const -> int
{
    return mProperties.getProperty<int>("NumberViews");
}

auto MvsProperties::numberViewsFuse() const -> int
{
    return mProperties.getProperty<int>("NumberViewsFuse");
}

auto MvsProperties::estimateColors() const -> bool
{
    return mProperties.getProperty<bool>("EstimateColors");
}

auto MvsProperties::estimateNormals() const -> bool
{
    return mProperties.getProperty<bool>("EstimateNormals");
}

void MvsProperties::setResolutionLevel(int resolutionLevel)
{
    mProperties.setProperty("ResolutionLevel", resolutionLevel);
}

void MvsProperties::setMinResolution(int minResolution)
{
    mProperties.setProperty("MinResolution", minResolution);
}

void MvsProperties::setMaxResolution(int maxResolution)
{
    mProperties.setProperty("MaxResolution", maxResolution);
}

void MvsProperties::setNumberViews(int numberViews)
{
    mProperties.setProperty("NumberViews", numberViews);
}

void MvsProperties::setNumberViewsFuse(int numberViewsFuse)
{
    mProperties.setProperty("NumberViewsFuse", numberViewsFuse);
}

void MvsProperties::setEstimateColors(bool estimateColors)
{
    mProperties.setProperty("EstimateColors", estimateColors);
}

void MvsProperties::setEstimateNormals(bool estimateNormals)
{
    mProperties.setProperty("EstimateNormals", estimateNormals);
}

void MvsProperties::clear()
{
    setResolutionLevel(1);
    setMinResolution(256);
    setMaxResolution(3000);
    setNumberViews(5);
    setNumberViewsFuse(3);
    setEstimateColors(true);
    setEstimateNormals(true);
}


} // namespace graphos
