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

#pragma once

#include "graphos/graphos_global.h"

#include <tidop/core/base/Path.h>

#include "graphos/core/Image.h"
#include "graphos/core/repositories/Repository.h"

namespace graphos
{

class MatchingRepository
{
private:

    tl::Path mDbPath;

public:

    MatchingRepository() = default;

    explicit MatchingRepository(tl::Path dbPath)
      : mDbPath(std::move(dbPath))
    {
    }

    void setDatabase(tl::Path database) { mDbPath = std::move(database); }
    //[[nodiscard]]
    //auto allPairs() const -> std::vector<ImagePair>
    //{
    //    std::vector<ImagePair> pairs;
    //    sqlite3 *db = openDatabase();
    //    if (!db) return pairs;

    //    // two_view_geometries almacena pair_id y rows (número de inliers)
    //    const char *sql = "SELECT pair_id FROM two_view_geometries WHERE rows > 0;";
    //    sqlite3_stmt *stmt = nullptr;

    //    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
    //        while (sqlite3_step(stmt) == SQLITE_ROW) {
    //            uint64_t pair_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 0));
    //            auto [id1, id2] = ColmapPairUtils::pairIdToImagePair(pair_id);
    //            pairs.emplace_back(id1, id2);
    //        }
    //    }

    //    sqlite3_finalize(stmt);
    //    sqlite3_close(db);
    //    return pairs;
    //}

    // Comprueba si dos imágenes tienen coincidencias guardadas en la DB
    //[[nodiscard]]
    //auto hasPair(uint64_t id1, uint64_t id2) const -> bool
    //{
    //    sqlite3 *db = openDatabase();
    //    if (!db) return false;

    //    uint64_t pair_id = ColmapPairUtils::imagePairToPairId(id1, id2);
    //    const char *sql = "SELECT COUNT(*) FROM two_view_geometries WHERE pair_id = ? AND rows > 0;";

    //    sqlite3_stmt *stmt = nullptr;
    //    bool exists = false;

    //    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
    //        sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(pair_id));
    //        if (sqlite3_step(stmt) == SQLITE_ROW) {
    //            exists = sqlite3_column_int(stmt, 0) > 0;
    //        }
    //    }

    //    sqlite3_finalize(stmt);
    //    sqlite3_close(db);
    //    return exists;
    //}

    // Devuelve los IDs de imágenes que solapan con 'image_id'
    //[[nodiscard]]
    //auto adjacentImages(uint64_t image_id) const -> std::vector<uint64_t>
    //{
    //    std::vector<uint64_t> neighbors;

    //    // Obtenemos todos los pares y filtramos los que contienen 'image_id'
    //    // (Para optimizar esto a nivel de SQL se pueden hacer desplazamientos de bits o consultar directos)
    //    for (const auto &pair : allPairs()) {
    //        if (pair.image_id1 == image_id) {
    //            neighbors.push_back(pair.image_id2);
    //        } else if (pair.image_id2 == image_id) {
    //            neighbors.push_back(pair.image_id1);
    //        }
    //    }
    //    return neighbors;
    //}

    //[[nodiscard]]
    //auto count() const -> size_t
    //{
    //    return allPairs().size();
    //}

    // --- Métodos de escritura deshabilitados ---
    // (COLMAP maneja la escritura en su proceso de matching)
    //void addPair(uint64_t, uint64_t) override {}
    //void addPairs(const std::vector<ImagePair> &) override {}
    //bool removePair(uint64_t, uint64_t) override { return false; }

    void clear();

private:

    //sqlite3 *openDatabase() const
    //{
    //    sqlite3 *db = nullptr;
    //    if (sqlite3_open_v2(mDbPath.string().c_str(), &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
    //        if (db) sqlite3_close(db);
    //        return nullptr;
    //    }
    //    return db;
    //}

};

} // end namespace graphos
