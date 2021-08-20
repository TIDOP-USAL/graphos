#include "matching.h"

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

