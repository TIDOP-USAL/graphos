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

#define BOOST_TEST_MODULE GRAPHOS Undistort test

#include <boost/test/unit_test.hpp>
#include "graphos/core/camera/Undistort.h"

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestUndistortSuite)

struct TestUndistort
{

  TestUndistort() 
  {
  }

  ~TestUndistort()
  {
  }

  void setup()
  {
    calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
    calibration->setParameter(Calibration::Parameters::focalx, 3859.3453965517);
    calibration->setParameter(Calibration::Parameters::focaly, 3862.6730228051);
    calibration->setParameter(Calibration::Parameters::cx, 2374.7230913013);
    calibration->setParameter(Calibration::Parameters::cy, 1596.5530498987);
    calibration->setParameter(Calibration::Parameters::k1, 0.0004829397);
    calibration->setParameter(Calibration::Parameters::k2, -0.0003108023);
    calibration->setParameter(Calibration::Parameters::p1, 0.0000801173);
    calibration->setParameter(Calibration::Parameters::p2, -0.0022438688);

    calibration2 = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
    calibration2->setParameter(Calibration::Parameters::focalx, 3639.8586374454);
    calibration2->setParameter(Calibration::Parameters::focaly, 3643.7449140809);
    calibration2->setParameter(Calibration::Parameters::cx, 2725.6294535589);
    calibration2->setParameter(Calibration::Parameters::cy, 1858.8093504647);
    calibration2->setParameter(Calibration::Parameters::k1, 0.0658540516);
    calibration2->setParameter(Calibration::Parameters::k2, -0.0230262070);
    calibration2->setParameter(Calibration::Parameters::k3, 0.0472457021);
    calibration2->setParameter(Calibration::Parameters::k4, -0.0585949530);
  }

  void teardown()
  {

  }

  std::shared_ptr<Calibration> calibration;
  std::shared_ptr<Calibration> calibration2;
};


BOOST_FIXTURE_TEST_CASE(opencv_camera_matrix, TestUndistort)
{
  cv::Mat camera_matrix = openCVCameraMatrix(*calibration);
  
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(0, 0), 3859.3453965517, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(0, 1), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(0, 2), 2374.7230913013, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(1, 0), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(1, 1), 3862.6730228051, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(1, 2), 1596.5530498987, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(2, 0), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(2, 1), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix.at<float>(2, 2), 1.0, 0.001);

  cv::Mat camera_matrix2 = openCVCameraMatrix(*calibration2);

  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(0, 0), 3639.8586374454, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(0, 1), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(0, 2), 2725.6294535589, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(1, 0), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(1, 1), 3643.7449140809, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(1, 2), 1858.8093504647, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(2, 0), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(2, 1), 0.0, 0.001);
  BOOST_CHECK_CLOSE(camera_matrix2.at<float>(2, 2), 1.0, 0.001);
}

BOOST_FIXTURE_TEST_CASE(opencv_distortion_coefficients, TestUndistort)
{
  cv::Mat distortion_coefficients = openCVDistortionCoefficients(*calibration);

  BOOST_CHECK_EQUAL(4, distortion_coefficients.cols);
  BOOST_CHECK_CLOSE(distortion_coefficients.at<float>(0), 0.0004829397, 0.001);
  BOOST_CHECK_CLOSE(distortion_coefficients.at<float>(1), -0.0003108023, 0.001);
  BOOST_CHECK_CLOSE(distortion_coefficients.at<float>(2), 0.0000801173, 0.001);
  BOOST_CHECK_CLOSE(distortion_coefficients.at<float>(3), -0.0022438688, 0.001);

  cv::Mat distortion_coefficients2 = openCVDistortionCoefficients(*calibration2);

  BOOST_CHECK_EQUAL(4, distortion_coefficients2.cols);
  BOOST_CHECK_CLOSE(distortion_coefficients2.at<float>(0), 0.0658540516, 0.001);
  BOOST_CHECK_CLOSE(distortion_coefficients2.at<float>(1), -0.0230262070, 0.001);
  BOOST_CHECK_CLOSE(distortion_coefficients2.at<float>(2), 0.0472457021, 0.001);
  BOOST_CHECK_CLOSE(distortion_coefficients2.at<float>(3), -0.0585949530, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()