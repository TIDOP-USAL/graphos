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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "FeaturesModel.h"

#include <tidop/core/messages.h>


namespace graphos
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

} // namespace graphos
