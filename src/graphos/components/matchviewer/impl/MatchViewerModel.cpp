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

#include "MatchViewerModel.h"

#include "graphos/core/project/Project.h"
#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/matching/MatchingRepository.h"

#include <colmap/scene/database.h>

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
              qApp->organizationName(),
              qApp->applicationName()))
{
    MatchViewerModelImp::init();
}

MatchViewerModelImp::~MatchViewerModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

auto MatchViewerModelImp::backgroundColor() const -> QString
{
    return mSettings->value("MatchesViewer/BackgroundColor", "#dcdcdc").toString();
}

auto MatchViewerModelImp::markerType() const -> int
{
    return mSettings->value("MatchesViewer/MarkerType", 1).toInt();
}

auto MatchViewerModelImp::markerSize() const -> int
{
    return mSettings->value("MatchesViewer/MarkerSize", 20).toInt();
}

auto MatchViewerModelImp::markerWidth() const -> int
{
    return mSettings->value("MatchesViewer/MarkerWidth", 2).toInt();
}

auto MatchViewerModelImp::markerColor() const -> QString
{
    return mSettings->value("MatchesViewer/MarkerColor", "#00aa00").toString();
}

auto MatchViewerModelImp::viewerSelectMarkerWidth() const -> int
{
    return mSettings->value("MatchesViewer/SelectMarkerWidth", 2).toInt();
}

auto MatchViewerModelImp::selectedMarkerColor() const -> QString
{
    return mSettings->value("MatchesViewer/SelectMarkerColor", "#e5097e").toString();
}

auto MatchViewerModelImp::lineColor() const -> QString
{
    return mSettings->value("MatchesViewer/LineColor", "#0000ff").toString();
}

auto MatchViewerModelImp::lineWidth() const -> int
{
    return mSettings->value("MatchesViewer/LineWidth", 2).toInt();
}

void MatchViewerModelImp::init()
{
}

auto MatchViewerModelImp::images() const -> const ImageRepository &
{
    return mProject->images();
}

auto MatchViewerModelImp::findImage(size_t imageId) const -> const Image *
{
    return mProject->images().find(imageId);
}

auto MatchViewerModelImp::imagePairs(size_t imageId) const -> std::vector<size_t>
{
    std::vector<size_t> image_pairs;
    
    const Image *image = mProject->images().find(imageId);
    if (image) {
        std::vector<std::string> pair_names = mProject->matches().imagePairs(image->path().toString());
        for (const auto &pair_name : pair_names) {
            for (const auto &img_pair : mProject->images()) {
                if (img_pair.second.path().toString() == pair_name) {
                    image_pairs.push_back(img_pair.first);
                    break;
                }
            }
        }
    }
    
    return image_pairs;
}

auto MatchViewerModelImp::loadMatches(size_t imageId1, size_t imageId2) const -> Matches
{
    std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> matches;

    const Image *image_left = mProject->images().find(imageId1);
    const Image *image_right = mProject->images().find(imageId2);
    
    TL_ASSERT(image_left, "Image left not found");
    TL_ASSERT(image_right, "Image right not found");

    tl::Path database_path = mProject->info().database();

    TL_ASSERT(database_path.exists(), "Database not found");

    auto database = colmap::Database::Open(database_path.toUtf8());

    if (!database->ExistsImageWithName(image_left->path().toString()))
        throw std::runtime_error(std::string("Image not found in database: ").append(image_left->name().toStdString()));
    if (!database->ExistsImageWithName(image_right->path().toString()))
        throw std::runtime_error(std::string("Image not found in database: ").append(image_right->name().toStdString()));

    if (auto image_left_colmap = database->ReadImageWithName(image_left->path().toString())) {
        colmap::image_t image_left_id = image_left_colmap->ImageId();
        
        if (auto image_right_colmap = database->ReadImageWithName(image_right->path().toString())) {
            colmap::image_t image_right_id = image_right_colmap->ImageId();

            if (image_left_id > 0 && image_right_id > 0) {
                colmap::FeatureKeypoints kp_1 = database->ReadKeypoints(image_left_id);
                colmap::FeatureKeypoints kp_2 = database->ReadKeypoints(image_right_id);

                colmap::TwoViewGeometry two_view_colmap = database->ReadTwoViewGeometry(image_left_id, image_right_id);
                colmap::FeatureMatches inlier_matches = two_view_colmap.inlier_matches;
                colmap::FeatureMatch match_colmap;

                for (size_t i = 0; i < inlier_matches.size(); i++) {
                    match_colmap = inlier_matches[i];
                    QPointF left_point(static_cast<qreal>(kp_1[match_colmap.point2D_idx1].x),
                                       static_cast<qreal>(kp_1[match_colmap.point2D_idx1].y));
                    QPointF right_point(static_cast<qreal>(kp_2[match_colmap.point2D_idx2].x),
                                        static_cast<qreal>(kp_2[match_colmap.point2D_idx2].y));
                    matches.emplace_back(i, match_colmap.point2D_idx1, left_point,
                                         match_colmap.point2D_idx1, right_point);
                }
            }
        }
    }

    return matches;
}

void MatchViewerModelImp::clear()
{
}

} // namespace graphos


