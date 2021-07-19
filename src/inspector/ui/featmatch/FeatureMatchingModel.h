#ifndef INSPECTOR_FEATURE_MATCHING_MODEL_INTERFACE_H
#define INSPECTOR_FEATURE_MATCHING_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{
	
class FeatureMatching;

namespace ui
{


class FeatureMatchingModel
  : public Model
{

  Q_OBJECT

public:

  FeatureMatchingModel(QObject *parent = nullptr) : Model(parent) {}
  ~FeatureMatchingModel() override = default;

  virtual std::shared_ptr<FeatureMatching> featureMatching() const = 0;
  virtual void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) = 0;
  virtual QString database() const = 0;
  virtual bool useCuda() const = 0;
  virtual bool spatialMatching() const = 0;
  virtual void writeMatchPairs() = 0;
  virtual bool existsMatches() const = 0;
  virtual void clearProject() = 0;
};

} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_MODEL_INTERFACE_H
