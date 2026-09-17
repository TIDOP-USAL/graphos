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

#include "graphos/core/matching/MatchingRepository.h"

#include <colmap/scene/database.h>
#include <colmap/util/types.h>

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

auto MatchingRepository::imagePairs(const std::string &imageName) const -> std::vector<std::string>
{
    std::vector<std::string> pairedImageNames;

    if (!mDbPath.exists()) return pairedImageNames;

    auto database = colmap::Database::Open(mDbPath.toUtf8());

    if (!database->ExistsImageWithName(imageName)) {
        return pairedImageNames;
    }

    // 1. Obtener la imagen objetivo y su ID interno en COLMAP
    const auto targetImage = database->ReadImageWithName(imageName);
    const colmap::image_t targetId = targetImage->ImageId();

    // 2. Crear un mapa local ID -> Name para resolver la ruta rápida al iterar las coincidencias
    const auto allImages = database->ReadAllImages();
    std::unordered_map<colmap::image_t, std::string> idToNameMap;
    idToNameMap.reserve(allImages.size());
    for (const auto &img : allImages) {
        idToNameMap[img.ImageId()] = img.Name();
    }

    // 3. Buscar todas las geometrías de dos vistas y filtrar las que conectan con targetId
    const auto twoViewGeometries = database->ReadTwoViewGeometries();

    for (const auto &geometry : twoViewGeometries) {
        auto [id1, id2] = colmap::PairIdToImagePair(geometry.first);

        // Solo se cuentan pares que tengan inliers válidos
        if (!geometry.second.inlier_matches.empty()) {
            if (id1 == targetId && idToNameMap.count(id2)) {
                pairedImageNames.push_back(idToNameMap[id2]);
            } else if (id2 == targetId && idToNameMap.count(id1)) {
                pairedImageNames.push_back(idToNameMap[id1]);
            }
        }
    }

    return pairedImageNames;
}

auto MatchingRepository::hasImagePairs(const std::string &imageName) const -> bool
{
    if (mDbPath.empty() || !mDbPath.exists()) return false;

    auto database = colmap::Database::Open(mDbPath.toUtf8());
    const auto image = database->ReadImageWithName(imageName);
    if (!image) return false;

    const colmap::image_t imageId = image->ImageId();
    for (const auto &[pairId, numInliers] : database->ReadTwoViewGeometryNumInliers()) {
        if (numInliers <= 0) continue;

        const auto [imageId1, imageId2] = colmap::PairIdToImagePair(pairId);
        if (imageId1 == imageId || imageId2 == imageId) {
            return true;
        }
    }

    return false;
}

auto MatchingRepository::imageNamesWithPairs() const -> std::unordered_set<std::string>
{
    std::unordered_set<std::string> imageNames;

    if (mDbPath.empty() || !mDbPath.exists()) return imageNames;

    auto database = colmap::Database::Open(mDbPath.toUtf8());
    const auto images = database->ReadAllImages();
    const auto geometries = database->ReadTwoViewGeometryNumInliers();

    std::unordered_map<colmap::image_t, std::string> imageNamesById;
    imageNamesById.reserve(images.size());
    for (const auto &image : images) {
        imageNamesById.emplace(image.ImageId(), image.Name());
    }

    for (const auto &[pairId, numInliers] : geometries) {
        if (numInliers <= 0) continue;

        const auto [imageId1, imageId2] = colmap::PairIdToImagePair(pairId);
        if (const auto image = imageNamesById.find(imageId1); image != imageNamesById.end()) {
            imageNames.insert(image->second);
        }
        if (const auto image = imageNamesById.find(imageId2); image != imageNamesById.end()) {
            imageNames.insert(image->second);
        }
    }

    return imageNames;
}

void MatchingRepository::clear()
{
    mDbPath.clear();
}

void MatchingRepository::removeMatches()
{
    if (mDbPath.empty() || !mDbPath.exists()) return;

    if (auto database = colmap::Database::Open(mDbPath.toUtf8())) {
        database->ClearMatches();
        database->ClearTwoViewGeometries();
        database->Close();
    }
}

} // end namespace graphos
