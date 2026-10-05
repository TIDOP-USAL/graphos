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

#include "graphos/core/orientation/GroundControlPoint.h"

#include <tidop/core/base/Endian.h>
#include <tidop/core/base/Split.h>

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <fstream>


namespace graphos
{

GroundControlPoint::GroundControlPoint()
{
}

GroundControlPoint::GroundControlPoint(const tl::Point3d &point3d)
  : tl::Point3d(point3d)
{
}

GroundControlPoint::~GroundControlPoint() = default;

auto GroundControlPoint::name() const -> std::string
{
    return mName;
}

void GroundControlPoint::setName(const std::string &name)
{
    mName = name;
}

void GroundControlPoint::setPoint(const tl::Point3d &point)
{
    this->x() = point.x();
    this->y() = point.y();
    this->z() = point.z();
}

auto GroundControlPoint::error() const -> double/*tl::Vector3d*/
{
    return mError;
}

void GroundControlPoint::setError(/*const tl::Vector3d &*/double error)
{
    mError = error;
}

void GroundControlPoint::removeTrackPoint(size_t imageId)
{
    mTrack.removePoint(imageId);
}

void GroundControlPoint::addPointToTrack(size_t imageId, const tl::Point2d &point)
{
    mTrack.addPoint(imageId, point);
}

void GroundControlPoint::addErrorToTrack(size_t imageId, const tl::Vector2d &error)
{
    mTrack.setError(imageId, error);
}

void GroundControlPoint::setTrack(const GCPTrack &track)
{
    mTrack = track;
}

auto GroundControlPoint::track() const -> const GCPTrack&
{
    return mTrack;
}

}
