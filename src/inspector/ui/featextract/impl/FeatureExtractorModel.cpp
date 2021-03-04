#include "FeatureExtractorModel.h"

#include "inspector/core/project.h"

#include <QFileInfo>
#include <QSettings>

namespace inspector
{

namespace ui
{
	
FeatureExtractorModelImp::FeatureExtractorModelImp(Project *project,
                                                   QObject *parent)
  : FeatureExtractorModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat, 
                            QSettings::UserScope, 
                            "TIDOP", 
                            "Inspector"))
{
  this->init();
}

FeatureExtractorModelImp::~FeatureExtractorModelImp()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }
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

bool FeatureExtractorModelImp::useCuda() const
{
  return mSettings->value("General/UseCuda", true).toBool();
}

Camera FeatureExtractorModelImp::camera(int id) const
{
  return mProject->findCamera(id);
}

FeatureExtractorModel::image_iterator FeatureExtractorModelImp::imageBegin()
{
  return mProject->imageBegin();
}

FeatureExtractorModel::image_const_iterator FeatureExtractorModelImp::imageBegin() const
{
  return mProject->imageBegin();
}

FeatureExtractorModel::image_iterator FeatureExtractorModelImp::imageEnd()
{
  return mProject->imageEnd();
}

FeatureExtractorModel::image_const_iterator FeatureExtractorModelImp::imageEnd() const
{
  return mProject->imageEnd();
}

} // namespace ui

} // namespace inspector
