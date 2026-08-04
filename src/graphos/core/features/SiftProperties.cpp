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

#include "graphos/core/features/SiftProperties.h"

namespace graphos
{

SiftProperties::SiftProperties()
  : FeatureExtractorProperties("SIFT")
{
    clear();
}

auto SiftProperties::featuresNumber() const -> int
{
    return mProperties.getProperty<int>("FeaturesNumber");
}

auto SiftProperties::octaveLayers() const -> int
{
    return mProperties.getProperty<int>("OctaveLayers");
}

auto SiftProperties::constrastThresholdAuto() const -> bool
{
    return mProperties.getProperty<bool>("ContrastThresholdAuto");
}

auto SiftProperties::contrastThreshold() const -> double
{
    return constrastThresholdAuto() ? 0.02 / octaveLayers() : mProperties.getProperty<double>("ContrastThreshold");
}

auto SiftProperties::edgeThreshold() const -> double
{
    return mProperties.getProperty<double>("EdgeThreshold");
}

void SiftProperties::setFeaturesNumber(int featuresNumber)
{
    mProperties.setProperty("FeaturesNumber", featuresNumber);
}

void SiftProperties::setOctaveLayers(int octaveLayers)
{
    mProperties.setProperty("OctaveLayers", octaveLayers);
}

void SiftProperties::setContrastThresholdAuto(bool active)
{
    mProperties.setProperty("ContrastThresholdAuto", active);
}

void SiftProperties::setContrastThreshold(double contrastThreshold)
{
    mProperties.setProperty("ContrastThreshold", contrastThreshold);
}

void SiftProperties::setEdgeThreshold(double edgeThreshold)
{
    mProperties.setProperty("EdgeThreshold", edgeThreshold);
}

void SiftProperties::clear()
{
    setFeaturesNumber(5000);
    setOctaveLayers(3);
    setContrastThresholdAuto(true);
    setContrastThreshold(0.02 / 3.);
    setEdgeThreshold(10.);
}

} // namespace graphos
