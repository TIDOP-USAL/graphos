#include "FeatureExtractorModel.h"

#include "graphos/core/project.h"

#include <QFileInfo>
#include <QSettings>

namespace graphos
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
                            "Graphos"))
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

}

std::shared_ptr<Feature> FeatureExtractorModelImp::featureExtractor() const
{
  return mProject->featureExtractor();
}

void graphos::ui::FeatureExtractorModelImp::setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor)
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

//Camera FeatureExtractorModelImp::camera(int id) const
//{
//  return mProject->findCamera(id);
//}

std::vector<Image> FeatureExtractorModelImp::images() const
{
  return mProject->images();
}

std::map<int, Camera> FeatureExtractorModelImp::cameras() const
{
  return mProject->cameras();
}

//FeatureExtractorModel::image_iterator FeatureExtractorModelImp::imageBegin()
//{
//  return mProject->imageBegin();
//}
//
//FeatureExtractorModel::image_const_iterator FeatureExtractorModelImp::imageBegin() const
//{
//  return mProject->imageBegin();
//}
//
//FeatureExtractorModel::image_iterator FeatureExtractorModelImp::imageEnd()
//{
//  return mProject->imageEnd();
//}
//
//FeatureExtractorModel::image_const_iterator FeatureExtractorModelImp::imageEnd() const
//{
//  return mProject->imageEnd();
//}

void FeatureExtractorModelImp::clearProject()
{
  TL_TODO("Colmap no permite borrar la tabla de keypoints ni sobreescribirla asi que por ahora borro la base de datos completa")
  QFile(this->database()).remove();
  mProject->removeFeatures();
}

} // namespace ui

} // namespace graphos
