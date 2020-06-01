#include "FeaturesModel.h"

#include <tidop/core/messages.h>


namespace inspector
{

namespace ui
{

FeaturesModelImp::FeaturesModelImp(Project *project,
                                   QObject *parent)
  : FeaturesModel(parent),
    mProject(project)
{
  init();
}

void FeaturesModelImp::init()
{
}

void FeaturesModelImp::clear()
{
  for (auto it = begin(); it != end();){
    removeFeatures(it->first);
    it = begin();
  }
}

QString FeaturesModelImp::features(const QString &imgName) const
{
  return mProject->features(imgName);
}

void FeaturesModelImp::addFeatures(const QString &imgName, const QString &featureFile)
{
  mProject->addFeatures(imgName, featureFile);
}

bool FeaturesModelImp::removeFeatures(const QString &imgName) const
{
  return mProject->removeFeatures(imgName);
}

FeaturesModel::features_iterator FeaturesModelImp::begin()
{
  return mProject->featuresBegin();
}

FeaturesModel::features_const_iterator FeaturesModelImp::begin() const
{
  return mProject->featuresBegin();
}

FeaturesModel::features_iterator FeaturesModelImp::end()
{
  return mProject->featuresEnd();
}

FeaturesModel::features_const_iterator FeaturesModelImp::end() const
{
  return mProject->featuresEnd();
}

} // namespace ui

} // namespace inspector
