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

#define BOOST_TEST_MODULE GRAPHOS Dense PMVS test

#include <boost/test/unit_test.hpp>
#include "graphos/core/dense/CmvsPmvs.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(PmvsTestSuite)

struct TestPmvs
{

  TestPmvs()
    : mCmvsPmvs(new CmvsPmvsProperties())
  { }

  ~TestPmvs()
  {
    if (mCmvsPmvs) {
      delete mCmvsPmvs;
      mCmvsPmvs = nullptr;
    }
  }

  CmvsPmvs *mCmvsPmvs;
};


BOOST_FIXTURE_TEST_CASE(default_constructor, TestPmvs)
{
  BOOST_CHECK_EQUAL(true, mCmvsPmvs->useVisibilityInformation());
  BOOST_CHECK_EQUAL(100, mCmvsPmvs->imagesPerCluster());
  BOOST_CHECK_EQUAL(1, mCmvsPmvs->level());
  BOOST_CHECK_EQUAL(2, mCmvsPmvs->cellSize());
  BOOST_CHECK_EQUAL(0.7, mCmvsPmvs->threshold());
  BOOST_CHECK_EQUAL(7, mCmvsPmvs->windowSize());
  BOOST_CHECK_EQUAL(3, mCmvsPmvs->minimunImageNumber());
}

BOOST_FIXTURE_TEST_CASE(constructor, TestPmvs)
{
  CmvsPmvsProperties cmvsPmvs(true, 50, 2, 3, 0.5, 10, 2);
  BOOST_CHECK_EQUAL(true, cmvsPmvs.useVisibilityInformation());
  BOOST_CHECK_EQUAL(50, cmvsPmvs.imagesPerCluster());
  BOOST_CHECK_EQUAL(2, cmvsPmvs.level());
  BOOST_CHECK_EQUAL(3, cmvsPmvs.cellSize());
  BOOST_CHECK_EQUAL(0.5, cmvsPmvs.threshold());
  BOOST_CHECK_EQUAL(10, cmvsPmvs.windowSize());
  BOOST_CHECK_EQUAL(2, cmvsPmvs.minimunImageNumber());
}

BOOST_FIXTURE_TEST_CASE(reset, TestPmvs)
{
  mCmvsPmvs->reset();

  BOOST_CHECK_EQUAL(true, mCmvsPmvs->useVisibilityInformation());
  BOOST_CHECK_EQUAL(100, mCmvsPmvs->imagesPerCluster());
  BOOST_CHECK_EQUAL(1, mCmvsPmvs->level());
  BOOST_CHECK_EQUAL(2, mCmvsPmvs->cellSize());
  BOOST_CHECK_EQUAL(0.7, mCmvsPmvs->threshold());
  BOOST_CHECK_EQUAL(7, mCmvsPmvs->windowSize());
  BOOST_CHECK_EQUAL(3, mCmvsPmvs->minimunImageNumber());
}


BOOST_FIXTURE_TEST_CASE(type, TestPmvs)
{
  BOOST_CHECK(Densification::Method::cmvs_pmvs == mCmvsPmvs->method());
}

BOOST_FIXTURE_TEST_CASE(name, TestPmvs)
{
  BOOST_CHECK_EQUAL("CMVS/PMVS", mCmvsPmvs->name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(use_visibility_information, TestPmvs)
{
  mCmvsPmvs->setUseVisibilityInformation(true);
  BOOST_CHECK_EQUAL(true, mCmvsPmvs->useVisibilityInformation());

  mCmvsPmvs->setUseVisibilityInformation(false);
  BOOST_CHECK_EQUAL(false, mCmvsPmvs->useVisibilityInformation());
}

BOOST_FIXTURE_TEST_CASE(images_per_cluster, TestPmvs)
{
  mCmvsPmvs->setImagesPerCluster(100);
  BOOST_CHECK_EQUAL(100, mCmvsPmvs->imagesPerCluster());

  mCmvsPmvs->setImagesPerCluster(50);
  BOOST_CHECK_EQUAL(50, mCmvsPmvs->imagesPerCluster());
}

BOOST_FIXTURE_TEST_CASE(level, TestPmvs)
{
  mCmvsPmvs->setLevel(1);
  BOOST_CHECK_EQUAL(1, mCmvsPmvs->level());

  mCmvsPmvs->setLevel(2);
  BOOST_CHECK_EQUAL(2, mCmvsPmvs->level());
}

BOOST_FIXTURE_TEST_CASE(cell_size, TestPmvs)
{
  mCmvsPmvs->setCellSize(3);
  BOOST_CHECK_EQUAL(3, mCmvsPmvs->cellSize());

  mCmvsPmvs->setCellSize(7);
  BOOST_CHECK_EQUAL(7, mCmvsPmvs->cellSize());
}

BOOST_FIXTURE_TEST_CASE(threshold, TestPmvs)
{
  mCmvsPmvs->setThreshold(0.6);
  BOOST_CHECK_EQUAL(0.6, mCmvsPmvs->threshold());

  mCmvsPmvs->setThreshold(1.);
  BOOST_CHECK_EQUAL(1., mCmvsPmvs->threshold());
}

BOOST_FIXTURE_TEST_CASE(window_size, TestPmvs)
{
  mCmvsPmvs->setWindowSize(5);
  BOOST_CHECK_EQUAL(5, mCmvsPmvs->windowSize());

  mCmvsPmvs->setWindowSize(10);
  BOOST_CHECK_EQUAL(10, mCmvsPmvs->windowSize());
}

BOOST_FIXTURE_TEST_CASE(minimun_image_number, TestPmvs)
{
  mCmvsPmvs->setMinimunImageNumber(2);
  BOOST_CHECK_EQUAL(2, mCmvsPmvs->minimunImageNumber());

  mCmvsPmvs->setMinimunImageNumber(3);
  BOOST_CHECK_EQUAL(3, mCmvsPmvs->minimunImageNumber());
}

BOOST_AUTO_TEST_SUITE_END()