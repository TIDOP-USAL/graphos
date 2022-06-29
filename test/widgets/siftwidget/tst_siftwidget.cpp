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

#define BOOST_TEST_MODULE GRAPHOS SiftWidget test

#include <boost/test/unit_test.hpp>
#include "graphos/widgets/SiftWidget.h"

#include <QApplication>

using namespace graphos;

BOOST_AUTO_TEST_SUITE(SiftWidgetTestSuite)

struct SiftWidgetTest
{

  SiftWidgetTest()
  {
    int argc = 0;
    static QApplication a(argc, nullptr);
  }

  ~SiftWidgetTest()
  {
    if (mSiftWidget) {
      delete mSiftWidget;
      mSiftWidget = nullptr;
    }
  }

  virtual void setup()
  {

    mSiftWidget = new SiftWidgetImp();
  }

  virtual void teardown()
  {

  }

  SiftWidget *mSiftWidget;
};


BOOST_FIXTURE_TEST_CASE(ConstructorSiftWidget, SiftWidgetTest)
{
  BOOST_CHECK_EQUAL(5000, mSiftWidget->featuresNumber());
  BOOST_CHECK_EQUAL(3, mSiftWidget->octaveLayers());
  BOOST_CHECK_EQUAL(true, mSiftWidget->constrastThresholdAuto());
  BOOST_CHECK_CLOSE(0.0067, mSiftWidget->contrastThreshold(), 0.1);
  BOOST_CHECK_EQUAL(10., mSiftWidget->edgeThreshold());
}

BOOST_FIXTURE_TEST_CASE(windowTitle, SiftWidgetTest)
{
  BOOST_CHECK("SIFT" == mSiftWidget->windowTitle());
}

BOOST_FIXTURE_TEST_CASE(features_number, SiftWidgetTest)
{
  mSiftWidget->setFeaturesNumber(500);
  BOOST_CHECK_EQUAL(500, mSiftWidget->featuresNumber());
  mSiftWidget->setFeaturesNumber(10000);
  BOOST_CHECK_EQUAL(10000, mSiftWidget->featuresNumber());
}

BOOST_FIXTURE_TEST_CASE(octave_layers, SiftWidgetTest)
{
  mSiftWidget->setOctaveLayers(0);
  BOOST_CHECK_EQUAL(0, mSiftWidget->octaveLayers());
  mSiftWidget->setOctaveLayers(1);
  BOOST_CHECK_EQUAL(1, mSiftWidget->octaveLayers());
  mSiftWidget->setOctaveLayers(2);
  BOOST_CHECK_EQUAL(2, mSiftWidget->octaveLayers());
  mSiftWidget->setOctaveLayers(4);
  BOOST_CHECK_EQUAL(4, mSiftWidget->octaveLayers());
  mSiftWidget->setOctaveLayers(7);
  BOOST_CHECK_EQUAL(7, mSiftWidget->octaveLayers());
}

BOOST_FIXTURE_TEST_CASE(contrast_threshold_auto, SiftWidgetTest)
{
  mSiftWidget->setContrastThresholdAuto(false);
  BOOST_CHECK_EQUAL(false, mSiftWidget->constrastThresholdAuto());

  mSiftWidget->setContrastThresholdAuto(true);
  BOOST_CHECK_EQUAL(true, mSiftWidget->constrastThresholdAuto());
}

BOOST_FIXTURE_TEST_CASE(contrast_threshold, SiftWidgetTest)
{
  mSiftWidget->setContrastThresholdAuto(false);

  mSiftWidget->setContrastThreshold(0.04);
  BOOST_CHECK_EQUAL(0.04, mSiftWidget->contrastThreshold());

  mSiftWidget->setContrastThreshold(0.2);
  BOOST_CHECK_EQUAL(0.2, mSiftWidget->contrastThreshold());
}

BOOST_FIXTURE_TEST_CASE(edge_threshold, SiftWidgetTest)
{
  mSiftWidget->setEdgeThreshold(10.);
  BOOST_CHECK_EQUAL(10., mSiftWidget->edgeThreshold());

  mSiftWidget->setEdgeThreshold(15.);
  BOOST_CHECK_EQUAL(15., mSiftWidget->edgeThreshold());
}


BOOST_FIXTURE_TEST_CASE(clear, SiftWidgetTest)
{
  mSiftWidget->setFeaturesNumber(500);
  mSiftWidget->setOctaveLayers(4);
  mSiftWidget->setContrastThreshold(0.5);
  mSiftWidget->setEdgeThreshold(20.);

  mSiftWidget->clear();

  BOOST_CHECK_EQUAL(5000, mSiftWidget->featuresNumber());
  BOOST_CHECK_EQUAL(3, mSiftWidget->octaveLayers());
  BOOST_CHECK_EQUAL(0.0067, mSiftWidget->contrastThreshold());
  BOOST_CHECK_EQUAL(10., mSiftWidget->edgeThreshold());
}

BOOST_AUTO_TEST_SUITE_END()