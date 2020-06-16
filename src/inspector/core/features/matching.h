#ifndef INSPECTOR_FEATURE_MATCHING_H
#define INSPECTOR_FEATURE_MATCHING_H

#include "inspector/inspector_global.h"

#include "inspector/core/features/features.h"

/* namespace colmap
{
struct ExhaustiveMatchingOptions;
struct SiftMatchingOptions;
class ExhaustiveFeatureMatcher;
} */

namespace inspector
{

class INSPECTOR_EXPORT FeatureMatchingProperties
  : public FeatureMatching
{

public:

  FeatureMatchingProperties();
  FeatureMatchingProperties(const FeatureMatchingProperties &featureMatching);
  ~FeatureMatchingProperties() override = default;

// FeatureMatching interface

public:

  bool crossCheck() const override;
  void enableCrossCheck(bool enable) override;
  double ratio() const override;
  void setRatio(double ratio) override;
  double distance() const override;
  void setDistance(double distance) override;
  double maxError() const override;
  void setMaxError(double error) override;
  double confidence() const override;
  void setConfidence(double error) override;

private:

  bool mCrossCheck;
  double mRatio;
  double mDistance;
  double mMaxError;
  double mConfidence;

};


//class INSPECTOR_EXPORT FeatureMatchingImp
//  : public FeatureMatchingProperties
//{

//public:

//  FeatureMatchingImp() {}

//// FeatureMatching interface

//public:

//  void setBlockSize(int blockSize) override;
//  void enableCrossCheck(bool enable) override;
//  void setRatio(double ratio) override;
//  void setDistance(double distance) override;
//  void setMaxMatches(int maxMatches) override;
//  void setMaxError(double error) override;
//  void setConfidence(double error) override;

//};

} // namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_H
