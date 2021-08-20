#ifndef GRAPHOS_FEATURE_EXTRACTOR_MODEL_INTERFACE_H
#define GRAPHOS_FEATURE_EXTRACTOR_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/camera.h"
#include "graphos/core/image.h"

namespace graphos
{

class Feature;

namespace ui
{

class FeatureExtractorModel
  : public Model
{
//public:
//
//  typedef std::vector<Image>::iterator image_iterator;
//  typedef std::vector<Image>::const_iterator image_const_iterator;

public:

  FeatureExtractorModel(QObject *parent = nullptr) : Model(parent) {}
  ~FeatureExtractorModel() override = default;

  virtual std::shared_ptr<Feature> featureExtractor() const = 0;
  virtual void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) = 0;
  virtual QString database() const = 0;
  virtual void addFeatures(const QString &imageName, const QString &featuresFile) = 0;
  virtual bool useCuda() const = 0;
  //virtual Camera camera(int id) const = 0;
  virtual std::vector<Image> images() const = 0;
  virtual std::map<int, Camera> cameras() const = 0;
  //virtual image_iterator imageBegin() = 0;
  //virtual image_const_iterator imageBegin() const = 0;
  //virtual image_iterator imageEnd() = 0;
  //virtual image_const_iterator imageEnd() const = 0;
  virtual void clearProject() = 0;
};


} // namespace ui

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_MODEL_INTERFACE_H
