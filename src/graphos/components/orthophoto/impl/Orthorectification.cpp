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

#include "graphos/components/orthophoto/impl/Orthorectification.h"

#include "graphos/core/camera/Undistort.h"

#include <tidop/math/geometry/translation.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif

namespace graphos
{

Orthorectification::Orthorectification(const tl::Path &dtm,
                                       CameraPose cameraPose, 
                                       std::shared_ptr<Undistort> &undistort,
                                       double zIni)
  : mDtmPath(dtm),
    mCameraPose(std::move(cameraPose)),
    mUndistort(undistort),
    mIniZ(zIni),
    mNoDataValue(-std::numeric_limits<double>::max()),
    bCuda(false)
{
    init();
}

tl::Point<int> Orthorectification::terrainToImage(const tl::Point3<double> &terrainPoint) const
{
    tl::Point<int> image_coordinates;

    try {

        tl::Point<double> photocoordinates = mDifferentialRectification->backwardProjection(terrainPoint);
        image_coordinates = photoCoordinatesToImageCoordinates(photocoordinates);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return image_coordinates;
}

tl::Point<double> Orthorectification::terrainToPhotoCoordinates(const tl::Point3<double> &terrainPoint) const
{
    tl::Point<double> photocoordinates;

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

        tl::Point<double> photo_coordinates = imageToPhotocoordinates(imageCoordinates);
        terrain_coordinates = photocoordinatesToTerrain(photo_coordinates);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return terrain_coordinates;
}

tl::Point3<double> Orthorectification::photocoordinatesToTerrain(const tl::Point<double> &photocoordinates) const
{
    double z = mIniZ;
    int it = 10;
    tl::Point3<double> terrain_coordinates;

    try {

        terrain_coordinates = mDifferentialRectification->forwardProjection(photocoordinates, z);
        double z2;

        while (it > 0) {

            tl::Point<int> dtm_point = terrainToDTMImageCoordinates(terrain_coordinates);

            if (mRectDtm.contains(dtm_point)) {
                z2 = mDtm.at<float>(dtm_point.y - mRectDtm.y, dtm_point.x - mRectDtm.x);
                if (std::abs(z2 - z) > 0.01 && z2 != mNoDataValue) {
                    terrain_coordinates = mDifferentialRectification->forwardProjection(photocoordinates, z2);
                    z = z2;
                } else {
                    break;
                }
            } else {
                // If the point is outside the DTM rectangle, we stop the iteration
                break;
            }

            it--;
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return terrain_coordinates;
}

tl::Point<double> Orthorectification::imageToPhotocoordinates(const tl::Point<int> &imagePoint) const
{
    return mAffineImageToPhotocoordinates.transform(tl::Point<double>(imagePoint));
}

tl::Point<double> Orthorectification::photoCoordinatesToImageCoordinates(const tl::Point<double> &photocoordinates) const
{
    return mAffinePhotocoordinatesToImage.transform(photocoordinates);
}

tl::Point3<double> Orthorectification::dtmImageCoordinatesToTerrain(const tl::Point<int> &imagePoint) const
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

tl::Point<int> Orthorectification::terrainToDTMImageCoordinates(const tl::Point3<double> &terrainPoint) const
{
    return mAffineTerrainToDtmImage.transform(static_cast<tl::Point<double>>(terrainPoint));
}

double Orthorectification::z(const tl::Point<double> &terrainPoint) const
{
    double z = mNoDataValue;

    try {

        tl::Point<int> dtm_point = terrainToDTMImageCoordinates(terrainPoint);
        if (mRectDtm.contains(dtm_point)) {
            z = mDtm.at<float>(dtm_point.y - mRectDtm.y, dtm_point.x - mRectDtm.x);
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
    return mUndistort->camera();
}

Camera Orthorectification::undistortCamera() const
{
    return mUndistort->undistortCamera();
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

    auto image_to_terrain = [&](const tl::Point<int> &imageCoordinates, tl::ImageReader *dtmReader) -> tl::Point3<double>
    {
        tl::Point3<double> terrain_coordinates;

        try {

            tl::Point<double> photocoordinates = imageToPhotocoordinates(imageCoordinates);

            double z = mIniZ;
            int it = 10;
            double z2;

            terrain_coordinates = mDifferentialRectification->forwardProjection(photocoordinates, z);

            while (it > 0) {

                tl::Point<int> image_point = terrainToDTMImageCoordinates(terrain_coordinates);

                tl::Rect<int> rect_full(tl::Point<int>(), dtmReader->cols(), dtmReader->rows());

                tl::Point<double> pt(terrain_coordinates.x, terrain_coordinates.y);
                if (rect_full.contains(image_point)) {
                    tl::Rect<int> rect(image_point, 1, 1);
                    cv::Mat image = dtmReader->read(rect);
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
    };

    try {

        std::unique_ptr<tl::ImageReader> dtm_reader = tl::ImageReaderFactory::create(mDtmPath);

        dtm_reader->open();

        mAffineDtmImageToTerrain = dtm_reader->georeference();
        mAffineTerrainToDtmImage = mAffineDtmImageToTerrain.inverse();

        tl::Point<float> principal_point = this->principalPoint();

        mAffineImageToPhotocoordinates = tl::Affine<double, 2>(1., -1., -principal_point.x, principal_point.y, 0.);
        mAffinePhotocoordinatesToImage = mAffineImageToPhotocoordinates.inverse();

        mWindowDtmTerrainExtension.pt1.x = mAffineDtmImageToTerrain.translation().x();
        mWindowDtmTerrainExtension.pt1.y = mAffineDtmImageToTerrain.translation().y();
        mWindowDtmTerrainExtension.pt2 = mAffineDtmImageToTerrain.transform(tl::Point<double>(dtm_reader->cols(), dtm_reader->rows()));
        mWindowDtmTerrainExtension.normalized();

        mDifferentialRectification = std::make_unique<tl::DifferentialRectification>(mCameraPose.rotationMatrix(),
                                                                                     mCameraPose.position(),
                                                                                     focal());

        bool exist_nodata = false;
        double nodata_value = dtm_reader->noDataValue(&exist_nodata);
        if (exist_nodata) mNoDataValue = nodata_value;

        mRectImage = tl::Rect<int>(0, 0, camera().width(), camera().height());

        // Se necesita un primera aproximación de mIniZ
        if (mIniZ == 0.) {
            cv::Mat dem = dtm_reader->read(0.1, 0.1);
            cv::Mat mask = cv::Mat::zeros(dem.rows, dem.cols, CV_8U);
            mask.setTo(cv::Scalar::all(255), dem > -9999.);
            cv::Scalar zmean = cv::mean(dem, mask);
            mIniZ = zmean(0);
        }

        tl::Point<double> center_project = image_to_terrain(mRectImage.window().center(), dtm_reader.get());

        // Lo compruebo antes
        if (mWindowDtmTerrainExtension.containsPoint(center_project)) {
            tl::WindowD w(center_project, mAffineDtmImageToTerrain.scale().x(), mAffineDtmImageToTerrain.scale().y());
            cv::Mat image = dtm_reader->read(w);
            mIniZ = image.at<float>(0, 0);
        } /*else {
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
        }*/

        tl::Rect<int> rect_full(tl::Point<int>(), dtm_reader->cols(), dtm_reader->rows());

        mFootprint.push_back(image_to_terrain(mRectImage.topLeft(), dtm_reader.get()));
        mFootprint.push_back(image_to_terrain(mRectImage.topRight(), dtm_reader.get()));
        mFootprint.push_back(image_to_terrain(mRectImage.bottomRight(), dtm_reader.get()));
        mFootprint.push_back(image_to_terrain(mRectImage.bottomLeft(), dtm_reader.get()));

        tl::WindowD window_terrain = mFootprint.window();

        tl::Point<int> window_dtm_image_pt1 = terrainToDTMImageCoordinates(window_terrain.pt1);
        tl::Point<int> window_dtm_image_pt2 = terrainToDTMImageCoordinates(window_terrain.pt2);

        tl::Rect<int> rect(window_dtm_image_pt1, window_dtm_image_pt2);
        rect.normalized();
        mRectDtm = intersect(rect, rect_full);
        mDtm = dtm_reader->read(mRectDtm);
        dtm_reader->close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
    
}

float Orthorectification::focal() const
{
    float focal_x = 1.f;
    float focal_y = 1.f;

    std::shared_ptr<Calibration> calibration = undistortCamera().calibration();

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

    std::shared_ptr<Calibration> calibration = undistortCamera().calibration();

    principal_point.x = static_cast<float>(calibration->parameter(Calibration::Parameters::cx));
    principal_point.y = static_cast<float>(calibration->parameter(Calibration::Parameters::cy));

    return principal_point;
}

cv::Mat Orthorectification::distCoeffs() const
{
    std::shared_ptr<Calibration> calibration = undistortCamera().calibration();

    cv::Mat dist_coeffs = openCvDistortionCoefficients(*calibration);

    return dist_coeffs;
}

void Orthorectification::setCuda(bool active)
{
    bCuda = active;
}

cv::Mat Orthorectification::undistort(const cv::Mat &image)
{
    return mUndistort->undistortImage(image);
}

bool Orthorectification::isValid() const
{
    return mIniZ != mNoDataValue;
}


} // namespace graphos