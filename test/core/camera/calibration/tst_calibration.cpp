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

#define BOOST_TEST_MODULE GRAPHOS Calibration test

#include <boost/test/unit_test.hpp>
#include "graphos/core/camera/Camera.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(CalibrationTestSuite)

struct CalibrationTest
{
    CalibrationTest()
    {
    }

    ~CalibrationTest()
    {
    }


    virtual void setup()
    {
        mCalibrationRadial1 = CalibrationFactory::create(Calibration::CameraModel::radial1);
        mCalibrationRadial1->setParameter(Calibration::Parameters::focal, 4000);
        mCalibrationRadial1->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationRadial1->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationRadial1->setParameter(Calibration::Parameters::k1, 0.005);

        mCalibrationRadial2 = CalibrationFactory::create(Calibration::CameraModel::radial2);
        mCalibrationRadial2->setParameter(Calibration::Parameters::focal, 4000);
        mCalibrationRadial2->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationRadial2->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationRadial2->setParameter(Calibration::Parameters::k1, 0.005);
        mCalibrationRadial2->setParameter(Calibration::Parameters::k2, 0.001);

        mCalibrationRadial3 = CalibrationFactory::create(Calibration::CameraModel::radial3);
        mCalibrationRadial3->setParameter(Calibration::Parameters::focal, 4000);
        mCalibrationRadial3->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationRadial3->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationRadial3->setParameter(Calibration::Parameters::k1, 0.005);
        mCalibrationRadial3->setParameter(Calibration::Parameters::k2, 0.001);
        mCalibrationRadial3->setParameter(Calibration::Parameters::k3, 0.0001);
        mCalibrationRadial3->setParameter(Calibration::Parameters::p1, 0.01);
        mCalibrationRadial3->setParameter(Calibration::Parameters::p2, 0.001);

        mCalibrationSimpleRadialFisheye = CalibrationFactory::create(Calibration::CameraModel::simple_radial_fisheye);
        mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::focal, 4000);
        mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::k1, 0.005);

        mCalibrationRadialFisheye = CalibrationFactory::create(Calibration::CameraModel::radial_fisheye);
        mCalibrationRadialFisheye->setParameter(Calibration::Parameters::focal, 4000);
        mCalibrationRadialFisheye->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationRadialFisheye->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationRadialFisheye->setParameter(Calibration::Parameters::k1, 0.005);
        mCalibrationRadialFisheye->setParameter(Calibration::Parameters::k2, 0.001);

        mCalibrationOpenCV = CalibrationFactory::create(Calibration::CameraModel::opencv);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::focalx, 4000);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::focaly, 4000);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::k1, 0.005);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::k2, 0.001);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::p1, 0.01);
        mCalibrationOpenCV->setParameter(Calibration::Parameters::p2, 0.001);

        mCalibrationOpenCVFisheye = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::focalx, 4000);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::focaly, 4000);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k1, 0.005);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k2, 0.001);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k3, 0.0005);
        mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k4, 0.0001);

        mCalibrationOpenCVFull = CalibrationFactory::create(Calibration::CameraModel::opencv_full);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::focalx, 4000);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::focaly, 4000);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::cy, 0.5);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k1, 0.005);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k2, 0.001);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k3, 0.0005);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k4, 0.0001);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k5, 0.00005);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k6, 0.00001);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::p1, 0.01);
        mCalibrationOpenCVFull->setParameter(Calibration::Parameters::p2, 0.001);

        mCalibrationSimplePinhole = CalibrationFactory::create(Calibration::CameraModel::simple_pinhole);
        mCalibrationSimplePinhole->setParameter(Calibration::Parameters::focal, 4000);
        mCalibrationSimplePinhole->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationSimplePinhole->setParameter(Calibration::Parameters::cy, 0.5);

        mCalibrationPinhole = CalibrationFactory::create(Calibration::CameraModel::pinhole);
        mCalibrationPinhole->setParameter(Calibration::Parameters::focalx, 4000);
        mCalibrationPinhole->setParameter(Calibration::Parameters::focaly, 4000);
        mCalibrationPinhole->setParameter(Calibration::Parameters::cx, 0.6);
        mCalibrationPinhole->setParameter(Calibration::Parameters::cy, 0.5);
    }

    virtual void teardown()
    {

    }

    std::shared_ptr<Calibration> mCalibrationRadial1;
    std::shared_ptr<Calibration> mCalibrationRadial2;
    std::shared_ptr<Calibration> mCalibrationRadial3;
    std::shared_ptr<Calibration> mCalibrationSimpleRadialFisheye;
    std::shared_ptr<Calibration> mCalibrationRadialFisheye;
    std::shared_ptr<Calibration> mCalibrationOpenCV;
    std::shared_ptr<Calibration> mCalibrationOpenCVFisheye;
    std::shared_ptr<Calibration> mCalibrationOpenCVFull;
    std::shared_ptr<Calibration> mCalibrationSimplePinhole;
    std::shared_ptr<Calibration> mCalibrationPinhole;
};


