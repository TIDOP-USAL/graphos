#ifndef INSPECTOR_FEATURE_EXTRACTOR_MODEL_H
#define INSPECTOR_FEATURE_EXTRACTOR_MODEL_H

#include "inspector/ui/featextract/FeatureExtractorModel.h"

class QSettings;

namespace inspector
{

class Project;
class Feature;

namespace ui
{

class FeatureExtractorModelImp
  : public FeatureExtractorModel
{

public:

  FeatureExtractorModelImp(Project *project,
                           QObject *parent = nullptr);
  ~FeatureExtractorModelImp() override;

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
  bool useCuda() const override;
  Camera camera(int id) const override;
  image_iterator imageBegin() override;
  image_const_iterator imageBegin() const override;
  image_iterator imageEnd() override;
  image_const_iterator imageEnd() const override;
  void clearProject() override;

protected:

  Project *mProject;
  QSettings *mSettings;
};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_MODEL_H
