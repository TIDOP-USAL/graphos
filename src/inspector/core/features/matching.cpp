#include "matching.h"

namespace inspector
{

FeatureMatchingProperties::FeatureMatchingProperties()
  : //mBlockSize(50),
    mCrossCheck(true),
    mRatio(0.8),
    mDistance(0.7),
//    mMaxMatches(32768),
    mMaxError(4.0),
    mConfidence(0.999)
{

}

FeatureMatchingProperties::FeatureMatchingProperties(const FeatureMatchingProperties &featureMatching)
  : FeatureMatching(featureMatching),
    //mBlockSize(featureMatching.mBlockSize),
    mCrossCheck(featureMatching.mCrossCheck),
    mRatio(featureMatching.mRatio),
    mDistance(featureMatching.mDistance),
//    mMaxMatches(featureMatching.mMaxMatches),
    mMaxError(featureMatching.mMaxError),
    mConfidence(featureMatching.mConfidence)
{

}

//int FeatureMatchingProperties::blockSize() const
//{
//  return mBlockSize;
//}

//void FeatureMatchingProperties::setBlockSize(int blockSize)
//{
//  mBlockSize = blockSize;
//}

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

//int FeatureMatchingProperties::maxMatches() const
//{
//  return mMaxMatches;
//}

//void FeatureMatchingProperties::setMaxMatches(int maxMatches)
//{
//  mMaxMatches = maxMatches;
//}

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

} // namespace inspector

