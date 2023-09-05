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

#define BOOST_TEST_MODULE GRAPHOS CSF test

#include <boost/test/unit_test.hpp>
#include "graphos/core/dtm/Csf.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(CSFTestSuite)

struct TestCSF
{
    TestCSF()
        : mCsf(new Csf())
    {
    }

    ~TestCSF()
    {
        if (mCsf) {
            delete mCsf;
            mCsf = nullptr;
        }
    }

    Csf *mCsf;

};





BOOST_FIXTURE_TEST_CASE(default_constructor, TestCSF)
{
    BOOST_CHECK_EQUAL(false, mCsf->sloopSmooth());
    BOOST_CHECK_EQUAL(0.65, mCsf->timeStep());
    BOOST_CHECK_EQUAL(0.5, mCsf->classThreshold());
    BOOST_CHECK_EQUAL(2, mCsf->clothResolution());
    BOOST_CHECK_EQUAL(2, mCsf->rigidness());
    BOOST_CHECK_EQUAL(500, mCsf->iterations());
}

BOOST_FIXTURE_TEST_CASE(reset, TestCSF)
{
    mCsf->reset();

    BOOST_CHECK_EQUAL(false, mCsf->sloopSmooth());
    BOOST_CHECK_EQUAL(0.65, mCsf->timeStep());
    BOOST_CHECK_EQUAL(0.5, mCsf->classThreshold());
    BOOST_CHECK_EQUAL(2, mCsf->clothResolution());
    BOOST_CHECK_EQUAL(2, mCsf->rigidness());
    BOOST_CHECK_EQUAL(500, mCsf->iterations());
}

BOOST_FIXTURE_TEST_CASE(sloop_smooth, TestCSF)
{
    mCsf->setSloopSmooth(true);
    BOOST_CHECK_EQUAL(true, mCsf->sloopSmooth());

    mCsf->setSloopSmooth(false);
    BOOST_CHECK_EQUAL(false, mCsf->sloopSmooth());
}

BOOST_FIXTURE_TEST_CASE(time_step, TestCSF)
{
    mCsf->setTimeStep(.3);
    BOOST_CHECK_EQUAL(.3, mCsf->timeStep());

    mCsf->setTimeStep(.5);
    BOOST_CHECK_EQUAL(.5, mCsf->timeStep());
}

BOOST_FIXTURE_TEST_CASE(class_threshold, TestCSF)
{
    mCsf->setClassThreshold(1.0);
    BOOST_CHECK_EQUAL(1.0, mCsf->classThreshold());

    mCsf->setClassThreshold(0.5);
    BOOST_CHECK_EQUAL(0.5, mCsf->classThreshold());
}

BOOST_FIXTURE_TEST_CASE(cloth_resolution, TestCSF)
{
    mCsf->setClothResolution(1);
    BOOST_CHECK_EQUAL(1, mCsf->clothResolution());

    mCsf->setClothResolution(2);
    BOOST_CHECK_EQUAL(2, mCsf->clothResolution());
}

BOOST_FIXTURE_TEST_CASE(rigidness, TestCSF)
{
    mCsf->setRigidness(1);
    BOOST_CHECK_EQUAL(1, mCsf->rigidness());

    mCsf->setRigidness(2);
    BOOST_CHECK_EQUAL(2, mCsf->rigidness());
}

BOOST_FIXTURE_TEST_CASE(iterations, TestCSF)
{
    mCsf->setIterations(50);
    BOOST_CHECK_EQUAL(50, mCsf->iterations());

    mCsf->setIterations(100);
    BOOST_CHECK_EQUAL(100, mCsf->iterations());
}

BOOST_AUTO_TEST_SUITE_END()