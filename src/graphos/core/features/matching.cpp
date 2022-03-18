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

#include "graphos/core/features/matching.h"

namespace graphos
{

FeatureMatchingProperties::FeatureMatchingProperties()
  : mCrossCheck(true),
    mRatio(0.8),
    mDistance(0.7),
    mMaxError(4.0),
    mConfidence(0.999)
{

}

FeatureMatchingProperties::FeatureMatchingProperties(const FeatureMatchingProperties &featureMatching)
  : FeatureMatching(featureMatching),
    mCrossCheck(featureMatching.mCrossCheck),
    mRatio(featureMatching.mRatio),
    mDistance(featureMatching.mDistance),
    mMaxError(featureMatching.mMaxError),
    mConfidence(featureMatching.mConfidence)
{

}

bool FeatureMatchingProperties::crossCheck() const
{
  return mCrossCheck;
}

void FeatureMatchingProperties::enableCrossCheck(bool enable)
{
  mCrossCheck = enable;
}

double FeatureMatchingProperties::ratio() const
{
  return mRatio;
}

void FeatureMatchingProperties::setRatio(double ratio)
{
  mRatio = ratio;
}

double FeatureMatchingProperties::distance() const
{
  return mDistance;
}

void FeatureMatchingProperties::setDistance(double distance)
{
  mDistance = distance;
}

double FeatureMatchingProperties::maxError() const
{
  return mMaxError;
}

void FeatureMatchingProperties::setMaxError(double error)
{
  mMaxError = error;
}

double FeatureMatchingProperties::confidence() const
{
  return mConfidence;
}

void FeatureMatchingProperties::setConfidence(double error)
{
  mConfidence = error;
}

} // namespace graphos