BOOST_FIXTURE_TEST_CASE(name, CalibrationTest)
{
    BOOST_CHECK_EQUAL("Radial 1", mCalibrationRadial1->name());
    BOOST_CHECK_EQUAL("Radial 2", mCalibrationRadial2->name());
    BOOST_CHECK_EQUAL("Radial 3", mCalibrationRadial3->name());
    BOOST_CHECK_EQUAL("Radial Fisheye 1", mCalibrationSimpleRadialFisheye->name());
    BOOST_CHECK_EQUAL("Radial Fisheye 2", mCalibrationRadialFisheye->name());
    BOOST_CHECK_EQUAL("OpenCV 1", mCalibrationOpenCV->name());
    BOOST_CHECK_EQUAL("OpenCV Fisheye", mCalibrationOpenCVFisheye->name());
    BOOST_CHECK_EQUAL("OpenCV 2", mCalibrationOpenCVFull->name());
    BOOST_CHECK_EQUAL("Pinhole 1", mCalibrationSimplePinhole->name());
    BOOST_CHECK_EQUAL("Pinhole 2", mCalibrationPinhole->name());
}

BOOST_FIXTURE_TEST_CASE(parameterName, CalibrationTest)
{
    BOOST_CHECK_EQUAL("f", mCalibrationRadial1->parameterName(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL("fx", mCalibrationRadial2->parameterName(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL("fy", mCalibrationRadial3->parameterName(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL("cx", mCalibrationSimpleRadialFisheye->parameterName(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL("cy", mCalibrationRadialFisheye->parameterName(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL("k1", mCalibrationOpenCV->parameterName(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL("k2", mCalibrationOpenCVFisheye->parameterName(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL("k3", mCalibrationOpenCVFull->parameterName(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL("k4", mCalibrationSimplePinhole->parameterName(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL("k5", mCalibrationOpenCVFull->parameterName(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL("k6", mCalibrationSimplePinhole->parameterName(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL("p1", mCalibrationOpenCVFull->parameterName(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL("p2", mCalibrationSimplePinhole->parameterName(Calibration::Parameters::p2));
}

BOOST_FIXTURE_TEST_CASE(iterator, CalibrationTest)
{
    Calibration::iterator it = mCalibrationRadial1->begin();

    BOOST_CHECK(Calibration::Parameters::focal == it->first);
    BOOST_CHECK_EQUAL(4000., it->second);

    it++;
    BOOST_CHECK(Calibration::Parameters::cx == it->first);
    BOOST_CHECK_EQUAL(0.6, it->second);

    it++;
    BOOST_CHECK(Calibration::Parameters::cy == it->first);
    BOOST_CHECK_EQUAL(0.5, it->second);

    it++;
    BOOST_CHECK(Calibration::Parameters::k1 == it->first);
    BOOST_CHECK_EQUAL(0.005, it->second);

    Calibration::iterator it2 = mCalibrationRadial2->begin();

    BOOST_CHECK(Calibration::Parameters::focal == it2->first);
    BOOST_CHECK_EQUAL(4000., it2->second);

    it2++;
    BOOST_CHECK(Calibration::Parameters::cx == it2->first);
    BOOST_CHECK_EQUAL(0.6, it2->second);

    it2++;
    BOOST_CHECK(Calibration::Parameters::cy == it2->first);
    BOOST_CHECK_EQUAL(0.5, it2->second);

    it2++;
    BOOST_CHECK(Calibration::Parameters::k1 == it2->first);
    BOOST_CHECK_EQUAL(0.005, it2->second);

    it2++;
    BOOST_CHECK(Calibration::Parameters::k2 == it2->first);
    BOOST_CHECK_EQUAL(0.001, it2->second);

    Calibration::iterator it3 = mCalibrationRadial3->begin();

    BOOST_CHECK(Calibration::Parameters::focal == it3->first);
    BOOST_CHECK_EQUAL(4000., it3->second);

    it3++;
    BOOST_CHECK(Calibration::Parameters::cx == it3->first);
    BOOST_CHECK_EQUAL(0.6, it3->second);

    it3++;
    BOOST_CHECK(Calibration::Parameters::cy == it3->first);
    BOOST_CHECK_EQUAL(0.5, it3->second);

    it3++;
    BOOST_CHECK(Calibration::Parameters::k1 == it3->first);
    BOOST_CHECK_EQUAL(0.005, it3->second);

    it3++;
    BOOST_CHECK(Calibration::Parameters::k2 == it3->first);
    BOOST_CHECK_EQUAL(0.001, it3->second);

    it3++;
    BOOST_CHECK(Calibration::Parameters::k3 == it3->first);
    BOOST_CHECK_EQUAL(0.0001, it3->second);

    it3++;
    BOOST_CHECK(Calibration::Parameters::p1 == it3->first);
    BOOST_CHECK_EQUAL(0.01, it3->second);

    it3++;
    BOOST_CHECK(Calibration::Parameters::p2 == it3->first);
    BOOST_CHECK_EQUAL(0.001, it3->second);

    Calibration::iterator it4 = mCalibrationSimpleRadialFisheye->begin();

    BOOST_CHECK(Calibration::Parameters::focal == it4->first);
    BOOST_CHECK_EQUAL(4000., it4->second);

    it4++;
    BOOST_CHECK(Calibration::Parameters::cx == it4->first);
    BOOST_CHECK_EQUAL(0.6, it4->second);

    it4++;
    BOOST_CHECK(Calibration::Parameters::cy == it4->first);
    BOOST_CHECK_EQUAL(0.5, it4->second);

    it4++;
    BOOST_CHECK(Calibration::Parameters::k1 == it4->first);
    BOOST_CHECK_EQUAL(0.005, it4->second);

    Calibration::iterator it5 = mCalibrationRadialFisheye->begin();

    BOOST_CHECK(Calibration::Parameters::focal == it5->first);
    BOOST_CHECK_EQUAL(4000., it5->second);

    it5++;
    BOOST_CHECK(Calibration::Parameters::cx == it5->first);
    BOOST_CHECK_EQUAL(0.6, it5->second);

    it5++;
    BOOST_CHECK(Calibration::Parameters::cy == it5->first);
    BOOST_CHECK_EQUAL(0.5, it5->second);

    it5++;
    BOOST_CHECK(Calibration::Parameters::k1 == it5->first);
    BOOST_CHECK_EQUAL(0.005, it5->second);

    it5++;
    BOOST_CHECK(Calibration::Parameters::k2 == it5->first);
    BOOST_CHECK_EQUAL(0.001, it5->second);

    Calibration::iterator it6 = mCalibrationOpenCV->begin();

    BOOST_CHECK(Calibration::Parameters::focalx == it6->first);
    BOOST_CHECK_EQUAL(4000., it6->second);

    it6++;
    BOOST_CHECK(Calibration::Parameters::focaly == it6->first);
    BOOST_CHECK_EQUAL(4000., it6->second);

    it6++;
    BOOST_CHECK(Calibration::Parameters::cx == it6->first);
    BOOST_CHECK_EQUAL(0.6, it6->second);

    it6++;
    BOOST_CHECK(Calibration::Parameters::cy == it6->first);
    BOOST_CHECK_EQUAL(0.5, it6->second);

    it6++;
    BOOST_CHECK(Calibration::Parameters::k1 == it6->first);
    BOOST_CHECK_EQUAL(0.005, it6->second);

    it6++;
    BOOST_CHECK(Calibration::Parameters::k2 == it6->first);
    BOOST_CHECK_EQUAL(0.001, it6->second);

    it6++;
    BOOST_CHECK(Calibration::Parameters::p1 == it6->first);
    BOOST_CHECK_EQUAL(0.01, it6->second);

    it6++;
    BOOST_CHECK(Calibration::Parameters::p2 == it6->first);
    BOOST_CHECK_EQUAL(0.001, it6->second);

    Calibration::iterator it7 = mCalibrationOpenCVFisheye->begin();

    BOOST_CHECK(Calibration::Parameters::focalx == it7->first);
    BOOST_CHECK_EQUAL(4000., it7->second);

    it7++;
    BOOST_CHECK(Calibration::Parameters::focaly == it7->first);
    BOOST_CHECK_EQUAL(4000., it7->second);

    it7++;
    BOOST_CHECK(Calibration::Parameters::cx == it7->first);
    BOOST_CHECK_EQUAL(0.6, it7->second);

    it7++;
    BOOST_CHECK(Calibration::Parameters::cy == it7->first);
    BOOST_CHECK_EQUAL(0.5, it7->second);

    it7++;
    BOOST_CHECK(Calibration::Parameters::k1 == it7->first);
    BOOST_CHECK_EQUAL(0.005, it7->second);

    it7++;
    BOOST_CHECK(Calibration::Parameters::k2 == it7->first);
    BOOST_CHECK_EQUAL(0.001, it7->second);

    it7++;
    BOOST_CHECK(Calibration::Parameters::k3 == it7->first);
    BOOST_CHECK_EQUAL(0.0005, it7->second);

    it7++;
    BOOST_CHECK(Calibration::Parameters::k4 == it7->first);
    BOOST_CHECK_EQUAL(0.0001, it7->second);

    Calibration::iterator it8 = mCalibrationOpenCVFull->begin();

    BOOST_CHECK(Calibration::Parameters::focalx == it8->first);
    BOOST_CHECK_EQUAL(4000., it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::focaly == it8->first);
    BOOST_CHECK_EQUAL(4000., it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::cx == it8->first);
    BOOST_CHECK_EQUAL(0.6, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::cy == it8->first);
    BOOST_CHECK_EQUAL(0.5, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::k1 == it8->first);
    BOOST_CHECK_EQUAL(0.005, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::k2 == it8->first);
    BOOST_CHECK_EQUAL(0.001, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::k3 == it8->first);
    BOOST_CHECK_EQUAL(0.0005, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::k4 == it8->first);
    BOOST_CHECK_EQUAL(0.0001, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::k5 == it8->first);
    BOOST_CHECK_EQUAL(0.00005, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::k6 == it8->first);
    BOOST_CHECK_EQUAL(0.00001, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::p1 == it8->first);
    BOOST_CHECK_EQUAL(0.01, it8->second);

    it8++;
    BOOST_CHECK(Calibration::Parameters::p2 == it8->first);
    BOOST_CHECK_EQUAL(0.001, it8->second);

    Calibration::iterator it9 = mCalibrationSimplePinhole->begin();

    BOOST_CHECK(Calibration::Parameters::focal == it9->first);
    BOOST_CHECK_EQUAL(4000., it9->second);

    it9++;
    BOOST_CHECK(Calibration::Parameters::cx == it9->first);
    BOOST_CHECK_EQUAL(0.6, it9->second);

    it9++;
    BOOST_CHECK(Calibration::Parameters::cy == it9->first);
    BOOST_CHECK_EQUAL(0.5, it9->second);

    Calibration::iterator it10 = mCalibrationPinhole->begin();

    BOOST_CHECK(Calibration::Parameters::focalx == it10->first);
    BOOST_CHECK_EQUAL(4000., it10->second);

    it10++;
    BOOST_CHECK(Calibration::Parameters::focaly == it10->first);
    BOOST_CHECK_EQUAL(4000., it10->second);

    it10++;
    BOOST_CHECK(Calibration::Parameters::cx == it10->first);
    BOOST_CHECK_EQUAL(0.6, it10->second);

    it10++;
    BOOST_CHECK(Calibration::Parameters::cy == it10->first);
    BOOST_CHECK_EQUAL(0.5, it10->second);
}

BOOST_FIXTURE_TEST_CASE(parameter, CalibrationTest)
{
    BOOST_CHECK_EQUAL(4000, mCalibrationRadial1->parameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(0.6, mCalibrationRadial1->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationRadial1->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationRadial1->parameter(Calibration::Parameters::k1));

    BOOST_CHECK_EQUAL(4000, mCalibrationRadial2->parameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(0.6, mCalibrationRadial2->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationRadial2->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationRadial2->parameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationRadial2->parameter(Calibration::Parameters::k2));

    BOOST_CHECK_EQUAL(4000, mCalibrationRadial3->parameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(0.6, mCalibrationRadial3->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationRadial3->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationRadial3->parameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationRadial3->parameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(0.0001, mCalibrationRadial3->parameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(0.01, mCalibrationRadial3->parameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationRadial3->parameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(4000, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(0.6, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::k1));

    BOOST_CHECK_EQUAL(4000, mCalibrationRadialFisheye->parameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(0.6, mCalibrationRadialFisheye->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationRadialFisheye->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationRadialFisheye->parameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationRadialFisheye->parameter(Calibration::Parameters::k2));

    BOOST_CHECK_EQUAL(4000, mCalibrationOpenCV->parameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(4000, mCalibrationOpenCV->parameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(0.6, mCalibrationOpenCV->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationOpenCV->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationOpenCV->parameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationOpenCV->parameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(0.01, mCalibrationOpenCV->parameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationOpenCV->parameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(4000, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(4000, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(0.6, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(0.0005, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(0.0001, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k4));

    BOOST_CHECK_EQUAL(4000, mCalibrationOpenCVFull->parameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(4000, mCalibrationOpenCVFull->parameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(0.6, mCalibrationOpenCVFull->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationOpenCVFull->parameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(0.005, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(0.0005, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(0.0001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(0.00005, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(0.00001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(0.01, mCalibrationOpenCVFull->parameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(0.001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(4000, mCalibrationSimplePinhole->parameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(0.6, mCalibrationSimplePinhole->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationSimplePinhole->parameter(Calibration::Parameters::cy));

    BOOST_CHECK_EQUAL(4000, mCalibrationPinhole->parameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(4000, mCalibrationPinhole->parameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(0.6, mCalibrationPinhole->parameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(0.5, mCalibrationPinhole->parameter(Calibration::Parameters::cy));
}

BOOST_FIXTURE_TEST_CASE(existParameter, CalibrationTest)
{
    BOOST_CHECK_EQUAL(true, mCalibrationRadial1->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial1->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial1->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial1->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->existParameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(true, mCalibrationRadial2->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial2->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial2->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial2->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial2->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->existParameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->existParameter(Calibration::Parameters::p2));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->existParameter(Calibration::Parameters::k6));

    BOOST_CHECK_EQUAL(true, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(true, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->existParameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(true, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(true, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(true, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->existParameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->existParameter(Calibration::Parameters::p2));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCV->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCV->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCV->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCV->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCV->existParameter(Calibration::Parameters::k6));

    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFisheye->existParameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::p2));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFull->existParameter(Calibration::Parameters::focal));

    BOOST_CHECK_EQUAL(true, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(true, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->existParameter(Calibration::Parameters::p2));

    BOOST_CHECK_EQUAL(true, mCalibrationPinhole->existParameter(Calibration::Parameters::focalx));
    BOOST_CHECK_EQUAL(true, mCalibrationPinhole->existParameter(Calibration::Parameters::focaly));
    BOOST_CHECK_EQUAL(true, mCalibrationPinhole->existParameter(Calibration::Parameters::cx));
    BOOST_CHECK_EQUAL(true, mCalibrationPinhole->existParameter(Calibration::Parameters::cy));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::focal));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::k1));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::k2));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::k3));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::k4));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::k5));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::k6));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::p1));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->existParameter(Calibration::Parameters::p2));
}

BOOST_FIXTURE_TEST_CASE(check_camera_type, CalibrationTest)
{
    BOOST_CHECK_EQUAL(true, mCalibrationRadial1->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial1->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(true, mCalibrationRadial2->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial2->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(true, mCalibrationRadial3->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(false, mCalibrationRadial3->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(true, mCalibrationSimpleRadialFisheye->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(true, mCalibrationSimpleRadialFisheye->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(false, mCalibrationSimpleRadialFisheye->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(true, mCalibrationRadialFisheye->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(true, mCalibrationRadialFisheye->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(false, mCalibrationRadialFisheye->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCV->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCV->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCV->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFisheye->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFisheye->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFull->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(false, mCalibrationOpenCVFull->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(true, mCalibrationOpenCVFull->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(true, mCalibrationSimplePinhole->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(false, mCalibrationSimplePinhole->checkCameraType(Calibration::CameraType::opencv));

    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->checkCameraType(Calibration::CameraType::radial));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->checkCameraType(Calibration::CameraType::fisheye));
    BOOST_CHECK_EQUAL(true, mCalibrationPinhole->checkCameraType(Calibration::CameraType::pinhole));
    BOOST_CHECK_EQUAL(false, mCalibrationPinhole->checkCameraType(Calibration::CameraType::opencv));

}

BOOST_AUTO_TEST_SUITE_END()