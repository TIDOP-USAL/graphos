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

#include "FeatureMatchingModel.h"

#include <tidop/core/messages.h>

#include <colmap/base/database.h>

#include <QFileInfo>
#include <QSettings>

namespace graphos
{

FeatureMatchingModelImp::FeatureMatchingModelImp(Project *project,
                                                 QObject *parent)
  : FeatureMatchingModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat, 
                            QSettings::UserScope, 
                            "TIDOP", 
                            "Graphos"))
{
  this->init();
}

FeatureMatchingModelImp::~FeatureMatchingModelImp()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }
}

void FeatureMatchingModelImp::init()
{
}

void FeatureMatchingModelImp::clear()
{

}

std::shared_ptr<FeatureMatching> FeatureMatchingModelImp::featureMatching() const
{
  return mProject->featureMatching();
}

void FeatureMatchingModelImp::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
  mProject->setFeatureMatching(featureMatching);
}

QString FeatureMatchingModelImp::database() const
{
  return mProject->database();
}

bool FeatureMatchingModelImp::useCuda() const
{
  return mSettings->value("General/UseCuda", true).toBool();
}

bool FeatureMatchingModelImp::spatialMatching() const
{
  bool bSpatialMatching = false;

  auto it = mProject->imageBegin();
  CameraPosition cameraPosition = it->cameraPosition();
  if (!cameraPosition.isEmpty())
    bSpatialMatching = true;

  return bSpatialMatching;
}

void FeatureMatchingModelImp::writeMatchPairs()
{
  QString database_file = mProject->database();
  colmap::Database database(database_file.toStdString());
  std::vector<colmap::Image> db_images = database.ReadAllImages();
  colmap::image_t image_id_l = 0;
  colmap::image_t image_id_r = 0;
  for (size_t i = 0; i < db_images.size(); i++){
    image_id_l = db_images[i].ImageId();
    for (size_t j = 0; j < i; j++){
      image_id_r = db_images[j].ImageId();

      colmap::FeatureMatches matches = database.ReadMatches(image_id_l, image_id_r);
      if (matches.size() > 0){
        QString path_left = QFileInfo(db_images[i].Name().c_str()).baseName();
        QString path_right = QFileInfo(db_images[j].Name().c_str()).baseName();
        mProject->addMatchesPair(path_left, path_right);
      }
    }
  }
}

bool FeatureMatchingModelImp::existsMatches() const
{
  QString database_file = mProject->database();
  colmap::Database database(database_file.toStdString());
  size_t num_matches = database.NumMatches();
  return num_matches > 0;
}

void FeatureMatchingModelImp::clearProject()
{
  QString database_file = mProject->database();
  colmap::Database database(database_file.toStdString());
  database.ClearMatches();
  database.ClearTwoViewGeometries();
  database.Close();
  mProject->removeMatchesPair();
}

} // End namespace graphos
