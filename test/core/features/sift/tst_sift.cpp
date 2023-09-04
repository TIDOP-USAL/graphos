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

#define BOOST_TEST_MODULE GRAPHOS Sift test

#include <boost/test/unit_test.hpp>
#include "graphos/core/features/sift.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(SiftTestSuite)

struct SiftTest
{

    SiftTest()
        : mSift(new SiftProperties()),
        mSiftCPUDetectorDescriptor(new SiftCPUDetectorDescriptor(6000, 4, 9.)),
        mSiftCudaDetectorDescriptor(new SiftCudaDetectorDescriptor(10000, 8, 11., 0.8))
    {

    }

    ~SiftTest()
    {
        if (mSift) {
            delete mSift;
            mSift = nullptr;
        }

        if (mSiftCPUDetectorDescriptor) {
            delete mSiftCPUDetectorDescriptor;
            mSiftCPUDetectorDescriptor = nullptr;
        }

        if (mSiftCudaDetectorDescriptor) {
            delete mSiftCudaDetectorDescriptor;
            mSiftCudaDetectorDescriptor = nullptr;
        }
    }

    virtual void setup()
    {
    }

    virtual void teardown()
    {

    }

    Sift *mSift;
    SiftCPUDetectorDescriptor *mSiftCPUDetectorDescriptor;
    SiftCudaDetectorDescriptor *mSiftCudaDetectorDescriptor;
};


BOOST_FIXTURE_TEST_CASE(ConstructorSiftProperties, SiftTest)
{
    BOOST_CHECK_EQUAL(5000, mSift->featuresNumber());
    BOOST_CHECK_EQUAL(3, mSift->octaveLayers());
    BOOST_CHECK_EQUAL(true, mSift->constrastThresholdAuto());
    BOOST_CHECK_EQUAL(0.02 / mSift->octaveLayers(), mSift->contrastThreshold());
    BOOST_CHECK_EQUAL(10., mSift->edgeThreshold());
}

BOOST_FIXTURE_TEST_CASE(ConstructorSiftCPU, SiftTest)
{
    BOOST_CHECK_EQUAL(6000, mSiftCPUDetectorDescriptor->featuresNumber());
    BOOST_CHECK_EQUAL(4, mSiftCPUDetectorDescriptor->octaveLayers());
    BOOST_CHECK_EQUAL(true, mSiftCPUDetectorDescriptor->constrastThresholdAuto());
    BOOST_CHECK_EQUAL(0.02 / mSiftCPUDetectorDescriptor->octaveLayers(), mSiftCPUDetectorDescriptor->contrastThreshold());
    BOOST_CHECK_EQUAL(9., mSiftCPUDetectorDescriptor->edgeThreshold());
}

BOOST_FIXTURE_TEST_CASE(ConstructorSiftCuda, SiftTest)
{
    BOOST_CHECK_EQUAL(10000, mSiftCudaDetectorDescriptor->featuresNumber());
    BOOST_CHECK_EQUAL(8, mSiftCudaDetectorDescriptor->octaveLayers());
    BOOST_CHECK_EQUAL(false, mSiftCudaDetectorDescriptor->constrastThresholdAuto());
    BOOST_CHECK_EQUAL(0.8, mSiftCudaDetectorDescriptor->contrastThreshold());
    BOOST_CHECK_EQUAL(11., mSiftCudaDetectorDescriptor->edgeThreshold());
}

BOOST_FIXTURE_TEST_CASE(test_copy_constructor, SiftTest)
{
    SiftCudaDetectorDescriptor c(*mSiftCudaDetectorDescriptor);
    BOOST_CHECK_EQUAL(10000, c.featuresNumber());
    BOOST_CHECK_EQUAL(8, c.octaveLayers());
    BOOST_CHECK_EQUAL(false, c.constrastThresholdAuto());
    BOOST_CHECK_EQUAL(0.8, c.contrastThreshold());
    BOOST_CHECK_EQUAL(11., c.edgeThreshold());
}

BOOST_FIXTURE_TEST_CASE(reset, SiftTest)
{
    mSift->setFeaturesNumber(500);
    mSift->setOctaveLayers(4);
    mSift->setContrastThreshold(0.5);
    mSift->setEdgeThreshold(20.);

    mSift->reset();

    BOOST_CHECK_EQUAL(5000, mSift->featuresNumber());
    BOOST_CHECK_EQUAL(3, mSift->octaveLayers());
    BOOST_CHECK_EQUAL(true, mSift->constrastThresholdAuto());
    BOOST_CHECK_EQUAL(0.02 / mSift->octaveLayers(), mSift->contrastThreshold());
    BOOST_CHECK_EQUAL(10., mSift->edgeThreshold());
}


BOOST_FIXTURE_TEST_CASE(type, SiftTest)
{
    BOOST_CHECK(Sift::Type::sift == mSift->type());
}

BOOST_FIXTURE_TEST_CASE(name, SiftTest)
{
    BOOST_CHECK_EQUAL("SIFT", mSift->name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(features_number, SiftTest)
{
    mSift->setFeaturesNumber(500);
    BOOST_CHECK_EQUAL(500, mSift->featuresNumber());
    mSift->setFeaturesNumber(10000);
    BOOST_CHECK_EQUAL(10000, mSift->featuresNumber());
}

BOOST_FIXTURE_TEST_CASE(octave_layers, SiftTest)
{
    mSift->setOctaveLayers(0);
    BOOST_CHECK_EQUAL(0, mSift->octaveLayers());
    mSift->setOctaveLayers(1);
    BOOST_CHECK_EQUAL(1, mSift->octaveLayers());
    mSift->setOctaveLayers(2);
    BOOST_CHECK_EQUAL(2, mSift->octaveLayers());
    mSift->setOctaveLayers(4);
    BOOST_CHECK_EQUAL(4, mSift->octaveLayers());
    mSift->setOctaveLayers(7);
    BOOST_CHECK_EQUAL(7, mSift->octaveLayers());
}

BOOST_FIXTURE_TEST_CASE(contrast_threshold_auto, SiftTest)
{
    mSift->setContrastThresholdAuto(false);
    BOOST_CHECK_EQUAL(false, mSift->constrastThresholdAuto());

    mSift->setContrastThresholdAuto(true);
    BOOST_CHECK_EQUAL(true, mSift->constrastThresholdAuto());
}

BOOST_FIXTURE_TEST_CASE(contrast_threshold, SiftTest)
{
    mSift->setContrastThresholdAuto(false);

    mSift->setContrastThreshold(0.04);
    BOOST_CHECK_EQUAL(0.04, mSift->contrastThreshold());

    mSift->setContrastThreshold(0.2);
    BOOST_CHECK_EQUAL(0.2, mSift->contrastThreshold());
}

BOOST_FIXTURE_TEST_CASE(edge_threshold, SiftTest)
{
    mSift->setEdgeThreshold(10.);
    BOOST_CHECK_EQUAL(10., mSift->edgeThreshold());

    mSift->setEdgeThreshold(15.);
    BOOST_CHECK_EQUAL(15., mSift->edgeThreshold());
}


BOOST_AUTO_TEST_SUITE_END()