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

#define BOOST_TEST_MODULE GRAPHOS feature matching test

#include <boost/test/unit_test.hpp>
#include "graphos/core/features/matching.h"

using namespace graphos;


BOOST_AUTO_TEST_SUITE(FeatureMatchingTestSuite)


struct TestFeatureMatchingProperties
{

    TestFeatureMatchingProperties()
        : mFeatureMatching(new FeatureMatchingProperties())
    {

    }

    ~TestFeatureMatchingProperties()
    {
        if (mFeatureMatching) {
            delete mFeatureMatching;
            mFeatureMatching = nullptr;
        }
    }

    FeatureMatching *mFeatureMatching;

};



BOOST_FIXTURE_TEST_CASE(default_constructor, TestFeatureMatchingProperties)
{
    BOOST_CHECK_EQUAL(true, mFeatureMatching->crossCheck());
    BOOST_CHECK_EQUAL(0.8, mFeatureMatching->ratio());
    BOOST_CHECK_EQUAL(0.7, mFeatureMatching->distance());
    BOOST_CHECK_EQUAL(4.0, mFeatureMatching->maxError());
    BOOST_CHECK_EQUAL(0.999, mFeatureMatching->confidence());
}

BOOST_FIXTURE_TEST_CASE(crossCheck, TestFeatureMatchingProperties)
{
    mFeatureMatching->enableCrossCheck(false);
    BOOST_CHECK_EQUAL(false, mFeatureMatching->crossCheck());
    mFeatureMatching->enableCrossCheck(true);
    BOOST_CHECK_EQUAL(true, mFeatureMatching->crossCheck());
}

BOOST_FIXTURE_TEST_CASE(ratio, TestFeatureMatchingProperties)
{
    mFeatureMatching->setRatio(0.7);
    BOOST_CHECK_EQUAL(0.7, mFeatureMatching->ratio());

    mFeatureMatching->setRatio(0.8);
    BOOST_CHECK_EQUAL(0.8, mFeatureMatching->ratio());
}

BOOST_FIXTURE_TEST_CASE(distance, TestFeatureMatchingProperties)
{
    mFeatureMatching->setDistance(0.9);
    BOOST_CHECK_EQUAL(0.9, mFeatureMatching->distance());

    mFeatureMatching->setDistance(1.5);
    BOOST_CHECK_EQUAL(1.5, mFeatureMatching->distance());
}

BOOST_FIXTURE_TEST_CASE(max_error, TestFeatureMatchingProperties)
{
    mFeatureMatching->setMaxError(2.0);
    BOOST_CHECK_EQUAL(2.0, mFeatureMatching->maxError());

    mFeatureMatching->setMaxError(3.0);
    BOOST_CHECK_EQUAL(3.0, mFeatureMatching->maxError());
}

BOOST_FIXTURE_TEST_CASE(confidence, TestFeatureMatchingProperties)
{
    mFeatureMatching->setConfidence(0.9);
    BOOST_CHECK_EQUAL(0.9, mFeatureMatching->confidence());

    mFeatureMatching->setConfidence(0.995);
    BOOST_CHECK_EQUAL(0.995, mFeatureMatching->confidence());
}


BOOST_AUTO_TEST_SUITE_END()