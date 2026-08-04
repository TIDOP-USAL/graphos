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

#include "graphos/core/matching/MatchingProperties.h"

namespace graphos
{

//MatchingProperties::MatchingProperties()
//  : mCrossCheck(true),
//    mRatio(0.8),
//    mDistance(0.7),
//    mMaxError(4.0),
//    mConfidence(0.999)
//{
//}
//
//MatchingProperties::MatchingProperties(const MatchingProperties &featureMatching)
//  : mCrossCheck(featureMatching.mCrossCheck),
//    mRatio(featureMatching.mRatio),
//    mDistance(featureMatching.mDistance),
//    mMaxError(featureMatching.mMaxError),
//    mConfidence(featureMatching.mConfidence)
//{
//}

MatchingProperties::MatchingProperties()
  : mProperties("FeatureExtractor")
{
    clear();
}

auto MatchingProperties::crossCheck() const -> bool
{
    return mProperties.getProperty<bool>("CrossCheck");
}

void MatchingProperties::enableCrossCheck(bool enable)
{
    mProperties.setProperty("CrossCheck", enable);
}

auto MatchingProperties::ratio() const -> double
{
    return mProperties.getProperty<double>("Ratio");
}

void MatchingProperties::setRatio(double ratio)
{
    mProperties.setProperty("Ratio", ratio);
}

auto MatchingProperties::distance() const -> double
{
    return mProperties.getProperty<double>("Distance");
}

void MatchingProperties::setDistance(double distance)
{
    mProperties.setProperty("Distance", distance);
}

auto MatchingProperties::maxError() const -> double
{
    return mProperties.getProperty<double>("MaxError");
}

void MatchingProperties::setMaxError(double error)
{
    mProperties.setProperty("MaxError", error);
}

auto MatchingProperties::confidence() const -> double
{
    return mProperties.getProperty<double>("Confidence");
}

void MatchingProperties::setConfidence(double confidence)
{
    mProperties.setProperty("Confidence", confidence);
}

void MatchingProperties::clear()
{
    enableCrossCheck(true);
    setRatio(0.8);
    setDistance(0.7);
    setMaxError(4.0);
    setConfidence(0.999);
}

void MatchingProperties::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}


} // namespace graphos

