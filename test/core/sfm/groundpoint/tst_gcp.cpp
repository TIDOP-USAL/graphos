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

#define BOOST_TEST_MODULE GRAPHOS Ground Control Point test

#include <boost/test/unit_test.hpp>
#include "graphos/core/sfm/groundpoint.h"

using namespace graphos;
using namespace tl;
using namespace math;

BOOST_AUTO_TEST_SUITE(TestGroundControlPointSuite)

struct TestGroundControlPoint
{

  TestGroundControlPoint()
  { }

  ~TestGroundControlPoint()
  {
  }

  void setup()
  {
    tl::Path gcp_path(GRAPHOS_SOURCE_PATH);
    gcp_path.append("test\\core\\sfm\\gcp\\georef.xml");
    gcps = groundControlPointsRead(gcp_path);
  }

  void teardown()
  {

  }

  GroundControlPoint mGCP;
  std::vector<GroundControlPoint> gcps;
};


BOOST_FIXTURE_TEST_CASE(DefaultConstructor, TestGroundControlPoint)
{
  BOOST_CHECK_EQUAL("", mGCP.name());
  BOOST_CHECK(tl::Point3D() == mGCP);
  BOOST_CHECK_EQUAL(0., mGCP.x);
  BOOST_CHECK_EQUAL(0., mGCP.y);
  BOOST_CHECK_EQUAL(0., mGCP.z);

  GCPTrack track = mGCP.track();

  BOOST_CHECK_EQUAL(false, track.existPoint(0));
  BOOST_CHECK_EQUAL(0, track.size());

}

BOOST_FIXTURE_TEST_CASE(name, TestGroundControlPoint)
{
  mGCP.setName("P1");
  BOOST_CHECK_EQUAL("P1", mGCP.name());

  mGCP.setName("P2");
  BOOST_CHECK_EQUAL("P2", mGCP.name());

  BOOST_CHECK_EQUAL("1", gcps.at(0).name());
  BOOST_CHECK_EQUAL("2", gcps.at(1).name());
  BOOST_CHECK_EQUAL("3", gcps.at(2).name());
  BOOST_CHECK_EQUAL("4", gcps.at(3).name());
  BOOST_CHECK_EQUAL("5", gcps.at(4).name());
  BOOST_CHECK_EQUAL("6", gcps.at(5).name());
}

BOOST_FIXTURE_TEST_CASE(point, TestGroundControlPoint)
{
  tl::Point3D point(357037.834, 4500179.609, 1068.39);
  mGCP.setPoint(point);
  BOOST_CHECK(point == mGCP);
  BOOST_CHECK_EQUAL(point.x, mGCP.x);
  BOOST_CHECK_EQUAL(point.y, mGCP.y);
  BOOST_CHECK_EQUAL(point.z, mGCP.z);

  point = gcps.at(0);
  BOOST_CHECK_EQUAL(357037.834, point.x);
  BOOST_CHECK_EQUAL(4500179.609, point.y);
  BOOST_CHECK_EQUAL(1068.39, point.z);

  point = gcps.at(1);
  BOOST_CHECK_EQUAL(356966.833, point.x);
  BOOST_CHECK_EQUAL(4500170.587, point.y);
  BOOST_CHECK_EQUAL(1068.24, point.z);
}

BOOST_FIXTURE_TEST_CASE(add_point_to_track, TestGroundControlPoint)
{
  tl::Point<double> point(2631.1, 1364.07);
  mGCP.addPointToTrack(1, point);
  tl::Point<double> point_2 = mGCP.track().point(1);
  BOOST_CHECK_EQUAL(point.x, point_2.x);
  BOOST_CHECK_EQUAL(point.y, point_2.y);
}

BOOST_FIXTURE_TEST_CASE(exist_track_point, TestGroundControlPoint)
{
  mGCP.addPointToTrack(1, tl::Point<double>(2631.1, 1364.07));
  BOOST_CHECK_EQUAL(false, mGCP.track().existPoint(0));
  BOOST_CHECK_EQUAL(true, mGCP.track().existPoint(1));
}

BOOST_FIXTURE_TEST_CASE(remove_track_point, TestGroundControlPoint)
{
  mGCP.addPointToTrack(1, tl::Point<double>(2631.1, 1364.07));
  BOOST_CHECK_EQUAL(true, mGCP.track().existPoint(1));
  mGCP.removeTrackPoint(1);
  BOOST_CHECK_EQUAL(false, mGCP.track().existPoint(1));
}

BOOST_FIXTURE_TEST_CASE(image_points, TestGroundControlPoint)
{
  const auto points = mGCP.track();
  BOOST_CHECK_EQUAL(0, points.size());
}

BOOST_AUTO_TEST_SUITE_END()