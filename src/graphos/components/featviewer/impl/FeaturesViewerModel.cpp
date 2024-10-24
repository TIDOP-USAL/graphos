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

#include <tidop/core/msg/message.h>

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
                            qApp->organizationName(),
                            qApp->applicationName()))
{
    FeaturesViewerModelImp::init();
}

FeaturesViewerModelImp::~FeaturesViewerModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void FeaturesViewerModelImp::init()
{
}

void FeaturesViewerModelImp::clear()
{

}

auto FeaturesViewerModelImp::images() const -> const std::unordered_map<size_t, Image>&
{
    return mProject->images();
}

auto FeaturesViewerModelImp::image(size_t imageId) const -> Image
{
    return mProject->findImageById(imageId);
}

auto FeaturesViewerModelImp::loadKeypoints(size_t imageId) -> std::vector<std::tuple<QPointF, float, float>>
{
    std::vector<std::tuple<QPointF, float, float>> keyPoints;

    try {

        Image image = mProject->findImageById(imageId);

        tl::Path database_path = mProject->database();

        TL_ASSERT(database_path.exists(), "Database not found");

        colmap::Database database(database_path.toString());

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

            for (size_t i = 0; i < size; i++) {

                QPointF keypoint(static_cast<qreal>(colmap_feature_keypoints[i].x),
                                 static_cast<qreal>(colmap_feature_keypoints[i].y));

                keyPoints[i] = std::make_tuple(keypoint,
                                               colmap_feature_keypoints[i].ComputeScale(),
                                               colmap_feature_keypoints[i].ComputeOrientation() * tl::consts::rad_to_deg<float>);
            }
        }
    } catch (std::exception &e) {
        tl::printException(e);
    }

    return keyPoints;
}

auto FeaturesViewerModelImp::backgroundColor() const -> QString
{
    return mSettings->value("KeypointsViewer/BackgroundColor", "#dcdcdc").toString();
}

auto FeaturesViewerModelImp::markerType() const -> int
{
    return mSettings->value("KeypointsViewer/Type", 1).toInt();
}

auto FeaturesViewerModelImp::markerSize() const -> int
{
    return mSettings->value("KeypointsViewer/MarkerSize", 20).toInt();
}

auto FeaturesViewerModelImp::markerWidth() const -> int
{
    return mSettings->value("KeypointsViewer/MarkerWidth", 2).toInt();
}

auto FeaturesViewerModelImp::markerColor() const -> QString
{
    return mSettings->value("KeypointsViewer/MarkerColor", "#00aa00").toString();
}

auto FeaturesViewerModelImp::selectedMarkerWidth() const -> int
{
    return mSettings->value("KeypointsViewer/SelectMarkerWidth", 2).toInt();
}

auto FeaturesViewerModelImp::selectedMarkerColor() const -> QString
{
    return mSettings->value("KeypointsViewer/SelectMarkerColor", "#e5097e").toString();
}

} // namespace graphos
