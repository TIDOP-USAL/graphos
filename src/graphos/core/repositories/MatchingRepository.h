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

struct FeatureMatchPoint
{
    size_t id;
    size_t featureIndex1;
    tl::Point2f point1;
    size_t featureIndex2;
    tl::Point2f point2;
};

using Matches = std::vector<FeatureMatchPoint>;

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

    [[nodiscard]]
    auto hasMatches() const -> bool;
    [[nodiscard]]
    auto hasInlierMatches() const -> bool;

    [[nodiscard]]
    auto numMatches() const -> size_t;

    [[nodiscard]]
    auto numInlierMatches() const -> size_t;

    [[nodiscard]]
    auto loadMatches(const std::string &imageNameLeft, const std::string &imageNameRight) const -> Matches;

    void clear();

};

} // end namespace graphos
