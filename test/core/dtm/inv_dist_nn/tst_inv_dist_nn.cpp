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

#define BOOST_TEST_MODULE GRAPHOS invdistnn test

#include <boost/test/unit_test.hpp>
#include "graphos/core/dtm/invdistnn.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(DtmInvDistNNTestSuite)

struct TestDtmInvDistNN
{

    TestDtmInvDistNN()
        : mDtmInvDistNN(new DtmInvDistNNProperties())
    {
    }

    ~TestDtmInvDistNN()
    {
        if (mDtmInvDistNN) {
            delete mDtmInvDistNN;
            mDtmInvDistNN = nullptr;
        }
    }



    DtmInvDistNN *mDtmInvDistNN;

};



BOOST_FIXTURE_TEST_CASE(default_constructor, TestDtmInvDistNN)
{
    BOOST_CHECK_EQUAL(2.0, mDtmInvDistNN->power());
    BOOST_CHECK_EQUAL(0.0, mDtmInvDistNN->smoothing());
    BOOST_CHECK_EQUAL(1.0, mDtmInvDistNN->radius());
    BOOST_CHECK_EQUAL(12, mDtmInvDistNN->maxPoints());
    BOOST_CHECK_EQUAL(0, mDtmInvDistNN->minPoints());
}

BOOST_FIXTURE_TEST_CASE(constructor, TestDtmInvDistNN)
{
    DtmInvDistNNAlgorithm dtmInvDistProperties(2.0, 0.1, 0.2, 100, 1);
    BOOST_CHECK_EQUAL(2.0, dtmInvDistProperties.power());
    BOOST_CHECK_EQUAL(0.1, dtmInvDistProperties.smoothing());
    BOOST_CHECK_EQUAL(0.2, dtmInvDistProperties.radius());
    BOOST_CHECK_EQUAL(100, dtmInvDistProperties.maxPoints());
    BOOST_CHECK_EQUAL(1, dtmInvDistProperties.minPoints());
}

BOOST_FIXTURE_TEST_CASE(reset, TestDtmInvDistNN)
{
    mDtmInvDistNN->reset();

    BOOST_CHECK_EQUAL(2.0, mDtmInvDistNN->power());
    BOOST_CHECK_EQUAL(0.0, mDtmInvDistNN->smoothing());
    BOOST_CHECK_EQUAL(1.0, mDtmInvDistNN->radius());
    BOOST_CHECK_EQUAL(12, mDtmInvDistNN->maxPoints());
    BOOST_CHECK_EQUAL(0, mDtmInvDistNN->minPoints());
}

BOOST_FIXTURE_TEST_CASE(interpolation, TestDtmInvDistNN)
{
    BOOST_CHECK(Dtm::Interpolation::inv_distnn == mDtmInvDistNN->interpolation());
}

BOOST_FIXTURE_TEST_CASE(name, TestDtmInvDistNN)
{
    BOOST_CHECK_EQUAL("INVDISTNN", mDtmInvDistNN->name());
}

BOOST_FIXTURE_TEST_CASE(power, TestDtmInvDistNN)
{
    mDtmInvDistNN->setPower(3.);
    BOOST_CHECK_EQUAL(3., mDtmInvDistNN->power());

    mDtmInvDistNN->setPower(4.);
    BOOST_CHECK_EQUAL(4., mDtmInvDistNN->power());
}

BOOST_FIXTURE_TEST_CASE(smoothing, TestDtmInvDistNN)
{
    mDtmInvDistNN->setSmoothing(0.5);
    BOOST_CHECK_EQUAL(0.5, mDtmInvDistNN->smoothing());

    mDtmInvDistNN->setSmoothing(0.7);
    BOOST_CHECK_EQUAL(0.7, mDtmInvDistNN->smoothing());
}

BOOST_FIXTURE_TEST_CASE(radius, TestDtmInvDistNN)
{
    mDtmInvDistNN->setRadius(1.);
    BOOST_CHECK_EQUAL(1., mDtmInvDistNN->radius());

    mDtmInvDistNN->setRadius(2.);
    BOOST_CHECK_EQUAL(2., mDtmInvDistNN->radius());
}

BOOST_FIXTURE_TEST_CASE(maxpoints, TestDtmInvDistNN)
{
    mDtmInvDistNN->setMaxPoints(5);
    BOOST_CHECK_EQUAL(5, mDtmInvDistNN->maxPoints());

    mDtmInvDistNN->setMaxPoints(10);
    BOOST_CHECK_EQUAL(10, mDtmInvDistNN->maxPoints());
}

BOOST_FIXTURE_TEST_CASE(minpoints, TestDtmInvDistNN)
{
    mDtmInvDistNN->setMinPoints(2);
    BOOST_CHECK_EQUAL(2, mDtmInvDistNN->minPoints());

    mDtmInvDistNN->setMinPoints(3);
    BOOST_CHECK_EQUAL(3, mDtmInvDistNN->minPoints());
}

BOOST_AUTO_TEST_SUITE_END()