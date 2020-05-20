#include "FeatureExtractorModel.h"

namespace inspector
{

namespace ui
{
	
FeatureExtractorModelImp::FeatureExtractorModelImp(Project *project,
                                                   QObject *parent)
  : FeatureExtractorModel(parent),
    mProject(project)
{
  this->init();
}

void FeatureExtractorModelImp::init()
{

}

void FeatureExtractorModelImp::clear()
{

}

std::shared_ptr<Feature> FeatureExtractorModelImp::featureExtractor() const
{
  return mProject->featureExtractor();
}

void inspector::ui::FeatureExtractorModelImp::setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor)
{
  mProject->setFeatureExtractor(featureExtractor);
}

QString FeatureExtractorModelImp::database() const
{
  return mProject->database();
}

} // namespace ui

} // namespace inspector
