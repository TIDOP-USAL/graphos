/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "FeatureExtractorModel.h"

#include "graphos/core/project.h"

#include <QFileInfo>
#include <QSettings>

namespace graphos
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

void FeatureExtractorModelImp::setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor)
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
  return mSettings->value("UseCuda", true).toBool();
}

std::vector<Image> FeatureExtractorModelImp::images() const
{
  return mProject->images();
}

std::map<int, tl::Camera> FeatureExtractorModelImp::cameras() const
{
  return mProject->cameras();
}

void FeatureExtractorModelImp::clearProject()
{
  TL_TODO("Colmap no permite borrar la tabla de keypoints ni sobreescribirla asi que por ahora borro la base de datos completa")
  QFile(this->database()).remove();
  mProject->removeFeatures();
}

} // namespace graphos
