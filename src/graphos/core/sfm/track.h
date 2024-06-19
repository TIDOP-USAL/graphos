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

#include <unordered_map>

#include <tidop/geometry/entities/point.h>

namespace graphos
{


/*!
 * \brief Represents a track of image points
 *
 * The `Track` class represents a track of image points, where each point is identified by its image ID and point ID.
 * It maintains a mapping of image IDs to point IDs.
 */
class Track
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs an empty `Track` object.
     */
    Track();
    ~Track();

    /*!
     * \brief Get the number of points in the track.
     *
     * \return The number of points in the track.
     */
    auto size() const -> size_t;

    /*!
     * \brief Get the point ID at the specified index.
     *
     * \param[in] idx The index of the point.
     * \return The point ID at the specified index.
     */
    auto pointId(size_t idx) const -> size_t;

    /*!
     * \brief Get the pairs of image IDs and point IDs in the track.
     *
     * \return A reference to the unordered map containing image IDs and point IDs.
     */
    auto pairs() const -> const std::unordered_map<size_t, size_t>&;

    /*!
     * \brief Add a pair of image ID and point ID to the track.
     *
     * \param[in] imageId The image ID of the point.
     * \param[in] pointId The point ID.
     */
    void addPair(size_t imageId, size_t pointId);

    /*!
     * \brief Check if a pair with the specified image ID exists in the track.
     *
     * \param[in] imageId The image ID to check.
     * \return True if a pair with the image ID exists, false otherwise.
     */
    auto existPair(size_t imageId) const -> bool;

    /*!
     * \brief Remove the pair with the specified image ID from the track.
     *
     * \param[in] imageId The image ID of the pair to remove.
     */
    void removePair(size_t imageId);

private:

    std::unordered_map<size_t, size_t> mPairs;

};



/*!
 * \brief Represents a track of ground control points (GCPs)
 *
 * The `GCPTrack` class represents a track of ground control points (GCPs), where each point is associated with an
 * image ID and a point in 3D space. It maintains a mapping of image IDs to 3D points.
 */
class GCPTrack
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs an empty `GCPTrack` object.
     */
    GCPTrack();
    ~GCPTrack();

    /*!
     * \brief Get the number of points in the track.
     *
     * \return The number of points in the track.
     */
    auto size() const -> size_t;

    /*!
     * \brief Get the 3D point at the specified index.
     *
     * \param idx The index of the point.
     * \return The 3D point at the specified index.
     */
    auto point(size_t idx) const -> tl::Point<double>;

    /*!
     * \brief Get the mapping of image IDs to 3D points in the track.
     *
     * \return A reference to the unordered map containing image IDs and 3D points.
     */
    auto points() const -> const std::unordered_map<size_t, tl::Point<double>>&;

    /*!
     * \brief Add a 3D point associated with an image ID to the track.
     *
     * \param imageId The image ID associated with the point.
     * \param point The 3D point to add.
     */
    void addPoint(size_t imageId, const tl::Point<double> &point);

    /*!
     * \brief Check if a point with the specified image ID exists in the track.
     *
     * \param imageId The image ID to check.
     * \return True if a point with the image ID exists, false otherwise.
     */
    auto existPoint(size_t imageId) const -> bool;

    /*!
     * \brief Remove the point with the specified image ID from the track.
     *
     * \param imageId The image ID of the point to remove.
     */
    void removePoint(size_t imageId);

private:

    std::unordered_map<size_t, tl::Point<double>> mImageIdPoint;
};


} // namespace graphos

#endif // GRAPHOS_CORE_SFM_TRACK_H
