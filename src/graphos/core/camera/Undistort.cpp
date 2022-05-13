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



Undistort::Undistort(Camera camera)
  : mCamera(std::move(camera))
{
  init();
}

Camera Undistort::undistortCamera()
{
  Camera undistort_camera = mCamera;

  double fx = static_cast<double>(mOptimalNewCameraMatrix.at<float>(0, 0));
  double fy = static_cast<double>(mOptimalNewCameraMatrix.at<float>(1, 1));
  double focal = (fx + fy) / 2.;
  undistort_camera.setFocal(focal);
  std::shared_ptr<Calibration> undistort_calibration = CalibrationFactory::create(mCamera.calibration()->cameraModel());
  undistort_calibration->setParameter(Calibration::Parameters::focal, focal);
  undistort_calibration->setParameter(Calibration::Parameters::focalx, mOptimalNewCameraMatrix.at<float>(0, 0));
  undistort_calibration->setParameter(Calibration::Parameters::focaly, mOptimalNewCameraMatrix.at<float>(1, 1));
  undistort_calibration->setParameter(Calibration::Parameters::cx, mOptimalNewCameraMatrix.at<float>(0, 2));
  undistort_calibration->setParameter(Calibration::Parameters::cy, mOptimalNewCameraMatrix.at<float>(1, 2));
  undistort_camera.setCalibration(undistort_calibration);

  return undistort_camera;
}

cv::Mat Undistort::undistortImage(const cv::Mat &image, bool cuda)
{
  cv::Mat img_undistort;

  try {

#ifdef HAVE_OPENCV_CUDAWARPING
    if(cuda) {
      cv::cuda::GpuMat gMap1(mMap1);
      cv::cuda::GpuMat gMap2(mMap2);
      cv::cuda::GpuMat gImgOut(image);
      cv::cuda::GpuMat gImgUndistort;

      cv::cuda::remap(gImgOut, gImgUndistort, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar());
      gImgUndistort.download(img_undistort);
    } else {
#endif

      cv::remap(image, img_undistort, mMap1, mMap2, cv::INTER_LINEAR);

#ifdef HAVE_OPENCV_CUDAWARPING
    }
#endif

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return img_undistort;
}

void Undistort::init()
{
  initCameraMatrix();
  initDistCoeffs();
  initOptimalNewCameraMatrix();
  initUndistortMaps();
}

void Undistort::initCameraMatrix()
{
  mCameraMatrix = openCVCameraMatrix(*mCamera.calibration());
}

void Undistort::initDistCoeffs()
{
  mDistCoeffs = openCVDistortionCoefficients(*mCamera.calibration());
}

void Undistort::initOptimalNewCameraMatrix()
{
  cv::Size size(static_cast<int>(mCamera.width()),
                static_cast<int>(mCamera.height()));

  mOptimalNewCameraMatrix = cv::getOptimalNewCameraMatrix(mCameraMatrix, mDistCoeffs, size, 1, size, nullptr);
}

void Undistort::initUndistortMaps()
{
  cv::Size size(static_cast<int>(mCamera.width()),
                static_cast<int>(mCamera.height()));
  cv::initUndistortRectifyMap(mCameraMatrix, mDistCoeffs, cv::Mat(), mOptimalNewCameraMatrix, size, CV_32FC1, mMap1, mMap2);
}


} // namespace graphos
