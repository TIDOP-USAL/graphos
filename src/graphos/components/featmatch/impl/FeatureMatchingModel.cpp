/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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
                            qApp->organizationName(), 
                            qApp->applicationName()))
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
  return mSettings->value("UseCuda", true).toBool();
}

bool FeatureMatchingModelImp::spatialMatching() const
{
  bool bSpatialMatching = false;

  auto it = mProject->images().begin();
  if (it != mProject->images().end()){
    CameraPose cameraPosition = it->second.cameraPose();
    if (!cameraPosition.isEmpty())
      bSpatialMatching = true;
  }

  return bSpatialMatching;
}

void FeatureMatchingModelImp::writeMatchPairs()
{

  QString database_file = mProject->database();
  colmap::Database database(database_file.toStdString());
  std::vector<colmap::Image> db_images = database.ReadAllImages();
  colmap::image_t colmap_image_id_l = 0;
  colmap::image_t colmap_image_id_r = 0;

  for (size_t i = 0; i < db_images.size(); i++) {

    colmap_image_id_l = db_images[i].ImageId();

    for (size_t j = 0; j < i; j++) {
      colmap_image_id_r = db_images[j].ImageId();

      colmap::FeatureMatches matches = database.ReadMatches(colmap_image_id_l, colmap_image_id_r);

      if (matches.size() > 0) {
        
        tl::Path colmap_image1(db_images[i].Name());
        tl::Path colmap_image2(db_images[j].Name());
        
        bool find_id_1 = false;
        bool find_id_2 = false;
        size_t image_id_1 = 0;
        size_t image_id_2 = 0;

        for (const auto &image_pair : mProject->images()) {
          
          tl::Path image(image_pair.second.path().toStdString());
          
          if (!find_id_1 && image.equivalent(colmap_image1)) {
            find_id_1 = true;
            image_id_1 = image_pair.first;
          }

          if (!find_id_2 && image.equivalent(colmap_image2)) {
            find_id_2 = true;
            image_id_2 = image_pair.first;
          }

          if (find_id_1 && find_id_2) {
            mProject->addMatchesPair(image_id_1, image_id_2);
          }
        }

      }

    }

  }

  database.Close();

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
