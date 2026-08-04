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

#include "graphos/core/repositories/MatchingRepository.h"

#include <colmap/scene/database.h>


namespace graphos
{


auto MatchingRepository::hasMatches() const -> bool
{
    if (mDbPath.empty() || !mDbPath.exists()) return false;

    if (auto database = colmap::Database::Open(mDbPath.toUtf8())) {
        return database->NumMatches() > 0;
    }

    return false;
}

auto MatchingRepository::hasInlierMatches() const -> bool
{
    if (mDbPath.empty() || !mDbPath.exists()) return false;

    if (auto database = colmap::Database::Open(mDbPath.toUtf8())) {
        return database->NumInlierMatches() > 0;
    }

    return false;
}

auto MatchingRepository::numMatches() const -> size_t
{
    if (mDbPath.empty() || !mDbPath.exists()) return 0;

    if (auto database = colmap::Database::Open(mDbPath.toUtf8())) {
        return database->NumMatches();
    }

    return 0;
}

auto MatchingRepository::numInlierMatches() const -> size_t
{
    if (mDbPath.empty() || !mDbPath.exists()) return 0;

    if (auto database = colmap::Database::Open(mDbPath.toUtf8())) {
        return database->NumInlierMatches();
    }

    return 0;
}

auto MatchingRepository::loadMatches(const std::string &imageNameLeft, 
                                     const std::string &imageNameRight) const -> Matches
{
    Matches matches;

    TL_ASSERT(mDbPath.exists(), "Database not found: {}", mDbPath.toString());

    auto database = colmap::Database::Open(mDbPath.toUtf8());

    if (!database->ExistsImageWithName(imageNameLeft)) {
        TL_THROW_EXCEPTION("Image not found in database: {}", imageNameLeft);
    }
    if (!database->ExistsImageWithName(imageNameRight)) {
        TL_THROW_EXCEPTION("Image not found in database: {}", imageNameRight);
    }

    auto image_left = database->ReadImageWithName(imageNameLeft);
    auto image_right = database->ReadImageWithName(imageNameRight);

    if (image_left && image_right) {

        colmap::image_t id_left = image_left->ImageId();
        colmap::image_t id_right = image_right->ImageId();

        colmap::FeatureKeypoints kp_left = database->ReadKeypoints(id_left);
        colmap::FeatureKeypoints kp_right = database->ReadKeypoints(id_right);

        colmap::TwoViewGeometry two_view = database->ReadTwoViewGeometry(id_left, id_right);
        const auto &inliers = two_view.inlier_matches;

        matches.reserve(inliers.size());

        for (size_t i = 0; i < inliers.size(); ++i) {
            const auto &m = inliers[i];

            FeatureMatchPoint match_point;
            match_point.id = i;
            match_point.featureIndex1 = m.point2D_idx1;
            match_point.point1 = tl::Point2f(kp_left[m.point2D_idx1].x, kp_left[m.point2D_idx1].y);
            match_point.featureIndex2 = m.point2D_idx2;
            match_point.point2 = tl::Point2f(kp_right[m.point2D_idx2].x, kp_right[m.point2D_idx2].y);

            matches.push_back(match_point);
        }
    }

    return matches;
}

void MatchingRepository::clear()
{
    if (mDbPath.empty() || !mDbPath.exists()) return;

    if (auto database = colmap::Database::Open(mDbPath.toUtf8())) {
        database->ClearMatches();
        database->ClearTwoViewGeometries();
        database->Close();
    }
}

} // end namespace graphos
