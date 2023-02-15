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

#define BOOST_TEST_MODULE GRAPHOS feature natching view test

#include <boost/test/unit_test.hpp>
#include <QtTest>
#include <QCoreApplication>

#include "graphos/components/featmatch/impl/FeatureMatchingView.h"
#include "graphos/widgets/FeatureMatchingWidget.h"


using namespace graphos;


BOOST_AUTO_TEST_SUITE(TestFeatureMatchingViewSuite)

QApplication app(boost::unit_test::framework::master_test_suite().argc,
                 boost::unit_test::framework::master_test_suite().argv);


class TestFeatureMatchingView
  : public FeatureMatchingViewImp
{

public:

  TestFeatureMatchingView()
    : FeatureMatchingViewImp(),
    mFeatureMatchingWidget(new FeatureMatchingWidgetImp)
  {
  }
  ~TestFeatureMatchingView()
  {
    if (mFeatureMatchingWidget) {
      delete mFeatureMatchingWidget;
      mFeatureMatchingWidget = nullptr;
    }
  }

  void setup() {}
  void teardown() {}

  FeatureMatchingWidget *mFeatureMatchingWidget;
};


BOOST_FIXTURE_TEST_CASE(default_constructor, TestFeatureMatchingView)
{
  BOOST_CHECK_EQUAL("", currentMatchMethod().toStdString());

  //addMatchMethod(mFeatureMatchingWidget);
  //setCurrentMatchMethod(mFeatureMatchingWidget->windowTitle());
}

BOOST_FIXTURE_TEST_CASE(_clear, TestFeatureMatchingView)
{
  clear();


}

BOOST_FIXTURE_TEST_CASE(window_title, TestFeatureMatchingView)
{
  BOOST_CHECK_EQUAL("Feature Matching", windowTitle().toStdString());
}

BOOST_FIXTURE_TEST_CASE(current_match_method, TestFeatureMatchingView)
{
  BOOST_CHECK_EQUAL("Feature Matching Colmap", this->currentMatchMethod().toStdString());
}

 BOOST_AUTO_TEST_SUITE_END()
