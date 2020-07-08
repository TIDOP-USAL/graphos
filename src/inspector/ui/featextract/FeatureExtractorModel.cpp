#include "FeatureExtractorModel.h"

#include "inspector/core/project.h"

#include <QFileInfo>

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
  TL_TODO("Colmap no permite borrar la tabla de keypoints ni sobreescribirla asi que por ahora borro la base de datos completa")
  QFile(this->database()).remove();
  mProject->removeFeatures();
  TL_TODO("Esto no tiene que estar aqui")
  mProject->removeMatchesPair();
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

void FeatureExtractorModelImp::addFeatures(const QString &imageName, const QString &featuresFile)
{
  mProject->addFeatures(imageName, featuresFile);
}

} // namespace ui

} // namespace inspector
