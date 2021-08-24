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

#include "MatchViewerModel.h"

#include "graphos/core/project.h"

#include <colmap/base/database.h>

#include <QImageReader>
#include <QFileInfo>
#include <QSettings>

namespace graphos
{

MatchViewerModelImp::MatchViewerModelImp(Project *project,
                                         QObject *parent)
  : MatchViewerModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat, 
                            QSettings::UserScope, 
                            "TIDOP", 
                            "Graphos"))
{
  this->init();
}

MatchViewerModelImp::~MatchViewerModelImp()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }
}

QString MatchViewerModelImp::viewerBGColor() const
{
  return mSettings->value("MatchesViewer/BGColor", "#dcdcdc").toString();
}

int MatchViewerModelImp::viewerMarkerType() const
{
  return mSettings->value("MatchesViewer/MarkerType", 1).toInt();
}

int MatchViewerModelImp::viewerMarkerSize() const
{
  return mSettings->value("MatchesViewer/MarkerSize", 20).toInt();
}

int MatchViewerModelImp::viewerMarkerWidth() const
{
  return mSettings->value("MatchesViewer/MarkerWidth", 2).toInt();
}

QString MatchViewerModelImp::viewerMarkerColor() const
{
  return mSettings->value("MatchesViewer/MarkerColor", "#00aa00").toString();
}

int MatchViewerModelImp::viewerSelectMarkerWidth() const
{
  return mSettings->value("MatchesViewer/SelectMarkerWidth", 2).toInt();
}

QString MatchViewerModelImp::viewerSelectMarkerColor() const
{
  return mSettings->value("MatchesViewer/SelectMarkerColor", "#e5097e").toString();
}

QString MatchViewerModelImp::viewerLineColor() const
{
  return mSettings->value("MatchesViewer/LineColor", "#0000ff").toString();
}

int MatchViewerModelImp::viewerLineWidth() const
{
  return mSettings->value("MatchesViewer/LineWidth", 2).toInt();
}

void MatchViewerModelImp::init()
{
}

std::vector<QString> MatchViewerModelImp::images() const
{
  std::vector<QString> images;
  for (auto it = mProject->imageBegin(); it != mProject->imageEnd(); it++){
    images.push_back((*it).path());
  }
  return images;
}

std::vector<QString> MatchViewerModelImp::imagePairs(const QString &imageName) const
{
  std::vector<QString> pair_names = mProject->matchesPairs(imageName);
  std::vector<QString> image_pairs;
  for (auto &image_name : pair_names){
    image_pairs.push_back(mProject->findImageByName(image_name).path());
  }
  return image_pairs;
}

std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> 
MatchViewerModelImp::loadMatches(const QString &imgName1, 
                                 const QString &imgName2) const
{
  std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> matches;

  Image imageLeft = mProject->findImageByName(imgName1);
  Image imageRight = mProject->findImageByName(imgName2);
  //QString image_left_file_name = QFileInfo(imageLeft.path()).fileName();
  //QString image_right_file_name = QFileInfo(imageRight.path()).fileName();

  QString database_path = mProject->database();
  if (!QFileInfo(database_path).exists()) throw std::runtime_error("Database not found");

  colmap::Database database(database_path.toStdString());
  
  if (!database.ExistsImageWithName(imageLeft.path().toStdString())) throw std::runtime_error(std::string("Image not found in database: ").append(imageLeft.name().toStdString()));
  if (!database.ExistsImageWithName(imageRight.path().toStdString())) throw std::runtime_error(std::string("Image not found in database: ").append(imageRight.name().toStdString()));

  colmap::Image image_left_colmap = database.ReadImageWithName(imageLeft.path().toStdString());
  colmap::image_t image_left_id = image_left_colmap.ImageId();
  colmap::Image image_right_colmap = database.ReadImageWithName(imageRight.path().toStdString());
  colmap::image_t image_right_id = image_right_colmap.ImageId();
  
  if (image_left_id != 0 && image_right_id != 0) {
    colmap::FeatureKeypoints kp_1 = database.ReadKeypoints(image_left_id);
    colmap::FeatureKeypoints kp_2 = database.ReadKeypoints(image_right_id);

    colmap::TwoViewGeometry two_view_colmap = database.ReadTwoViewGeometry(image_left_id, image_right_id);
    colmap::FeatureMatches inlier_matches = two_view_colmap.inlier_matches;
    colmap::FeatureMatch match_colmap;

    for (size_t i = 0; i < inlier_matches.size(); i++) {
      match_colmap = inlier_matches[i];
      QPointF left_point(static_cast<qreal>(kp_1[match_colmap.point2D_idx1].x),
                         static_cast<qreal>(kp_1[match_colmap.point2D_idx1].y));
      QPointF right_point(static_cast<qreal>(kp_2[match_colmap.point2D_idx2].x),
                          static_cast<qreal>(kp_2[match_colmap.point2D_idx2].y));
      matches.push_back(std::make_tuple(i,
                        match_colmap.point2D_idx1,
                        left_point,
                        match_colmap.point2D_idx1,
                        right_point));
    }
  }

  return matches;
}

void MatchViewerModelImp::clear()
{
}

} // namespace graphos


