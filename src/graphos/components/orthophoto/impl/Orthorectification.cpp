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
                                       Camera undistortedCamera,
                                       double zIni)
  : mDsmPath(dtm),
    mCameraPose(std::move(cameraPose)),
    mUndistortedCamera(undistortedCamera),
    mIniZ(zIni),
    mFootprint(4),
    mNoDataValue(-std::numeric_limits<double>::max()),
    bCuda(false)
{
    init();
}

auto Orthorectification::terrainToImage(const tl::Point3<double> &terrainPoint) const -> tl::Point<int>
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

auto Orthorectification::terrainToPhotoCoordinates(const tl::Point3<double> &terrainPoint) const -> tl::Point<double>
{
    tl::Point<double> photocoordinates;

    try {

        photocoordinates = mDifferentialRectification->backwardProjection(terrainPoint);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return photocoordinates;
}

auto Orthorectification::imageToTerrain(const tl::Point<int> &imageCoordinates) const -> tl::Point3<double>
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

auto Orthorectification::photocoordinatesToTerrain(const tl::Point<double> &photocoordinates) const -> tl::Point3<double>
{
    double z = mIniZ;
    int it = 10;
    tl::Point3<double> terrain_coordinates;

    try {

        terrain_coordinates = mDifferentialRectification->forwardProjection(photocoordinates, z);
        double z2;

        while (it > 0) {

            tl::Point<int> dtm_point = terrainToDsmImageCoordinates(terrain_coordinates);

            if (mRectDsm.contains(dtm_point)) {
                z2 = mDsm.at<float>(dtm_point.y - mRectDsm.y, dtm_point.x - mRectDsm.x);
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

auto Orthorectification::imageToPhotocoordinates(const tl::Point<int> &imagePoint) const -> tl::Point<double>
{
    return mAffineImageToPhotocoordinates.transform(tl::Point<double>(imagePoint));
}

auto Orthorectification::photoCoordinatesToImageCoordinates(const tl::Point<double> &photocoordinates) const -> tl::Point<double>
{
    return mAffinePhotocoordinatesToImage.transform(photocoordinates);
}

auto Orthorectification::dsmImageCoordinatesToTerrain(const tl::Point<int> &imagePoint) const -> tl::Point3<double>
{
    tl::Point3<double> dtm_terrain_point;

    try {

        dtm_terrain_point = mAffineDsmImageToTerrain.transform(static_cast<tl::Point<double>>(imagePoint));
        dtm_terrain_point.z = mDsm.at<float>(imagePoint.y - mRectDsm.y, imagePoint.x - mRectDsm.x);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return dtm_terrain_point;
}

auto Orthorectification::terrainToDsmImageCoordinates(const tl::Point3<double> &terrainPoint) const -> tl::Point<int>
{
    return mAffineTerrainToDsmImage.transform(static_cast<tl::Point<double>>(terrainPoint));
}

auto Orthorectification::z(const tl::Point<double> &terrainPoint) const -> double
{
    double z = mNoDataValue;

    try {

        tl::Point<int> dtm_point = terrainToDsmImageCoordinates(terrainPoint);
        if (mRectDsm.contains(dtm_point)) {
            z = mDsm.at<float>(dtm_point.y - mRectDsm.y, dtm_point.x - mRectDsm.x);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return z;
}

auto Orthorectification::rectImage() const -> tl::Rect<int>
{
    return mRectImage;
}

auto Orthorectification::rectDsm() const -> tl::Rect<int>
{
    return mRectDsm;
}

auto Orthorectification::windowDsm() const -> tl::Window<tl::Point<double>>
{
    tl::Window<tl::Point<double>> window;

    window.pt1 = mAffineDsmImageToTerrain.transform(static_cast<tl::Point<double>>(mRectDsm.topLeft()));
    window.pt2 = mAffineDsmImageToTerrain.transform(static_cast<tl::Point<double>>(mRectDsm.bottomRight()));
    window.normalized();
    return window;
}

auto Orthorectification::footprint() const -> tl::GPolygon
{
    return mFootprint;
}

auto Orthorectification::orientation() const -> CameraPose
{
    return mCameraPose;
}

auto Orthorectification::undistortCamera() const -> Camera
{
    return mUndistortedCamera;
}

auto Orthorectification::hasNodataValue() const -> bool
{
    return mNoDataValue != -std::numeric_limits<double>().max();
}

auto Orthorectification::nodataValue() const -> double
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

                tl::Point<int> image_point = terrainToDsmImageCoordinates(terrain_coordinates);

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

        std::unique_ptr<tl::ImageReader> dtm_reader = tl::ImageReaderFactory::create(mDsmPath);

        dtm_reader->open();

        mAffineDsmImageToTerrain = dtm_reader->georeference();
        mAffineTerrainToDsmImage = mAffineDsmImageToTerrain.inverse();

        tl::Point<float> principal_point = this->principalPoint();

        mAffineImageToPhotocoordinates = tl::Affine<double, 2>(1., -1., -principal_point.x, principal_point.y, 0.);
        mAffinePhotocoordinatesToImage = mAffineImageToPhotocoordinates.inverse();

        mWindowDsmTerrainExtension.pt1.x = mAffineDsmImageToTerrain.translation().x();
        mWindowDsmTerrainExtension.pt1.y = mAffineDsmImageToTerrain.translation().y();
        mWindowDsmTerrainExtension.pt2 = mAffineDsmImageToTerrain.transform(tl::Point<double>(dtm_reader->cols(), dtm_reader->rows()));
        mWindowDsmTerrainExtension.normalized();

        mDifferentialRectification = std::make_unique<tl::DifferentialRectification>(mCameraPose.rotationMatrix(),
                                                                                     mCameraPose.position(),
                                                                                     focal());

        bool exist_nodata = false;
        double nodata_value = dtm_reader->noDataValue(&exist_nodata);
        if (exist_nodata) mNoDataValue = nodata_value;

        mRectImage = tl::Rect<int>(0, 0, mUndistortedCamera.width(), mUndistortedCamera.height());

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
        if (mWindowDsmTerrainExtension.containsPoint(center_project)) {
            tl::WindowD w(center_project, mAffineDsmImageToTerrain.scale().x(), mAffineDsmImageToTerrain.scale().y());
            cv::Mat image = dtm_reader->read(w);
            mIniZ = image.at<float>(0, 0);
        } /*else {
            // ¿Buscar la z media del DTM?, ¿la mas próxima? o ¿ir buscando en las diferentes esquinas de la imagen?
            tl::WindowD w(center_project, mAffineDsmImageToTerrain.scale().x() * mRectImage.width / 2., mAffineDsmImageToTerrain.scale().y() * mRectImage.height / 2.);
            w.normalized();

            if (tl::intersectWindows(mWindowDsmTerrainExtension, w)) {
                cv::Mat image = mDtmReader->read(w);
                cv::Mat mask  = cv::Mat::zeros(image.rows, image.cols, CV_8U);
                mask.setTo(cv::Scalar::all(255), image > -9999.);
                cv::Scalar zmean = cv::mean(image, mask);
                mIniZ = zmean(0);
            } else return; // No intersecta con el DTM
        }*/

        tl::Rect<int> rect_full(tl::Point<int>(), dtm_reader->cols(), dtm_reader->rows());

        mFootprint[0] = image_to_terrain(mRectImage.topLeft(), dtm_reader.get());
        mFootprint[1] = image_to_terrain(mRectImage.topRight(), dtm_reader.get());
        mFootprint[2] = image_to_terrain(mRectImage.bottomRight(), dtm_reader.get());
        mFootprint[3] = image_to_terrain(mRectImage.bottomLeft(), dtm_reader.get());

        tl::WindowD window_terrain = mFootprint.window();

        tl::Point<int> window_dtm_image_pt1 = terrainToDsmImageCoordinates(window_terrain.pt1);
        tl::Point<int> window_dtm_image_pt2 = terrainToDsmImageCoordinates(window_terrain.pt2);

        tl::Rect<int> rect(window_dtm_image_pt1, window_dtm_image_pt2);
        rect.normalized();
        mRectDsm = intersect(rect, rect_full);
        mDsm = dtm_reader->read(mRectDsm);
        dtm_reader->close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
    
}

auto Orthorectification::focal() const -> float
{
    float focal_x = 1.f;
    float focal_y = 1.f;

    std::shared_ptr<Calibration> calibration = mUndistortedCamera.calibration();

    if (calibration->existParameter(Calibration::Parameters::focal)) {
        focal_x = static_cast<float>(calibration->parameter(Calibration::Parameters::focal));
        focal_y = static_cast<float>(calibration->parameter(Calibration::Parameters::focal));
    } else {
        focal_x = static_cast<float>(calibration->parameter(Calibration::Parameters::focalx));
        focal_y = static_cast<float>(calibration->parameter(Calibration::Parameters::focaly));
    }

    return (focal_x + focal_y) / 2.f;
}

auto Orthorectification::principalPoint() const -> tl::Point<float>
{
    tl::Point<float> principal_point;

    std::shared_ptr<Calibration> calibration = mUndistortedCamera.calibration();

    principal_point.x = static_cast<float>(calibration->parameter(Calibration::Parameters::cx));
    principal_point.y = static_cast<float>(calibration->parameter(Calibration::Parameters::cy));

    return principal_point;
}

void Orthorectification::setCuda(bool active)
{
    bCuda = active;
}

auto Orthorectification::isValid() const -> bool
{
    return mIniZ != mNoDataValue;
}


} // namespace graphos