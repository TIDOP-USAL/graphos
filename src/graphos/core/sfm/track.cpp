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

#include "graphos/core/sfm/track.h"

namespace graphos
{

Track::Track(){}

Track::~Track(){}

size_t Track::size() const
{
  return mPairs.size();
}

std::pair<size_t, size_t> Track::at(size_t idx)
{
  return mPairs.at(idx);
}

void Track::push_back(const std::pair<size_t, size_t> &pair)
{
  mPairs.push_back(pair);
}
  



GCPTrack::GCPTrack()
{
}

GCPTrack::~GCPTrack()
{
}

size_t GCPTrack::size() const
{
  return mImageIdPoint.size();
}

tl::Point<double> GCPTrack::point(size_t idx) const
{
  return mImageIdPoint.at(idx);
}

const std::unordered_map<size_t, tl::Point<double>> &GCPTrack::points() const
{
  return mImageIdPoint;
}

void GCPTrack::addPoint(size_t imageId, const tl::Point<double> &point)
{
  mImageIdPoint[imageId] = point ;
}

bool GCPTrack::existPoint(size_t imageId) const
{ 
  auto point = mImageIdPoint.find(imageId);
  return (point != mImageIdPoint.end());
}

void GCPTrack::removePoint(size_t imageId)
{
  auto point = mImageIdPoint.find(imageId);
  if(point != mImageIdPoint.end()) {
    mImageIdPoint.erase(point);
  }
}


}
