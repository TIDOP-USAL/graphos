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

#include "graphos/core/camera/Undistort.h"

#include <tidop/math/geometry/translation.h>

#include <opencv2/calib3d.hpp>
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif

namespace graphos
{

Orthorectification::Orthorectification(const tl::Path &dtm,
                                       const Camera &camera,
                                       const CameraPose &cameraPose)
  : mDtmReader(tl::ImageReaderFactory::create(dtm)),
    mCamera(camera),
    mCameraPose(cameraPose),
    mIniZ(0.),
    mNoDataValue(-std::numeric_limits<double>().max()),
    bCuda(false)
{
    init();
}

tl::Point<int> Orthorectification::terrainToImage(const tl::Point3<double> &terrainPoint) const
{
    tl::Point<int> image_coordinates;

    try {

        tl::Point<double> photocoordinates = mDifferentialRectification->backwardProjection(terrainPoint);
        image_coordinates = photocoordinatesToImage(photocoordinates);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return image_coordinates;
}

tl::Point<int> Orthorectification::terrainToPhotocoordinates(const tl::Point3<double> &terrainPoint) const
{
    tl::Point<int> photocoordinates;

    try {

        photocoordinates = mDifferentialRectification->backwardProjection(terrainPoint);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return photocoordinates;
}

tl::Point3<double> Orthorectification::imageToTerrain(const tl::Point<int> &imageCoordinates) const
{
    tl::Point3<double> terrain_coordinates;

    try {

        tl::Point<int> photo_coordinates = imageToPhotocoordinates(imageCoordinates);
        terrain_coordinates = photocoordinatesToTerrain(photo_coordinates);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return terrain_coordinates;
}

tl::Point3<double> Orthorectification::photocoordinatesToTerrain(const tl::Point<int> &photocoordinates) const
{
    double z = mIniZ;
    int it = 10;
    tl::Point3<double> terrain_coordinates;

    try {

        terrain_coordinates = mDifferentialRectification->forwardProjection(photocoordinates, z);
        double z2;

        while (it > 0) {

            tl::Point<int> image_point = terrainToDTM(terrain_coordinates);
            tl::Rect<int> rect_full(tl::Point<int>(), mDtmReader->cols(), mDtmReader->rows());

            tl::Point<double> pt(terrain_coordinates.x, terrain_coordinates.y);
            if (rect_full.contains(image_point)) {
                tl::Rect<int> rect(image_point, 1, 1);
                cv::Mat image = mDtmReader->read(rect);
                if (!image.empty()) {
                    z2 = image.at<float>(0, 0);
                    if (std::abs(z2 - z) > 0.01 && z2 != mNoDataValue) {
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

tl::Point<int> Orthorectification::imageToPhotocoordinates(const tl::Point<int> &imagePoint) const
{
    return mAffineImageToPhotocoordinates.transform(imagePoint);
}

tl::Point<int> Orthorectification::photocoordinatesToImage(const tl::Point<int> &photocoordinates) const
{
    return mAffineImageToPhotocoordinates.transform(photocoordinates, tl::geom::Transform::Order::inverse);
}

tl::Point3<double> Orthorectification::dtmToTerrain(const tl::Point<int> &imagePoint) const
{
    tl::Point3<double> dtm_terrain_point;

    try {

        dtm_terrain_point = mAffineDtmImageToTerrain.transform(static_cast<tl::Point<double>>(imagePoint));
        dtm_terrain_point.z = mDtm.at<float>(imagePoint.y - mRectDtm.y, imagePoint.x - mRectDtm.x);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return dtm_terrain_point;
}

tl::Point<int> Orthorectification::terrainToDTM(const tl::Point3<double> &terrainPoint) const
{
    auto inverse_transform = mAffineDtmImageToTerrain.inverse();
    return inverse_transform.transform(static_cast<tl::Point<double>>(terrainPoint));
    //return mAffineDtmImageToTerrain.transform(terrainPoint, tl::geom::Transform::Order::inverse);
}

double Orthorectification::z(const tl::Point<double> &terrainPoint) const
{
    double z = mNoDataValue;

    try {

        tl::Rect<int> rect(terrainToDTM(terrainPoint), 1, 1);
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

tl::GPolygon Orthorectification::footprint() const
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
    return mUndistortCamera;
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
    try {

        initUndistortCamera();

        mDtmReader->open();

        mAffineDtmImageToTerrain = mDtmReader->georeference();

        tl::Point<float> principal_point = this->principalPoint();
        mAffineImageToPhotocoordinates = tl::geom::Affine<tl::Point<int>>(-principal_point.x, principal_point.y, 1, -1, 0);

        mWindowDtmTerrainExtension.pt1.x = mAffineDtmImageToTerrain.translation().x();
        mWindowDtmTerrainExtension.pt1.y = mAffineDtmImageToTerrain.translation().y();
        //mWindowDtmTerrainExtension.pt2.x = mAffineDtmImageToTerrain.tx + mAffineDtmImageToTerrain.scaleX() * mDtmReader->cols();
        //mWindowDtmTerrainExtension.pt2.y = mAffineDtmImageToTerrain.ty + mAffineDtmImageToTerrain.scaleY() * mDtmReader->rows();
        mWindowDtmTerrainExtension.pt2 = mAffineDtmImageToTerrain.transform(tl::Point<double>(mDtmReader->cols(), mDtmReader->rows()));
        mWindowDtmTerrainExtension.normalized();

        mDifferentialRectification = std::make_unique<tl::DifferentialRectification>(mCameraPose.rotationMatrix(),
            mCameraPose.position(),
            focal());

        bool exist_nodata = false;
        double nodata_value = mDtmReader->noDataValue(&exist_nodata);
        if (exist_nodata) mNoDataValue = nodata_value;

        mRectImage = tl::Rect<int>(0, 0, mCamera.width(), mCamera.height());

        // Si no existe DTM en el centro es un problema
        // En lugar de calcular en el centro establezco las coordenadas
        tl::Point<double> center_project = imageToTerrain(mRectImage.window().center());

        // Lo compruebo antes
        if (mWindowDtmTerrainExtension.containsPoint(center_project)) {
            tl::WindowD w(center_project, mAffineDtmImageToTerrain.scale().x(), mAffineDtmImageToTerrain.scale().y());
            cv::Mat image = mDtmReader->read(w);
            mIniZ = image.at<float>(0, 0);
        } else {
            // ¿Buscar la z media del DTM?, ¿la mas próxima? o ¿ir buscando en las diferentes esquinas de la imagen?
            tl::WindowD w(center_project, mAffineDtmImageToTerrain.scale().x() * mRectImage.width / 2., mAffineDtmImageToTerrain.scale().y() * mRectImage.height / 2.);
            w.normalized();

            if (tl::intersectWindows(mWindowDtmTerrainExtension, w)) {
                cv::Mat image = mDtmReader->read(w);
                cv::Mat mask  = cv::Mat::zeros(image.rows, image.cols, CV_8U);
                mask.setTo(cv::Scalar::all(255), image > -9999.);
                cv::Scalar zmean = cv::mean(image, mask);
                mIniZ = zmean(0);
            } else return; // No intersecta con el DTM
        }

        tl::Rect<int> rect_full(tl::Point<int>(), mDtmReader->cols(), mDtmReader->rows());

        mFootprint.push_back(imageToTerrain(mRectImage.topLeft()));
        mFootprint.push_back(imageToTerrain(mRectImage.topRight()));
        mFootprint.push_back(imageToTerrain(mRectImage.bottomRight()));
        mFootprint.push_back(imageToTerrain(mRectImage.bottomLeft()));

        tl::WindowD window_terrain = mFootprint.window();

        tl::Point<int> window_dtm_image_pt1 = terrainToDTM(window_terrain.pt1);
        tl::Point<int> window_dtm_image_pt2 = terrainToDTM(window_terrain.pt2);

        tl::Rect<int> rect(window_dtm_image_pt1, window_dtm_image_pt2);
        rect.normalized();
        mRectDtm = intersect(rect, rect_full);
        mDtm = mDtmReader->read(mRectDtm);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
    
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
    mUndistortCamera.setFocal((optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.);
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

tl::Point<float> Orthorectification::principalPoint() const
{
    tl::Point<float> principal_point;

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