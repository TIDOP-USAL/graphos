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

#define BOOST_TEST_MODULE GRAPHOS feature extractor view test

#include <boost/test/unit_test.hpp>
#include <QtTest>
#include <QCoreApplication>

#include "graphos/components/featextract/impl/FeatureExtractorView.h"
#include "graphos/widgets/SiftWidget.h"


using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestFeatureExtractorViewSuite)

QApplication app(boost::unit_test::framework::master_test_suite().argc,
                 boost::unit_test::framework::master_test_suite().argv);

class TestFeatureExtractorView
  : public FeatureExtractorViewImp
{

public:

  TestFeatureExtractorView()
    : FeatureExtractorViewImp(),
      mSift(new SiftWidget){}
  ~TestFeatureExtractorView() 
  {
    if (mSift) {
      delete mSift;
      mSift = nullptr;
    }
  }

  void setup() {}
  void teardown() {}

  SiftWidget *mSift;
};

BOOST_FIXTURE_TEST_CASE(default_constructor, TestFeatureExtractorView)
{
  BOOST_CHECK_EQUAL("", currentDetectorDescriptor().toStdString());
  BOOST_CHECK_EQUAL(3000, maxImageSize());
  BOOST_CHECK_EQUAL(false, fullImageSize());
}

BOOST_FIXTURE_TEST_CASE(_clear, TestFeatureExtractorView)
{
  clear();

  BOOST_CHECK_EQUAL(3000, maxImageSize());
  BOOST_CHECK_EQUAL(false, fullImageSize());
}

BOOST_FIXTURE_TEST_CASE(window_title, TestFeatureExtractorView)
{
  BOOST_CHECK_EQUAL("Feature Detector/Descriptor", this->windowTitle().toStdString());
}

BOOST_FIXTURE_TEST_CASE(current_detector_descriptor, TestFeatureExtractorView)
{
  addDetectorDescriptor(mSift);
  setCurrentDetectorDescriptor(mSift->windowTitle());
  BOOST_CHECK_EQUAL("SIFT", this->currentDetectorDescriptor().toStdString());
}

BOOST_FIXTURE_TEST_CASE(max_image_size, TestFeatureExtractorView)
{
  setMaxImageSize(2500);
  BOOST_CHECK_EQUAL(2500, this->maxImageSize());

  setMaxImageSize(3500);
  BOOST_CHECK_EQUAL(3500, this->maxImageSize());
}

BOOST_FIXTURE_TEST_CASE(full_image_size, TestFeatureExtractorView)
{
  this->setFullImageSize(true);
  BOOST_CHECK_EQUAL(true, this->fullImageSize());

  this->setFullImageSize(false);
  BOOST_CHECK_EQUAL(false, this->fullImageSize());
}

BOOST_AUTO_TEST_SUITE_END()