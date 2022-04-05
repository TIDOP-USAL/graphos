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

#include "graphos/core/camera/Undistort.h"

#include <opencv2/calib3d.hpp>
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif

namespace graphos
{

cv::Mat openCVCameraMatrix(const Calibration &calibration)
{
  float focal_x{};
  float focal_y{};

  if (calibration.existParameter(Calibration::Parameters::focal)) {
    focal_x = static_cast<float>(calibration.parameter(Calibration::Parameters::focal));
    focal_y = static_cast<float>(calibration.parameter(Calibration::Parameters::focal));
  } else {
    focal_x = static_cast<float>(calibration.parameter(Calibration::Parameters::focalx));
    focal_y = static_cast<float>(calibration.parameter(Calibration::Parameters::focaly));
  }

  float ppx = static_cast<float>(calibration.parameter(Calibration::Parameters::cx));
  float ppy = static_cast<float>(calibration.parameter(Calibration::Parameters::cy));

  std::array<std::array<float, 3>, 3> camera_matrix_data = {focal_x, 0.f, ppx,
                                                            0.f, focal_y, ppy,
                                                            0.f, 0.f, 1.f};
  cv::Mat cameraMatrix(3, 3, CV_32F, camera_matrix_data.data());

  return cameraMatrix.clone();
}

cv::Mat openCVDistortionCoefficients(const Calibration &calibration)
{
  Calibration::CameraModel camera_model = calibration.cameraModel();
  bool b_fisheye = calibration.checkCameraType(Calibration::CameraType::fisheye);

  size_t distCoeffs_size = 4;

  switch (camera_model) {
    case Calibration::CameraModel::radial1:
      distCoeffs_size = 4;
      break;
    case Calibration::CameraModel::radial2:
      distCoeffs_size = 4;
      break;
    case Calibration::CameraModel::radial3:
      distCoeffs_size = 5;
      break;
    case Calibration::CameraModel::simple_radial_fisheye:
      distCoeffs_size = 4;
      break;
    case Calibration::CameraModel::radial_fisheye:
      distCoeffs_size = 4;
      break;
    case Calibration::CameraModel::opencv:
      distCoeffs_size = 4;
      break;
    case Calibration::CameraModel::opencv_fisheye:
      distCoeffs_size = 4;
      break;
    case Calibration::CameraModel::opencv_full:
      distCoeffs_size = 8;
      break;
    case Calibration::CameraModel::simple_pinhole:
      distCoeffs_size = 4;
      break;
    case Calibration::CameraModel::pinhole:
      distCoeffs_size = 4;
      break;
    default:
      break;
  }

  cv::Mat distCoeffs = cv::Mat::zeros(1, distCoeffs_size, CV_32F);

  distCoeffs.at<float>(0) = calibration.existParameter(Calibration::Parameters::k1) ?
                            static_cast<float>(calibration.parameter(Calibration::Parameters::k1)) : 0.f;
  distCoeffs.at<float>(1) = calibration.existParameter(Calibration::Parameters::k2) ?
                            static_cast<float>(calibration.parameter(Calibration::Parameters::k2)) : 0.f;
  if (b_fisheye) {
    distCoeffs.at<float>(2) = calibration.existParameter(Calibration::Parameters::k3) ?
                              static_cast<float>(calibration.parameter(Calibration::Parameters::k3)) : 0.f;
    distCoeffs.at<float>(3) = calibration.existParameter(Calibration::Parameters::k4) ?
                              static_cast<float>(calibration.parameter(Calibration::Parameters::k4)) : 0.f;
  } else {

    distCoeffs.at<float>(2) = calibration.existParameter(Calibration::Parameters::p1) ?
                              static_cast<float>(calibration.parameter(Calibration::Parameters::p1)) : 0.f;
    distCoeffs.at<float>(3) = calibration.existParameter(Calibration::Parameters::p2) ?
                              static_cast<float>(calibration.parameter(Calibration::Parameters::p2)) : 0.f;

    if (camera_model == Calibration::CameraModel::opencv_full ||
        camera_model == Calibration::CameraModel::radial3) {
      distCoeffs.at<float>(4) = calibration.existParameter(Calibration::Parameters::k3) ?
                                static_cast<float>(calibration.parameter(Calibration::Parameters::k3)) : 0.f;
    }

    if (camera_model == Calibration::CameraModel::opencv_full) {
      distCoeffs.at<float>(5) = calibration.existParameter(Calibration::Parameters::k4) ?
                                static_cast<float>(calibration.parameter(Calibration::Parameters::k4)) : 0.f;
      distCoeffs.at<float>(6) = calibration.existParameter(Calibration::Parameters::k5) ?
                                static_cast<float>(calibration.parameter(Calibration::Parameters::k5)) : 0.f;
      distCoeffs.at<float>(7) = calibration.existParameter(Calibration::Parameters::k6) ?
                                static_cast<float>(calibration.parameter(Calibration::Parameters::k6)) : 0.f;
    }
  }

  return distCoeffs;
}


//UndistortTask::UndistortTask()
//{
//}
//
//void UndistortTask::execute(tl::Progress *progressBar)
//{
//
//}

Camera undistortCamera(const Camera &camera)
{
  std::shared_ptr<Calibration> calibration = camera.calibration();

  cv::Mat cameraMatrix = openCVCameraMatrix(*calibration);
  cv::Mat dist_coeffs = openCVDistortionCoefficients(*calibration);

  cv::Size imageSize(static_cast<int>(camera.width()),
                     static_cast<int>(camera.height()));

  cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, dist_coeffs, imageSize, 1, imageSize, nullptr);

  Camera undistort_camera = camera;
  undistort_camera.setFocal((optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.);
  std::shared_ptr<Calibration> undistort_calibration = CalibrationFactory::create(calibration->cameraModel());
  undistort_calibration->setParameter(Calibration::Parameters::focal, (optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.);
  undistort_calibration->setParameter(Calibration::Parameters::focalx, optCameraMat.at<float>(0, 0));
  undistort_calibration->setParameter(Calibration::Parameters::focaly, optCameraMat.at<float>(1, 1));
  undistort_calibration->setParameter(Calibration::Parameters::cx, optCameraMat.at<float>(0, 2));
  undistort_calibration->setParameter(Calibration::Parameters::cy, optCameraMat.at<float>(1, 2));
  undistort_camera.setCalibration(undistort_calibration);
  
  return undistort_camera;
}

} // namespace graphos
