#ifndef GRAPHOS_FEATURE_MATCHING_H
#define GRAPHOS_FEATURE_MATCHING_H

#include "graphos/graphos_global.h"

#include "graphos/core/features/features.h"


namespace graphos
{

class GRAPHOS_EXPORT FeatureMatchingProperties
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


} // namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_H
