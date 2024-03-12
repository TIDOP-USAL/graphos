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

#define BOOST_TEST_MODULE GRAPHOS Dense SMVS test

#include <boost/test/unit_test.hpp>
#include "graphos/core/dense/Smvs.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(SmvsTestSuite)

struct TestSmvs
{

    TestSmvs()
      : mSmvs(new Smvs())
    {
    }

    ~TestSmvs()
    {
        if (mSmvs) {
            delete mSmvs;
            mSmvs = nullptr;
        }
    }

    Smvs *mSmvs;

};


BOOST_FIXTURE_TEST_CASE(default_constructor, TestSmvs)
{
    BOOST_CHECK_EQUAL(1, mSmvs->inputImageScale());
    BOOST_CHECK_EQUAL(2, mSmvs->outputDepthScale());
    BOOST_CHECK_EQUAL(false, mSmvs->semiGlobalMatching());
    BOOST_CHECK_EQUAL(1., mSmvs->surfaceSmoothingFactor());
    BOOST_CHECK_EQUAL(false, mSmvs->shadingBasedOptimization());
}

BOOST_FIXTURE_TEST_CASE(constructor, TestSmvs)
{
    Smvs smvs(2, 3, true, true, 0.7);
    BOOST_CHECK_EQUAL(2, smvs.inputImageScale());
    BOOST_CHECK_EQUAL(3, smvs.outputDepthScale());
    BOOST_CHECK_EQUAL(true, smvs.semiGlobalMatching());
    BOOST_CHECK_EQUAL(0.7, smvs.surfaceSmoothingFactor());
    BOOST_CHECK_EQUAL(true, smvs.shadingBasedOptimization());
}

BOOST_FIXTURE_TEST_CASE(reset, TestSmvs)
{
    mSmvs->reset();

    BOOST_CHECK_EQUAL(1, mSmvs->inputImageScale());
    BOOST_CHECK_EQUAL(2, mSmvs->outputDepthScale());
    BOOST_CHECK_EQUAL(false, mSmvs->semiGlobalMatching());
    BOOST_CHECK_EQUAL(1., mSmvs->surfaceSmoothingFactor());
    BOOST_CHECK_EQUAL(false, mSmvs->shadingBasedOptimization());
}

BOOST_FIXTURE_TEST_CASE(type, TestSmvs)
{
    BOOST_CHECK(Densification::Method::smvs == mSmvs->method());
}

BOOST_FIXTURE_TEST_CASE(name, TestSmvs)
{
    BOOST_CHECK_EQUAL("SMVS", mSmvs->name().toStdString());
}

BOOST_FIXTURE_TEST_CASE(inputi_mage_scale, TestSmvs)
{
    mSmvs->setInputImageScale(1);
    BOOST_CHECK_EQUAL(1, mSmvs->inputImageScale());

    mSmvs->setInputImageScale(1);
    BOOST_CHECK_EQUAL(1, mSmvs->inputImageScale());
}

BOOST_FIXTURE_TEST_CASE(output_depth_scale, TestSmvs)
{
    mSmvs->setOutputDepthScale(1);
    BOOST_CHECK_EQUAL(1, mSmvs->outputDepthScale());

    mSmvs->setOutputDepthScale(2);
    BOOST_CHECK_EQUAL(2, mSmvs->outputDepthScale());
}

BOOST_FIXTURE_TEST_CASE(shading_based_optimization, TestSmvs)
{
    mSmvs->setShadingBasedOptimization(true);
    BOOST_CHECK_EQUAL(true, mSmvs->shadingBasedOptimization());

    mSmvs->setShadingBasedOptimization(false);
    BOOST_CHECK_EQUAL(false, mSmvs->shadingBasedOptimization());
}

BOOST_FIXTURE_TEST_CASE(sgb, TestSmvs)
{
    mSmvs->setSemiGlobalMatching(true);
    BOOST_CHECK_EQUAL(true, mSmvs->semiGlobalMatching());

    mSmvs->setSemiGlobalMatching(false);
    BOOST_CHECK_EQUAL(false, mSmvs->semiGlobalMatching());
}

BOOST_FIXTURE_TEST_CASE(surface_smoothing_factor, TestSmvs)
{
    mSmvs->setSurfaceSmoothingFactor(0.5);
    BOOST_CHECK_EQUAL(0.5, mSmvs->surfaceSmoothingFactor());

    mSmvs->setSurfaceSmoothingFactor(1.);
    BOOST_CHECK_EQUAL(1., mSmvs->surfaceSmoothingFactor());
}


BOOST_AUTO_TEST_SUITE_END()