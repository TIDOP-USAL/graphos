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

#ifndef GRAPHOS_CORE_SFM_TRACK_H
#define GRAPHOS_CORE_SFM_TRACK_H

#include "graphos/graphos_global.h"

#include <vector>
#include <unordered_map>

#include <tidop/geometry/entities/point.h>

namespace graphos
{


/*!
 * \brief Track
 *
 * [image_id_1 point_id_1 image_id_2 point_id_2 ...]
 *
 */
class Track
{

public:

    Track();
    ~Track();

    size_t size() const;

    size_t pointId(size_t idx);
    const std::unordered_map<size_t, size_t> &pairs() const;

    void addPair(size_t imageId, size_t pointId);
    bool existPair(size_t imageId) const;
    void removePair(size_t imageId);

private:

    std::unordered_map<size_t, size_t> mPairs;

};


class GCPTrack
{

public:

    GCPTrack();
    ~GCPTrack();

    size_t size() const;

    tl::Point<double> point(size_t idx) const;
    const std::unordered_map<size_t, tl::Point<double>> &points() const;

    void addPoint(size_t imageId, const tl::Point<double> &point);
    bool existPoint(size_t imageId) const;
    void removePoint(size_t imageId);

private:

    std::unordered_map<size_t, tl::Point<double>> mImageIdPoint;
};


} // namespace graphos

#endif // GRAPHOS_CORE_SFM_TRACK_H
