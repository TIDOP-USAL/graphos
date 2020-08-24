#ifndef INSPECTOR_FEATURE_MATCHING_MODEL_H
#define INSPECTOR_FEATURE_MATCHING_MODEL_H

#include <QObject>

#include "inspector/ui/featmatch/FeatureMatching.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class FeatureMatchingModelImp
  : public FeatureMatchingModel
{

public:

  FeatureMatchingModelImp(Project *project,
                          QObject *parent = nullptr);
  ~FeatureMatchingModelImp() override;

// FeatureMatchingModel interface

public:

  std::shared_ptr<FeatureMatching> featureMatching() const override;
  void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) override;
  QString database() const override;
  bool spatialMatching() const override;
  void writeMatchPairs() override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
};

} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_MODEL_H
