#ifndef INSPECTOR_FEATURE_EXTRACTOR_MODEL_H
#define INSPECTOR_FEATURE_EXTRACTOR_MODEL_H

#include "inspector/ui/featextract/FeatureExtractor.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class FeatureExtractorModelImp
  : public FeatureExtractorModel
{

public:

  FeatureExtractorModelImp(Project *project,
                           QObject *parent = nullptr);
  ~FeatureExtractorModelImp() override = default;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

// FeatureExtractorModel interface

public:

  std::shared_ptr<Feature> featureExtractor() const override;
  void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) override;
  QString database() const override;
  void addFeatures(const QString &imageName, const QString &featuresFile) override;

protected:

  Project *mProject;

};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_MODEL_H
