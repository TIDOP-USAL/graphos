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

#include "graphos/core/ortho/Orthorectification.h"

#include "graphos/core/camera/Utils.h"

#include <opencv2/calib3d.hpp>
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif

namespace graphos
{

Orthorectification::Orthorectification(const tl::Path &dtm, 
                                       const Camera &camera,
                                       const CameraPose &cameraPose)
  : mDtmReader(tl::ImageReaderFactory::createReader(dtm)),
    mCamera(camera),
    mCameraPose(cameraPose),
    mIniZ(0.),
    mNoDataValue(-std::numeric_limits<double>().max()),
    bCuda(false)
{
  init();
}

tl::PointI Orthorectification::terrainToImage(const tl::Point3D &terrainPoint) const
{
  tl::PointI image_coordinates;

  try {

    tl::PointD photocoordinates = mDifferentialRectification->backwardProjection(terrainPoint);
    image_coordinates = photocoordinatesToImage(photocoordinates);

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return image_coordinates;
}

tl::PointI Orthorectification::terrainToPhotocoordinates(const tl::Point3D &terrainPoint) const
{
  tl::PointI photocoordinates;

  try {

    photocoordinates = mDifferentialRectification->backwardProjection(terrainPoint);

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return photocoordinates;
}

tl::Point3D Orthorectification::imageToTerrain(const tl::PointI &imageCoordinates) const
{
  tl::Point3D terrain_coordinates;

  try {

    tl::PointI photo_coordinates = imageToPhotocoordinates(imageCoordinates);
    terrain_coordinates = photocoordinatesToTerrain(photo_coordinates);

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return terrain_coordinates;
}

tl::Point3D Orthorectification::photocoordinatesToTerrain(const tl::PointI &photocoordinates) const
{
  double z = mIniZ;
  int it = 10;
  tl::Point3D terrain_coordinates;

  try {

    terrain_coordinates = mDifferentialRectification->forwardProjection(photocoordinates, z);
    double z2;

    while (it > 0) {

      tl::PointI image_point = terrainToDTM(terrain_coordinates);
      tl::RectI rect_full(tl::PointI(), mDtmReader->cols(), mDtmReader->rows());

      tl::PointD pt(terrain_coordinates.x, terrain_coordinates.y);
      if (rect_full.contains(image_point)) {
        tl::RectI rect(image_point, 1, 1);
        cv::Mat image = mDtmReader->read(rect); 
        if (!image.empty()) {
          z2 = image.at<float>(0, 0);
          if (std::abs(z2 - z) > 0.1 && z2 != mNoDataValue) {
            terrain_coordinates = mDifferentialRectification->forwardProjection(photocoordinates, z2);
            z = z2;
          } else {
            break;
          }
        }
      } else {
        break;
      }
      it--;
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return terrain_coordinates;
}

tl::PointI Orthorectification::imageToPhotocoordinates(const tl::PointI &imagePoint) const
{
  return mAffineImageToPhotocoordinates.transform(imagePoint);
}

tl::PointI Orthorectification::photocoordinatesToImage(const tl::PointI &photocoordinates) const
{
  return mAffineImageToPhotocoordinates.transform(photocoordinates, tl::Transform::Order::inverse);
}

tl::Point3D Orthorectification::dtmToTerrain(const tl::PointI &imagePoint) const
{
  tl::Point3D dtm_terrain_point;
  
  try {

    dtm_terrain_point = mAffineDtmImageToTerrain.transform(imagePoint);
    dtm_terrain_point.z = mDtm.at<float>(imagePoint.y - mRectDtm.y, imagePoint.x - mRectDtm.x);

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return dtm_terrain_point;
}

tl::PointI Orthorectification::terrainToDTM(const tl::Point3D &terrainPoint) const
{
  return mAffineDtmImageToTerrain.transform(terrainPoint, tl::Transform::Order::inverse);
}

double Orthorectification::z(const tl::PointD &terrainPoint) const
{
  double z = mNoDataValue;

  try {

    tl::RectI rect(terrainToDTM(terrainPoint), 1, 1);
    cv::Mat image = mDtmReader->read(rect);
    if (!image.empty()) {
      z = image.at<float>(0, 0);
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return z;
}

tl::Rect<int> Orthorectification::rectImage() const
{
  return mRectImage;
}

tl::Rect<int> Orthorectification::rectDtm() const
{
  return mRectDtm;
}

tl::graph::GPolygon Orthorectification::footprint() const
{
  return mFootprint;
}

CameraPose Orthorectification::orientation() const
{
  return mCameraPose;
}

Camera Orthorectification::camera() const
{
  return mCamera;
}

Camera Orthorectification::undistortCamera() const
{
  return Camera();
}

bool Orthorectification::hasNodataValue() const
{
  return mNoDataValue != -std::numeric_limits<double>().max();
}

double Orthorectification::nodataValue() const
{
  return mNoDataValue;
}

void Orthorectification::init()
{
  initUndistortCamera();

  mDtmReader->open();

  mAffineDtmImageToTerrain = mDtmReader->georeference();

  mWindowDtmTerrainExtension.pt1.x = mAffineDtmImageToTerrain.tx;
  mWindowDtmTerrainExtension.pt1.y = mAffineDtmImageToTerrain.ty;
  mWindowDtmTerrainExtension.pt2.x = mAffineDtmImageToTerrain.tx + mAffineDtmImageToTerrain.scaleX() * mDtmReader->cols();
  mWindowDtmTerrainExtension.pt2.y = mAffineDtmImageToTerrain.ty + mAffineDtmImageToTerrain.scaleY() * mDtmReader->rows();
  
  mDifferentialRectification = std::make_unique<tl::geospatial::DifferentialRectification>(mCameraPose.rotationMatrix(),
                                                                                           mCameraPose.position(),
                                                                                           focal());

  bool exist_nodata = false;
  double nodata_value = mDtmReader->noDataValue(&exist_nodata);
  if (exist_nodata) mNoDataValue = nodata_value;

  tl::PointD center_project = imageToTerrain(mRectImage.window().center());

  tl::WindowD w(center_project,
                mAffineDtmImageToTerrain.scaleX(),
                mAffineDtmImageToTerrain.scaleY());
  cv::Mat image = mDtmReader->read(w);
  mIniZ = image.at<float>(0, 0);

  tl::PointF principal_point = this->principalPoint();
  mAffineImageToPhotocoordinates = tl::Affine<tl::PointI>(-principal_point.x, principal_point.y, 1, -1, 0);

  mRectImage = tl::Rect<int>(0, 0, mCamera.width(), mCamera.height());

  tl::RectI rect_full(tl::PointI(), mDtmReader->cols(), mDtmReader->rows());

  mFootprint.push_back(imageToTerrain(mRectImage.topLeft()));
  mFootprint.push_back(imageToTerrain(mRectImage.topRight()));
  mFootprint.push_back(imageToTerrain(mRectImage.bottomRight()));
  mFootprint.push_back(imageToTerrain(mRectImage.bottomLeft()));

  tl::WindowD window_terrain = mFootprint.window();

  tl::PointI window_dtm_image_pt1 = terrainToDTM(window_terrain.pt1);
  tl::PointI window_dtm_image_pt2 = terrainToDTM(window_terrain.pt2);

  tl::RectI rect(window_dtm_image_pt1, window_dtm_image_pt2);
  rect.normalized();
  mRectDtm = intersect(rect, rect_full);
  mDtm = mDtmReader->read(mRectDtm);
}

void Orthorectification::initUndistortCamera()
{
  std::shared_ptr<Calibration> calibration = mCamera.calibration();
  
  cv::Mat cameraMatrix = openCVCameraMatrix(*calibration);
  cv::Mat dist_coeffs = openCVDistortionCoefficients(*calibration);
  
  cv::Size imageSize(static_cast<int>(mCamera.width()),
                     static_cast<int>(mCamera.height()));
  
  cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, dist_coeffs, imageSize, 1, imageSize, nullptr);
  
  mUndistortCamera = mCamera;
  mUndistortCamera.setFocal((optCameraMat.at<float>(0, 0)+ optCameraMat.at<float>(1, 1))/2.);
  std::shared_ptr<Calibration> undistort_calibration = CalibrationFactory::create(calibration->cameraModel());
  undistort_calibration->setParameter(Calibration::Parameters::focal, (optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.);
  undistort_calibration->setParameter(Calibration::Parameters::focalx, optCameraMat.at<float>(0, 0));
  undistort_calibration->setParameter(Calibration::Parameters::focaly, optCameraMat.at<float>(1, 1));
  undistort_calibration->setParameter(Calibration::Parameters::cx, optCameraMat.at<float>(0, 2));
  undistort_calibration->setParameter(Calibration::Parameters::cy, optCameraMat.at<float>(1, 2));
  mUndistortCamera.setCalibration(undistort_calibration);

}

float Orthorectification::focal() const
{
  float focal_x = 1.f;
  float focal_y = 1.f;

  std::shared_ptr<Calibration> calibration = mUndistortCamera.calibration();

  if (calibration->existParameter(Calibration::Parameters::focal)) {
    focal_x = static_cast<float>(calibration->parameter(Calibration::Parameters::focal));
    focal_y = static_cast<float>(calibration->parameter(Calibration::Parameters::focal));
  } else {
    focal_x = static_cast<float>(calibration->parameter(Calibration::Parameters::focalx));
    focal_y = static_cast<float>(calibration->parameter(Calibration::Parameters::focaly));
  }

  return (focal_x + focal_y) / 2.f;
}

tl::PointF Orthorectification::principalPoint() const
{
  tl::PointF principal_point;

  std::shared_ptr<Calibration> calibration = mUndistortCamera.calibration();

  principal_point.x = static_cast<float>(calibration->parameter(Calibration::Parameters::cx));
  principal_point.y = static_cast<float>(calibration->parameter(Calibration::Parameters::cy));

  return principal_point;
}

cv::Mat Orthorectification::distCoeffs() const
{
  std::shared_ptr<Calibration> calibration = mUndistortCamera.calibration();

  cv::Mat dist_coeffs = openCVDistortionCoefficients(*calibration);

  return dist_coeffs;
}

void Orthorectification::setCuda(bool active)
{
  bCuda = active;
}

cv::Mat Orthorectification::undistort(const cv::Mat &image)
{
  cv::Mat img_undistort;

  TL_TODO("No debería calcular la cámara y los coeficientes de distorsión cada vez")

  try {

    std::shared_ptr<Calibration> calibration = mCamera.calibration();

    cv::Mat cameraMatrix = openCVCameraMatrix(*calibration);
    cv::Mat distCoeffs = openCVDistortionCoefficients(*calibration);
    cv::Size imageSize(static_cast<int>(mCamera.width()),
                       static_cast<int>(mCamera.height()));

    cv::Mat map1;
    cv::Mat map2;
    cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr);
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), optCameraMat, imageSize, CV_32FC1, map1, map2);

#ifdef HAVE_OPENCV_CUDAWARPING
    if (bCuda) {
      cv::cuda::GpuMat gMap1(map1);
      cv::cuda::GpuMat gMap2(map2);
      cv::cuda::GpuMat gImgOut(image);
      cv::cuda::GpuMat gImgUndistort;

      cv::cuda::remap(gImgOut, gImgUndistort, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar());
      gImgUndistort.download(img_undistort);
    } else {
#endif

    cv::remap(image, img_undistort, map1, map2, cv::INTER_LINEAR);

#ifdef HAVE_OPENCV_CUDAWARPING
    }
#endif

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return img_undistort;
}

bool Orthorectification::isValid() const
{
  return mIniZ != mNoDataValue;
}


} // namespace graphos