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

#include "FeaturesViewerModel.h"

#include "graphos/core/project.h"

#include <tidop/core/messages.h>

#include <colmap/base/database.h>

#include <QFileInfo>
#include <QSettings>

namespace graphos
{

FeaturesViewerModelImp::FeaturesViewerModelImp(Project *project,
                                               QObject *parent)
  : FeaturesViewerModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat, 
                            QSettings::UserScope, 
                            "TIDOP", 
                            "Graphos"))
{
  this->init();
}

FeaturesViewerModelImp::~FeaturesViewerModelImp()
{

}

void FeaturesViewerModelImp::init()
{
}

void FeaturesViewerModelImp::clear()
{

}

const std::unordered_map<size_t, Image> &FeaturesViewerModelImp::images() const
{
  return mProject->images();
}

Image FeaturesViewerModelImp::image(size_t imageId) const
{
  return mProject->findImageById(imageId);
}

std::vector<QPointF> FeaturesViewerModelImp::loadKeypoints(size_t imageId)
{
  std::vector<QPointF> keyPoints;

  try {

    Image image = mProject->findImageById(imageId);

    QString database_path = mProject->database();
    if (!QFileInfo(database_path).exists()) throw std::runtime_error("Database not found");
  
    colmap::Database database(database_path.toStdString());
  
    auto _images = database.ReadAllImages();
    for (const auto &image : _images) {
      std::string name = image.Name();
    }

    if (!database.ExistsImageWithName(image.path().toStdString()))
      throw std::runtime_error(std::string("Image not found in database: ").append(image.path().toStdString()));
  
    colmap::Image image_colmap = database.ReadImageWithName(image.path().toStdString());
    colmap::image_t image_id = image_colmap.ImageId();

    if (image_id > 0) {
      colmap::FeatureKeypoints colmap_feature_keypoints = database.ReadKeypoints(image_id);
      size_t size = colmap_feature_keypoints.size();
      keyPoints.resize(size);
      for (size_t i = 0; i < size; i++){
        keyPoints[i].setX(static_cast<qreal>(colmap_feature_keypoints[i].x));
        keyPoints[i].setY(static_cast<qreal>(colmap_feature_keypoints[i].y));
      }
    }
  } catch (std::exception &e) {
    msgError(e.what());
  }

  return keyPoints;
}

QString FeaturesViewerModelImp::viewerBGColor() const
{
  return mSettings->value("KeypointsViewer/BGColor", "#dcdcdc").toString();
}

int FeaturesViewerModelImp::viewerMarkerType() const
{
  return mSettings->value("KeypointsViewer/Type", 1).toInt();
}

int FeaturesViewerModelImp::viewerMarkerSize() const
{
  return mSettings->value("KeypointsViewer/MarkerSize", 20).toInt();
}

int FeaturesViewerModelImp::viewerMarkerWidth() const
{
  return mSettings->value("KeypointsViewer/MarkerWidth", 2).toInt();
}

QString FeaturesViewerModelImp::viewerMarkerColor() const
{
  return mSettings->value("KeypointsViewer/MarkerColor", "#00aa00").toString();
}

int FeaturesViewerModelImp::viewerSelectMarkerWidth() const
{
  return mSettings->value("KeypointsViewer/SelectMarkerWidth", 2).toInt();
}

QString FeaturesViewerModelImp::viewerSelectMarkerColor() const
{
  return mSettings->value("KeypointsViewer/SelectMarkerColor","#e5097e").toString();
}

} // namespace graphos
