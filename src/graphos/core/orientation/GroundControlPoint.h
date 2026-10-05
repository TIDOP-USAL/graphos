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

#include <memory>
#include <unordered_map>

#include <tidop/core/base/Path.h>
#include <tidop/geometry/primitives/Point.h>
#include <tidop/graphic/Color.h>

#include "graphos/core/orientation/Track.h"
#include "graphos/core/image/Image.h"


namespace graphos
{


/*!
 * \brief Ground Control Points
 *
 * This class represents a ground control point, which is a point in 3D space.
 * A ground control point has an associated track with the corresponding points
 * on the images.
 * It inherits from tl::Point3d, which provides the x, y, and z coordinates.
 */
class GroundControlPoint
  : public tl::Point3d
{

public:

    /*!
     * \brief Default constructor.
     */
    GroundControlPoint();

    /*!
     * \brief Constructor with a 3D point.
     * \param[in] point3d A point in 3D space.
     */
    explicit GroundControlPoint(const tl::Point3d &point3d);

    /*!
     * \brief Destructor.
     */
    ~GroundControlPoint();

    /*!
     * \brief Get the name of the ground control point.
     * \return The name of the ground control point.
     */
    auto name() const -> std::string;

    /*!
     * \brief Set the name of the ground control point.
     * \param[in] name The name to set.
     */
    void setName(const std::string &name);

    /*!
     * \brief Set the 3D point of the ground control point.
     * \param[in] point The 3D point to set.
     */
    void setPoint(const tl::Point3d &point);

    auto error() const -> double/*tl::Vector3d*/;

    void setError(/*const tl::Vector3d &*/double error);

    /*!
     * \brief Add a point to the track of the ground control point.
     * \param imageId The ID of the image.
     * \param point The point to add to the track.
     */
    void addPointToTrack(size_t imageId, const tl::Point2d &point);

    void addErrorToTrack(size_t imageId, const tl::Vector2d &error);

    /*!
     * \brief Set the track of the ground control point.
     * \param[in] track The track to set.
     * \see GCPTrack
     */
    void setTrack(const GCPTrack &track);

    /*!
     * \brief Remove a point from the track of the ground control point.
     * \param[in] imageId The ID of the image.
     */
    void removeTrackPoint(size_t imageId);

    /*!
     * \brief Get the track of the ground control point.
     * \return The track of the ground control point.
     * \see GCPTrack
     */
    auto track() const -> const GCPTrack&;

private:

    std::string mName;  ///< The name of the ground control point.
    GCPTrack mTrack;    ///< The track of the ground control point.
    /*tl::Vector3d*/double mError;
};

} // namespace graphos
