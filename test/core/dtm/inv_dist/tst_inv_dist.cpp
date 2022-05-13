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

#define BOOST_TEST_MODULE GRAPHOS invdist test


#include <boost/test/unit_test.hpp>
#include "graphos/core/dtm/invdist.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(DtmInvDistTestSuite)

struct TestDtmInvDist
{

  TestDtmInvDist()
    : mDtmInvDist(new DtmInvDistProperties())
  {
  }

  ~TestDtmInvDist()
  {
    if (mDtmInvDist) {
      delete mDtmInvDist;
      mDtmInvDist = nullptr;
    }
  }

  DtmInvDist *mDtmInvDist;

};


BOOST_FIXTURE_TEST_CASE(default_constructor, TestDtmInvDist)
{
  BOOST_CHECK_EQUAL(2.0, mDtmInvDist->power());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->smoothing());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->radius1());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->radius2());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->angle());
  BOOST_CHECK_EQUAL(0, mDtmInvDist->maxPoints());
  BOOST_CHECK_EQUAL(0, mDtmInvDist->minPoints());
}

BOOST_FIXTURE_TEST_CASE(constructor, TestDtmInvDist)
{
  DtmInvDistAlgorithm dtmInvDistProperties(2.0, 0.1, 0.2, 0.15, 0.5, 100, 1);
  BOOST_CHECK_EQUAL(2.0, dtmInvDistProperties.power());
  BOOST_CHECK_EQUAL(0.1, dtmInvDistProperties.smoothing());
  BOOST_CHECK_EQUAL(0.2, dtmInvDistProperties.radius1());
  BOOST_CHECK_EQUAL(0.15, dtmInvDistProperties.radius2());
  BOOST_CHECK_EQUAL(0.5, dtmInvDistProperties.angle());
  BOOST_CHECK_EQUAL(100, dtmInvDistProperties.maxPoints());
  BOOST_CHECK_EQUAL(1, dtmInvDistProperties.minPoints());
}

BOOST_FIXTURE_TEST_CASE(reset, TestDtmInvDist)
{
  mDtmInvDist->reset();

  BOOST_CHECK_EQUAL(2.0, mDtmInvDist->power());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->smoothing());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->radius1());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->radius2());
  BOOST_CHECK_EQUAL(0.0, mDtmInvDist->angle());
  BOOST_CHECK_EQUAL(0, mDtmInvDist->maxPoints());
  BOOST_CHECK_EQUAL(0, mDtmInvDist->minPoints());
}

BOOST_FIXTURE_TEST_CASE(interpolation, TestDtmInvDist)
{
  BOOST_CHECK(Dtm::Interpolation::inv_dist == mDtmInvDist->interpolation());
}

BOOST_FIXTURE_TEST_CASE(name, TestDtmInvDist)
{
  BOOST_CHECK_EQUAL("INVDIST", mDtmInvDist->name());
}

BOOST_FIXTURE_TEST_CASE(power, TestDtmInvDist)
{
  mDtmInvDist->setPower(3.);
  BOOST_CHECK_EQUAL(3., mDtmInvDist->power());

  mDtmInvDist->setPower(4.);
  BOOST_CHECK_EQUAL(4., mDtmInvDist->power());
}

BOOST_FIXTURE_TEST_CASE(smoothing, TestDtmInvDist)
{
  mDtmInvDist->setSmoothing(1.0);
  BOOST_CHECK_EQUAL(1.0, mDtmInvDist->smoothing());

  mDtmInvDist->setSmoothing(2.0);
  BOOST_CHECK_EQUAL(2.0, mDtmInvDist->smoothing());
}

BOOST_FIXTURE_TEST_CASE(radius1, TestDtmInvDist)
{
  mDtmInvDist->setRadius1(1.0);
  BOOST_CHECK_EQUAL(1.0, mDtmInvDist->radius1());

  mDtmInvDist->setRadius1(2.0);
  BOOST_CHECK_EQUAL(2.0, mDtmInvDist->radius1());
}

BOOST_FIXTURE_TEST_CASE(radius2, TestDtmInvDist)
{
  mDtmInvDist->setRadius2(1.0);
  BOOST_CHECK_EQUAL(1.0, mDtmInvDist->radius2());

  mDtmInvDist->setRadius2(2.0);
  BOOST_CHECK_EQUAL(2.0, mDtmInvDist->radius2());
}

BOOST_FIXTURE_TEST_CASE(angle, TestDtmInvDist)
{
  mDtmInvDist->setAngle(0.1);
  BOOST_CHECK_EQUAL(0.1, mDtmInvDist->angle());

  mDtmInvDist->setAngle(0.2);
  BOOST_CHECK_EQUAL(0.2, mDtmInvDist->angle());
}

BOOST_FIXTURE_TEST_CASE(maxpoints, TestDtmInvDist)
{
  mDtmInvDist->setMaxPoints(5);
  BOOST_CHECK_EQUAL(5, mDtmInvDist->maxPoints());

  mDtmInvDist->setMaxPoints(10);
  BOOST_CHECK_EQUAL(10, mDtmInvDist->maxPoints());
}

BOOST_FIXTURE_TEST_CASE(minPoints, TestDtmInvDist)
{
  mDtmInvDist->setMinPoints(2);
  BOOST_CHECK_EQUAL(2, mDtmInvDist->minPoints());

  mDtmInvDist->setMinPoints(3);
  BOOST_CHECK_EQUAL(3, mDtmInvDist->minPoints());
}

BOOST_AUTO_TEST_SUITE_END()