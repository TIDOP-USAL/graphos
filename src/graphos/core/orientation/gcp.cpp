/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#include "graphos/core/orientation/gcp.h"

namespace graphos
{

GroundControlPoint::GroundControlPoint()
{

}

GroundControlPoint::~GroundControlPoint()
{
}

std::string GroundControlPoint::name() const
{
  return mName;
}

void GroundControlPoint::setName(const std::string &name)
{
  mName = name;
}

tl::Point3D GroundControlPoint::point() const
{
  return mCoordinates;
}

void GroundControlPoint::setPoint(const tl::Point3D &point)
{
  mCoordinates = point;
}

double GroundControlPoint::x() const
{
  return mCoordinates.x;
}

void GroundControlPoint::setX(double x)
{
  mCoordinates.x = x;
}

double GroundControlPoint::y() const
{
  return mCoordinates.y;
}

void GroundControlPoint::setY(double y)
{
  mCoordinates.y = y;
}

double GroundControlPoint::z() const
{
  return mCoordinates.z;
}

void GroundControlPoint::setZ(double z)
{
  mCoordinates.z = z;
}

void GroundControlPoint::addImagePoint(const std::string &image, const tl::PointD &point)
{
  mPoints[image] = point;
}

tl::PointD GroundControlPoint::imagePoint(const std::string &image)
{
  return mPoints[image];
}

bool GroundControlPoint::existImagePoint(const std::string &image)
{
  auto it = mPoints.find(image);
  
  return (it != mPoints.end());
}

}
