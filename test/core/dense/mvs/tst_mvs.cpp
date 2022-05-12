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

#define BOOST_TEST_MODULE GRAPHOS Dense MVS test

#include <boost/test/unit_test.hpp>
#include "graphos/core/dense/mvs.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(MvsTestSuite)

struct TestMvs
{

  TestMvs()
    : mMVS(new MvsProperties())
  {
  }

  ~TestMvs()
  {
    if (mMVS) {
      delete mMVS;
      mMVS = nullptr;
    }
  }

  Mvs *mMVS;

};


BOOST_FIXTURE_TEST_CASE(default_constructor, TestMvs)
{
  BOOST_CHECK_EQUAL(1, mMVS->resolutionLevel());
  BOOST_CHECK_EQUAL(640, mMVS->minResolution());
  BOOST_CHECK_EQUAL(3200, mMVS->maxResolution());
  BOOST_CHECK_EQUAL(5, mMVS->numberViews());
  BOOST_CHECK_EQUAL(3, mMVS->numberViewsFuse());
}

BOOST_FIXTURE_TEST_CASE(constructor, TestMvs)
{
  MvsDensifier densifierMVS(2, 500, 3000, 6, 4);
  BOOST_CHECK_EQUAL(2, densifierMVS.resolutionLevel());
  BOOST_CHECK_EQUAL(500, densifierMVS.minResolution());
  BOOST_CHECK_EQUAL(3000, densifierMVS.maxResolution());
  BOOST_CHECK_EQUAL(6, densifierMVS.numberViews());
  BOOST_CHECK_EQUAL(4, densifierMVS.numberViewsFuse());
}

BOOST_FIXTURE_TEST_CASE(reset, TestMvs)
{
  mMVS->reset();

  BOOST_CHECK_EQUAL(1, mMVS->resolutionLevel());
  BOOST_CHECK_EQUAL(640, mMVS->minResolution());
  BOOST_CHECK_EQUAL(3200, mMVS->maxResolution());
  BOOST_CHECK_EQUAL(5, mMVS->numberViews());
  BOOST_CHECK_EQUAL(3, mMVS->numberViewsFuse());
}

BOOST_FIXTURE_TEST_CASE(type, TestMvs)
{
  BOOST_CHECK(Densification::Method::mvs == mMVS->method());
}

BOOST_FIXTURE_TEST_CASE(name, TestMvs)
{
  BOOST_CHECK_EQUAL("MVS", mMVS->name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(resolution_level, TestMvs)
{
  mMVS->setResolutionLevel(2);
  BOOST_CHECK_EQUAL(2, mMVS->resolutionLevel());

  mMVS->setResolutionLevel(3);
  BOOST_CHECK_EQUAL(3, mMVS->resolutionLevel());
}

BOOST_FIXTURE_TEST_CASE(min_resolution, TestMvs)
{
  mMVS->setMinResolution(650);
  BOOST_CHECK_EQUAL(650, mMVS->minResolution());

  mMVS->setMinResolution(750);
  BOOST_CHECK_EQUAL(750, mMVS->minResolution());
}

BOOST_FIXTURE_TEST_CASE(max_resolution, TestMvs)
{
  mMVS->setMaxResolution(2500);
  BOOST_CHECK_EQUAL(2500, mMVS->maxResolution());

  mMVS->setMaxResolution(3000);
  BOOST_CHECK_EQUAL(3000, mMVS->maxResolution());
}

BOOST_FIXTURE_TEST_CASE(number_views, TestMvs)
{
  mMVS->setNumberViews(4);
  BOOST_CHECK_EQUAL(4, mMVS->numberViews());

  mMVS->setNumberViews(6);
  BOOST_CHECK_EQUAL(6, mMVS->numberViews());
}

BOOST_FIXTURE_TEST_CASE(number_views_fuse, TestMvs)
{
  mMVS->setNumberViewsFuse(2);
  BOOST_CHECK_EQUAL(2, mMVS->numberViewsFuse());

  mMVS->setNumberViewsFuse(3);
  BOOST_CHECK_EQUAL(3, mMVS->numberViewsFuse());
}

BOOST_AUTO_TEST_SUITE_END()