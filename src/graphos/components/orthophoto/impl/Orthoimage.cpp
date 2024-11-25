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

#include "graphos/components/orthophoto/impl/Orthoimage.h"

#include "graphos/components/orthophoto/impl/ZBuffer.h"
#include "graphos/components/orthophoto/impl/Orthorectification.h"

#include <tidop/core/progress.h>
#include <tidop/graphic/layer.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/geotools/CRSsTools.h>

#ifdef HAVE_OPENCV_CUDAARITHM
#include <opencv2/cudaarithm.hpp>
#endif
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/imgcodecs.hpp>

namespace graphos
{

cv::Mat createBlackPixelMask(const cv::Mat &image, double areaThreshold, bool upper) 
{
    try {
        // Convert the image to grayscale if it's not already
        cv::Mat gray;
        if (image.channels() != 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }

        // Create an initial mask for black pixels (value 0)
        cv::Mat blackMask = (gray == 0);

        // Find connected components to identify areas
        cv::Mat labels, stats, centroids;
        int numComponents = cv::connectedComponentsWithStats(blackMask, labels, stats, centroids);

        // Create a new mask to store the valid black pixels
        cv::Mat finalMask = cv::Mat::zeros(image.size(), CV_8U);

        // Iterate through each component
        for (int i = 0; i < numComponents; i++) { 
            int area = stats.at<int>(i, cv::CC_STAT_AREA);
            //int width = stats.at<int>(i, cv::CC_STAT_WIDTH);
            //int height = stats.at<int>(i, cv::CC_STAT_HEIGHT);

            // Check if the area is less than or equal to 4 and within a 2x2 bounding box
            if ((upper && area > areaThreshold) || area <= areaThreshold /*&& width <= 2 && height <= 2*/) {
                cv::Mat componentMask = (labels == i);
                finalMask |= componentMask;
            }
        }

        return finalMask;

    } catch (std::exception &e) {
        tl::printException(e);
    }
}


Orthoimage::Orthoimage(const tl::Path &image,
                       Orthorectification *orthorectification,
                       //const tl::EcefToEnu &ecefToEnu, 
                       //const std::shared_ptr<tl::CrsTransform> &crsTransfom,
                       const std::string &enuCrs,
                       const std::string &crs,
                       const tl::Rect<int> &rectOrtho,
                       const tl::Affine<double, 2> &georeference,
                       const std::string &interpolation,
                       bool cuda)
  : mImageReader(tl::ImageReaderFactory::create(image)),
    mOrthorectification(orthorectification),
    //mEcefToEnu(ecefToEnu), 
    //mCrsTransfom(crsTransfom),
    mEnuCrs(enuCrs),
    mCrs(crs),
    mRectOrtho(rectOrtho),
    mGeoreference(georeference),
    mInterpolation(interpolation),
    bCuda(cuda)
{
}

Orthoimage::~Orthoimage()
{
}

void Orthoimage::run(const tl::Path &ortho, const cv::Mat &visibilityMap)
{
    TL_TODO("Comprobar que visibilityMap tenga el tamaño adecuado")

        auto convertEnuToProjected = [&](const tl::Point3d &point) -> tl::Point3d 
        {
            //auto point_ecef = mEcefToEnu.inverse(point);
            //auto point_utm = mCrsTransfom->transform(point_ecef);
            tl::GeoTools *geo_tools = tl::GeoTools::getInstance();
            tl::Point3d projected = static_cast<tl::Point3d>(point);
            geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, projected.x, projected.y, projected.z);
            return projected;
        };

        auto convertProjectedToEnu = [&](const tl::Point3d &point) -> tl::Point3d 
        {
            //auto point_ecef = mCrsTransfom->transform(point, tl::CrsTransform::Order::inverse);
            //auto point_projected = mEcefToEnu.direct(point_ecef);
            //return point_projected;
            tl::GeoTools *geo_tools = tl::GeoTools::getInstance();
            tl::Point3d enu = static_cast<tl::Point3d>(point);
            geo_tools->ptrCRSsTools()->crsOperation(mCrs, mEnuCrs, enu.x, enu.y, enu.z);
            return enu;
        };

    try {

        tl::Rect<int> rect_image = mOrthorectification->rectImage();
        tl::Rect<int> rect_dtm = mOrthorectification->rectDtm();

        //// Carga de la imagen

        mImageReader->open();
        if (!mImageReader->isOpen()) throw std::runtime_error("Image open error");
        cv::Mat image = mImageReader->read();
        int depth = mImageReader->depth();

        if (depth != 8) {
#ifdef HAVE_OPENCV_CUDAARITHM
            if (bCuda) {
                cv::cuda::GpuMat gImgIn(image);
                cv::cuda::GpuMat gImgOut;
                cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
                gImgOut.download(image);
            } else {
#endif

                cv::normalize(image, image, 0., 255., cv::NORM_MINMAX, CV_8U);

#ifdef HAVE_OPENCV_CUDAARITHM
            }
#endif

            depth = 8;
        }

        /// Undistort

        cv::Mat undistort_image = mOrthorectification->undistort(image);
        image.release();
        /// georeferencia orto

        mOrthophotoWriter = tl::ImageWriterFactory::create(ortho);
        mOrthophotoWriter->open();
        if (!mOrthophotoWriter->isOpen()) throw std::runtime_error("Image open error");
        int channels_ortho = mImageReader->channels();
        tl::DataType data_type_ortho = tl::DataType::TL_8U;// mImageReader->dataType();

        /// Ortoimagen en coordenadas proyectadas
            
        // ENU coordinates
        auto top_left = mGeoreference.transform(tl::Point<double>(0., 0.));
        auto top_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(mRectOrtho.width), 0.));
        auto bottom_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(mRectOrtho.width), static_cast<double>(mRectOrtho.height)));
        auto bottom_left = mGeoreference.transform(tl::Point<double>(0., static_cast<double>(mRectOrtho.height)));
        tl::Window<tl::Point<double>> window_total_enu(tl::Point<double>(0., 0.), tl::Point<double>(mRectOrtho.height, mRectOrtho.height));

        /// Projected coordinates
        auto top_left_projected = convertEnuToProjected(top_left);
        auto top_right_projected = convertEnuToProjected(top_right);
        auto bottom_right_projected = convertEnuToProjected(bottom_right);
        auto bottom_left_projected = convertEnuToProjected(bottom_left);
        tl::Window<tl::Point<double>> window_projected({top_left_projected, 
                                                        top_right_projected,
                                                        bottom_right_projected,
                                                        bottom_left_projected});
        tl::Affine<double, 2> affine_ortho_projected(mGeoreference.scale().x(), -mGeoreference.scale().y(),
                                                       window_projected.pt1.x,
                                                       window_projected.pt2.y, 0.0);
        
        int out_width = tl::numberCast<int>(window_projected.width() / mGeoreference.scale().x());
        int out_height = tl::numberCast<int>(window_projected.height() / mGeoreference.scale().y());
        tl::Rect<int> ortho_rect(0, 0, out_width, out_height);

        mOrthophotoWriter->create(out_height, out_width, channels_ortho, data_type_ortho);
        cv::Mat mat_ortho(out_height, out_width, CV_MAKETYPE(dataTypeToOpenCVDataType(data_type_ortho), channels_ortho));
        //mOrthophotoWriter->create(mRectOrtho.height, mRectOrtho.width, channels_ortho, data_type_ortho);
        //cv::Mat mat_ortho(mRectOrtho.height, mRectOrtho.width, CV_MAKETYPE(dataTypeToOpenCVDataType(data_type_ortho), channels_ortho));
        mat_ortho = cv::Scalar(0, 0, 0);

        std::vector<tl::Point3<double>> dtm_grid_terrain_points(4);
        std::vector<tl::Point<double>> ortho_image_coordinates(4);
        std::vector<tl::Point<double>> photo_photocoordinates(4);
        std::vector<tl::Point<double>> photo_image_coordinates(4);

        for (int r = rect_dtm.y; r < rect_dtm.y + rect_dtm.height - 1; r++) {
            for (int c = rect_dtm.x; c < rect_dtm.x + rect_dtm.width - 1; c++) {

                if (!visibilityMap.empty() && visibilityMap.at<uchar>(r - rect_dtm.y, c - rect_dtm.x) == 0) continue;

                dtm_grid_terrain_points[0] = mOrthorectification->dtmToTerrain(tl::Point<int>(c, r));
                dtm_grid_terrain_points[1] = mOrthorectification->dtmToTerrain(tl::Point<int>(c + 1, r));
                dtm_grid_terrain_points[2] = mOrthorectification->dtmToTerrain(tl::Point<int>(c + 1, r + 1));
                dtm_grid_terrain_points[3] = mOrthorectification->dtmToTerrain(tl::Point<int>(c, r + 1));

                if (mOrthorectification->hasNodataValue()) {
                    double nodata_value = mOrthorectification->nodataValue();
                    if (dtm_grid_terrain_points[0].z == nodata_value ||
                        dtm_grid_terrain_points[1].z == nodata_value ||
                        dtm_grid_terrain_points[2].z == nodata_value ||
                        dtm_grid_terrain_points[3].z == nodata_value) {
                        continue;
                    }
                }

                auto inverse_transform = mGeoreference.inverse();

                ortho_image_coordinates[0] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[0]));
                ortho_image_coordinates[1] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[1]));
                ortho_image_coordinates[2] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[2]));
                ortho_image_coordinates[3] = inverse_transform.transform(static_cast<tl::Point<double>>(dtm_grid_terrain_points[3]));

                photo_photocoordinates[0] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[0]);
                photo_photocoordinates[1] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[1]);
                photo_photocoordinates[2] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[2]);
                photo_photocoordinates[3] = mOrthorectification->terrainToPhotocoordinates(dtm_grid_terrain_points[3]);

                photo_image_coordinates[0] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[0]);
                photo_image_coordinates[1] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[1]);
                photo_image_coordinates[2] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[2]);
                photo_image_coordinates[3] = mOrthorectification->photocoordinatesToImage(photo_photocoordinates[3]);

                if (rect_image.contains(photo_image_coordinates[0]) &&
                    rect_image.contains(photo_image_coordinates[1]) &&
                    rect_image.contains(photo_image_coordinates[2]) &&
                    rect_image.contains(photo_image_coordinates[3]) &&
                    mRectOrtho.contains(ortho_image_coordinates[0]) &&
                    mRectOrtho.contains(ortho_image_coordinates[1]) &&
                    mRectOrtho.contains(ortho_image_coordinates[2]) &&
                    mRectOrtho.contains(ortho_image_coordinates[3])) {


                    try {


                        
                        /*
                        // Transformación perspectiva entre las coordenadas en la ortoimagen y el fotograma

                        auto window_ortho_in = boundingWindow(ortho_image_coordinates.begin(), ortho_image_coordinates.end());
                        if (!window_ortho_in.isValid()) continue;

                        auto window_image_in = boundingWindow(photo_image_coordinates.begin(), photo_image_coordinates.end());
                        if (!window_image_in.isValid()) continue;

                        cv::Point2f cv_photo_image_coordinates[4];
                        cv::Point2f cv_ortho_image_coordinates[4];
                        for (int i = 0; i < 4; i++) {
                            cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(photo_image_coordinates[i].x),
                                                                        static_cast<float>(photo_image_coordinates[i].y));
                            cv_ortho_image_coordinates[i] = cv::Point2f(static_cast<float>(ortho_image_coordinates[i].x),
                                                                        static_cast<float>(ortho_image_coordinates[i].y));
                        }

                        cv::Mat perspective = cv::getPerspectiveTransform(cv_photo_image_coordinates, cv_ortho_image_coordinates, cv::DECOMP_SVD);


                        // Se calcula una ventana expandida para la orto
                        
                        tl::WindowD window_ortho_extend = tl::expandWindow(window_ortho_in, 1);
                        window_ortho_extend = tl::windowIntersection<tl::WindowD>(mRectOrtho.window(), window_ortho_extend);


                        // Se calcula la ventana expandida en la imagen;


                        std::vector<cv::Point2f> ortho_points = {
                            cv::Point2f(window_ortho_extend.pt1.x, window_ortho_extend.pt1.y),
                            cv::Point2f(window_ortho_extend.pt2.x, window_ortho_extend.pt1.y),
                            cv::Point2f(window_ortho_extend.pt2.x, window_ortho_extend.pt2.y),
                            cv::Point2f(window_ortho_extend.pt1.x, window_ortho_extend.pt2.y)
                        };

                        // Vector para almacenar los puntos transformados
                        std::vector<cv::Point2f> image_points;

                        // Aplica la transformación de perspectiva
                        cv::perspectiveTransform(ortho_points, image_points, perspective.inv());

                        tl::Window<tl::Point<double>> window_image_extend({{image_points.at(0).x, image_points.at(0).y},
                                                                           {image_points.at(1).x, image_points.at(1).y},
                                                                           {image_points.at(2).x, image_points.at(2).y},
                                                                           {image_points.at(3).x, image_points.at(3).y}});


                        cv::Rect roi_image(tl::numberCast<int>(window_image_extend.pt1.x),
                                           tl::numberCast<int>(window_image_extend.pt1.y),
                                           tl::numberCast<int>(window_image_extend.width()),
                                           tl::numberCast<int>(window_image_extend.height()));

                        cv::Rect roi_ortho(tl::numberCast<int>(window_ortho_extend.pt1.x),
                                           tl::numberCast<int>(window_ortho_extend.pt1.y),
                                           tl::numberCast<int>(window_ortho_extend.width()),
                                           tl::numberCast<int>(window_ortho_extend.height()));

                        cv::Size out_size(tl::numberCast<int>(window_ortho_extend.width()), tl::numberCast<int>(window_ortho_extend.height()));

                        /// Calculo una perspectiva sin desplazamiento
                        for (int i = 0; i < 4; i++) {
                            cv_photo_image_coordinates[i] -= cv::Point2f(window_image_in.pt1.x, window_image_in.pt1.y);
                            cv_ortho_image_coordinates[i] -= cv::Point2f(window_ortho_in.pt1.x, window_ortho_in.pt1.y);
                        }

                        cv::Mat h = cv::getPerspectiveTransform(cv_photo_image_coordinates, cv_ortho_image_coordinates);

                        cv::Mat in = undistort_image(roi_image);
                        //cv::Mat out = mat_ortho(roi_ortho);
                        cv::Mat out = cv::Mat::zeros(out_size, undistort_image.type());
                        cv::warpPerspective(in, out, h, out_size, cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);

                        cv::Rect roi(tl::numberCast<int>(window_ortho_in.pt1.x - window_ortho_extend.pt1.x),
                                     tl::numberCast<int>(window_ortho_in.pt1.y - window_ortho_extend.pt1.y),
                                     tl::numberCast<int>(window_ortho_in.width()),
                                     tl::numberCast<int>(window_ortho_in.height()));
                        out(roi).copyTo(mat_ortho.colRange(window_ortho_in.pt1.x, window_ortho_in.pt2.x)
                                                 .rowRange(window_ortho_in.pt1.y, window_ortho_in.pt2.y));
                        */

                        // Se genera la ortoimagen en el CRS de salida
                        
                        // Transformación perspectiva entre las coordenadas en la ortoimagen y el fotograma
                        
                        auto affine_ortho_projected_inverse = affine_ortho_projected.inverse();

                        std::vector<tl::Point<double>> ortho_coordinates_projected(4);
                        ortho_coordinates_projected[0] = convertEnuToProjected(dtm_grid_terrain_points[0]);
                        ortho_coordinates_projected[1] = convertEnuToProjected(dtm_grid_terrain_points[1]);
                        ortho_coordinates_projected[2] = convertEnuToProjected(dtm_grid_terrain_points[2]);
                        ortho_coordinates_projected[3] = convertEnuToProjected(dtm_grid_terrain_points[3]);

                        std::vector<tl::Point<double>> orthoimage_coordinates_projected(4);
                        // Coordenadas imagen
                        orthoimage_coordinates_projected[0] = affine_ortho_projected_inverse.transform(static_cast<tl::Point<double>>(ortho_coordinates_projected[0]));
                        orthoimage_coordinates_projected[1] = affine_ortho_projected_inverse.transform(static_cast<tl::Point<double>>(ortho_coordinates_projected[1]));
                        orthoimage_coordinates_projected[2] = affine_ortho_projected_inverse.transform(static_cast<tl::Point<double>>(ortho_coordinates_projected[2]));
                        orthoimage_coordinates_projected[3] = affine_ortho_projected_inverse.transform(static_cast<tl::Point<double>>(ortho_coordinates_projected[3]));


                        auto window_ortho_in = boundingWindow(orthoimage_coordinates_projected.begin(), orthoimage_coordinates_projected.end());
                        if (!window_ortho_in.isValid()) continue;

                        auto window_image_in = boundingWindow(photo_image_coordinates.begin(), photo_image_coordinates.end());
                        if (!window_image_in.isValid()) continue;

                        cv::Point2f cv_photo_image_coordinates[4];
                        cv::Point2f cv_orthoimage_coordinates[4];
                        for (int i = 0; i < 4; i++) {
                            cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(photo_image_coordinates[i].x),
                                                                        static_cast<float>(photo_image_coordinates[i].y));
                            cv_orthoimage_coordinates[i] = cv::Point2f(static_cast<float>(orthoimage_coordinates_projected[i].x),
                                                                        static_cast<float>(orthoimage_coordinates_projected[i].y));
                        }

                        cv::Mat perspective = cv::getPerspectiveTransform(cv_photo_image_coordinates, cv_orthoimage_coordinates, cv::DECOMP_SVD);

                        // Se calcula una ventana expandida para la orto
                        
                        tl::WindowD window_ortho_extend = tl::expandWindow(window_ortho_in, 2);
                        auto full_window = ortho_rect.window();
                        if (!intersectWindows(ortho_rect.window(), window_ortho_extend)) continue;
                            
                        window_ortho_extend = tl::windowIntersection<tl::WindowD>(ortho_rect.window(), window_ortho_extend);

                        // Se calcula la ventana expandida en la imagen;

                        std::vector<cv::Point2f> ortho_points = {
                            cv::Point2f(window_ortho_extend.pt1.x, window_ortho_extend.pt1.y),
                            cv::Point2f(window_ortho_extend.pt2.x, window_ortho_extend.pt1.y),
                            cv::Point2f(window_ortho_extend.pt2.x, window_ortho_extend.pt2.y),
                            cv::Point2f(window_ortho_extend.pt1.x, window_ortho_extend.pt2.y)
                        };

                        // Vector para almacenar los puntos transformados
                        std::vector<cv::Point2f> image_points;

                        // Aplica la transformación de perspectiva
                        cv::perspectiveTransform(ortho_points, image_points, perspective.inv());

                        tl::Window<tl::Point<double>> window_image_extend({{image_points.at(0).x, image_points.at(0).y},
                                                                           {image_points.at(1).x, image_points.at(1).y},
                                                                           {image_points.at(2).x, image_points.at(2).y},
                                                                           {image_points.at(3).x, image_points.at(3).y}});

                        cv::Rect roi_image(tl::numberCast<int>(window_image_extend.pt1.x),
                                           tl::numberCast<int>(window_image_extend.pt1.y),
                                           tl::numberCast<int>(window_image_extend.width()),
                                           tl::numberCast<int>(window_image_extend.height()));

                        cv::Rect roi_ortho(tl::numberCast<int>(window_ortho_extend.pt1.x),
                                           tl::numberCast<int>(window_ortho_extend.pt1.y),
                                           tl::numberCast<int>(window_ortho_extend.width()),
                                           tl::numberCast<int>(window_ortho_extend.height()));

                        cv::Size out_size(tl::numberCast<int>(window_ortho_extend.width()), tl::numberCast<int>(window_ortho_extend.height()));

                        /// Calculo una perspectiva sin desplazamiento en la imagen transformada
                        for (int i = 0; i < 4; i++) {
                            cv_photo_image_coordinates[i] -= cv::Point2f(window_image_extend.pt1.x, window_image_extend.pt1.y);
                            cv_orthoimage_coordinates[i] -= cv::Point2f(window_ortho_extend.pt1.x, window_ortho_extend.pt1.y);
                        }

                        cv::Mat h = cv::getPerspectiveTransform(cv_photo_image_coordinates, cv_orthoimage_coordinates);

                        //std::vector<cv::Point2f> pointsToTransform;
                        //pointsToTransform.push_back(cv_photo_image_coordinates[0]);
                        //pointsToTransform.push_back(cv_photo_image_coordinates[1]);
                        //pointsToTransform.push_back(cv_photo_image_coordinates[2]);
                        //pointsToTransform.push_back(cv_photo_image_coordinates[3]);
                        //std::vector<cv::Point2f> transformedPoints;
                        //for (int i = 0; i < 4; i++)
                        //    cv::perspectiveTransform(pointsToTransform, transformedPoints, h);

                        cv::Mat in = undistort_image(roi_image);
                        //cv::Mat out = mat_ortho(roi_ortho);
                        cv::Mat out = cv::Mat::zeros(out_size, undistort_image.type());

                        int interpolation = cv::INTER_LINEAR;
                        if (mInterpolation == "Nearest")
                            interpolation = cv::INTER_NEAREST;
                        else 
                            interpolation = cv::INTER_CUBIC;

                        cv::warpPerspective(in, out, h, out_size, interpolation, cv::BORDER_TRANSPARENT);

                        cv::Rect roi_in(tl::numberCast<int>(window_ortho_in.pt1.x - window_ortho_extend.pt1.x),
                                     tl::numberCast<int>(window_ortho_in.pt1.y - window_ortho_extend.pt1.y),
                                     tl::numberCast<int>(window_ortho_in.width()),
                                     tl::numberCast<int>(window_ortho_in.height()));
                        cv::Rect roi_out(tl::numberCast<int>(window_ortho_in.pt1.x),
                                     tl::numberCast<int>(window_ortho_in.pt1.y),
                                     tl::numberCast<int>(window_ortho_in.width()),
                                     tl::numberCast<int>(window_ortho_in.height()));
                        out(roi_in).copyTo(mat_ortho(roi_out));
                        //out(roi).copyTo(mat_ortho.colRange(tl::numberCast<int>(window_ortho_in.pt1.x), tl::numberCast<int>(window_ortho_in.pt2.x))
                        //                         .rowRange(tl::numberCast<int>(window_ortho_in.pt1.y), tl::numberCast<int>(window_ortho_in.pt2.y)));
                    } catch (std::exception &e) {
                        //tl::printException(e);
                    }
                    //if (rect_image.contains(window_aux.pt1) &&
                    //    rect_image.contains(window_aux.pt2)) {
                    //    window_image_in = window_aux;
                    //}

                                        
                    //cv::Size out_size(tl::numberCast<int>(window_ortho_in.width()), tl::numberCast<int>(window_ortho_in.height()));
                    //cv::Mat out = cv::Mat::zeros(out_size, undistort_image.type());


                    //cv::Point2f cv_photo_image_coordinates[4];
                    //cv::Point2f cv_ortho_image_coordinates[4];
                    //for (int i = 0; i < 4; i++) {
                    //    cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(photo_image_coordinates[i].x - window_image_in.pt1.x),
                    //                                                static_cast<float>(photo_image_coordinates[i].y - window_image_in.pt1.y));
                    //    cv_ortho_image_coordinates[i] = cv::Point2f(static_cast<float>(ortho_image_coordinates[i].x - window_ortho_in.pt1.x),
                    //                                                static_cast<float>(ortho_image_coordinates[i].y - window_ortho_in.pt1.y));
                    //}

                    //cv::Mat perspective = cv::getPerspectiveTransform(cv_photo_image_coordinates, cv_ortho_image_coordinates);

                    /// Estoy expandiendo despues de comprobar si se sale de los limites de la imagen... Solucionar cuando refactorice
                    /// Habría que calcular la intersección de window_aux con la ventana imagen total.


                    //undistort_image.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                    //               .rowRange(window_image_in.pt1.y, window_image_in.pt2.y).copyTo(in);

                    //cv::warpPerspective(in, out, h, cv::Size(window_ortho_in.width(), window_ortho_in.height()), cv::INTER_NEAREST | cv::WARP_INVERSE_MAP, cv::BORDER_TRANSPARENT);
                    //cv::Rect roi_enu(tl::numberCast<int>(window_image_in.pt1.x),
                    //                     tl::numberCast<int>(window_image_in.pt1.y),
                    //                     tl::numberCast<int>(window_image_in.width()),
                    //                     tl::numberCast<int>(window_image_in.height()));

                    //cv::Rect roi(tl::numberCast<int>(window_ortho_in.pt1.x),
                    //             tl::numberCast<int>(window_ortho_in.pt1.y),
                    //             tl::numberCast<int>(window_ortho_in.width()),
                    //             tl::numberCast<int>(window_ortho_in.height()));
                    
                    //tl::WindowI window_aux = tl::expandWindow(window_image_in, 2);
                    //if (rect_image.contains(window_aux.pt1) &&
                    //    rect_image.contains(window_aux.pt2)) {
                    //    window_image_in = window_aux;
                    //}
                    //cv::Mat in(window_image_in.height(), window_image_in.width(), undistort_image.type());
                    //undistort_image.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                    //               .rowRange(window_image_in.pt1.y, window_image_in.pt2.y).copyTo(in);
                    //cv::Mat in = undistort_image(roi_enu);
                    //cv::Mat out = mat_ortho(roi);
                    //cv::warpPerspective(in, out, h, out_size, cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
                    //cv::warpPerspective(in, out, h, out_size, cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
                    //out.copyTo(mat_ortho.colRange(window_ortho_in.pt1.x, window_ortho_in.pt2.x)
                    //           .rowRange(window_ortho_in.pt1.y, window_ortho_in.pt2.y));
                }
            }
        }


        // Relleno de pixeles negros
        {
            //cv::Mat blackPixelMask = createBlackPixelMask(mat_ortho);
            //cv::inpaint(mat_ortho, blackPixelMask, mat_ortho, 3, cv::INPAINT_TELEA);
        }


        /// Transformación de la orto al CRS de salida
        
        /// Transformación entre coordenadas ENU y Projectadas


        //auto convertImageToUTM = [&](const cv::Mat &image, 
        //                             tl::Affine<double, 2> &georeference, 
        //                             cv::Mat &orthoimage) 
        //{

        //    // ENU coordinates
        //    auto top_left = mGeoreference.transform(tl::Point<double>(0., 0.));
        //    auto top_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(image.cols), 0.));
        //    auto bottom_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(image.cols), static_cast<double>(image.rows)));
        //    auto bottom_left = mGeoreference.transform(tl::Point<double>(0., static_cast<double>(image.rows)));
 
        //    /// UTM coordinates
        //    auto top_left_utm = convertEnuToUTM(top_left);
        //    auto top_right_utm = convertEnuToUTM(top_right);
        //    auto bottom_right_utm = convertEnuToUTM(bottom_right);
        //    auto bottom_left_utm = convertEnuToUTM(bottom_left);
        //    tl::Window<tl::Point<double>> window_utm({top_left_utm, 
        //                                              top_right_utm,
        //                                              bottom_right_utm,
        //                                              bottom_left_utm});
        //    int out_width = tl::numberCast<int>(window_utm.width() / mGeoreference.scale().x());
        //    int out_height = tl::numberCast<int>(window_utm.height() / mGeoreference.scale().y());
        //    orthoimage = cv::Mat::zeros(out_height, out_width, image.type());

        //    ///TODO: mGeoreference.scale().y() debería venir con el signo adecuado
        //    georeference = tl::Affine<double, 2>(mGeoreference.scale().x(), -mGeoreference.scale().y(), window_utm.pt1.x, window_utm.pt2.y, 0.);

        //    int width = image.cols / 25;
        //    int height = image.rows / 25;

        //    for (int i = 0; i < height - 1; ++i) {
        //        for (int j = 0; j < width - 1; ++j) {
        //            int x = j * 25;
        //            int y = i * 25;
        //            int w = 25;
        //            int h = 25;
        //            if (image.cols < x + w) w = image.cols - x;
        //            if (image.rows < y + h) h = image.rows - y;

        //            cv::Rect roi(x, y, w, h);
        //            cv::Mat enu_image = image(roi);

        //            std::vector<cv::Point2f> srcPoints = {
        //                cv::Point2f(0, 0),
        //                cv::Point2f(w, 0),
        //                cv::Point2f(w, h),
        //                cv::Point2f(0, h)
        //            };

        //            auto top_left_enu = mGeoreference.transform(tl::Point<double>(x, y));
        //            auto top_right_enu = mGeoreference.transform(tl::Point<double>(x + w, y));
        //            auto bottom_right_enu = mGeoreference.transform(tl::Point<double>(x + w, y + h));
        //            auto bottom_left_enu = mGeoreference.transform(tl::Point<double>(x, y + h));

        //            auto top_left_utm = convertEnuToUTM(static_cast<tl::Point3d>(top_left_enu));
        //            auto top_top_right = convertEnuToUTM(static_cast<tl::Point3d>(top_right_enu));
        //            auto bottom_right_utm = convertEnuToUTM(static_cast<tl::Point3d>(bottom_right_enu));
        //            auto bottom_left_utm = convertEnuToUTM(static_cast<tl::Point3d>(bottom_left_enu));

        //            auto georeference_inverse = georeference.inverse();

        //            auto top_left_image = georeference_inverse.transform(static_cast<tl::Point2d>(top_left_utm));
        //            auto top_right_image = georeference_inverse.transform(static_cast<tl::Point2d>(top_top_right));
        //            auto bottom_right_image = georeference_inverse.transform(static_cast<tl::Point2d>(bottom_right_utm));
        //            auto bottom_left_image = georeference_inverse.transform(static_cast<tl::Point2d>(bottom_left_utm));
        //     
        //            tl::Window<tl::Point<double>> window_image_utm({top_left_image, 
        //                                                            top_right_image,
        //                                                            bottom_right_image,
        //                                                            bottom_left_image});

        //            cv::Point2f ini_utm(window_image_utm.pt1.x, window_image_utm.pt1.y);

        //            std::vector<cv::Point2f> dstPoints = {
        //                cv::Point2f(top_left_image.x - ini_utm.x, top_left_image.y - ini_utm.y), 
        //                cv::Point2f(top_right_image.x - ini_utm.x, top_right_image.y - ini_utm.y),
        //                cv::Point2f(bottom_right_image.x - ini_utm.x, bottom_right_image.y - ini_utm.y), 
        //                cv::Point2f(bottom_left_image.x - ini_utm.x, bottom_left_image.y - ini_utm.y)};



        //            cv::Rect roi_utm(tl::numberCast<int>(window_image_utm.pt1.x),
        //                             tl::numberCast<int>(window_image_utm.pt1.y), 
        //                             tl::numberCast<int>(window_image_utm.width()), 
        //                             tl::numberCast<int>(window_image_utm.height()));

        //            cv::Mat perspective_transform = cv::getPerspectiveTransform(srcPoints, dstPoints);
        //            cv::warpPerspective(enu_image, orthoimage(roi_utm), perspective_transform, roi_utm.size(), cv::INTER_NEAREST | cv::WARP_INVERSE_MAP, cv::BORDER_TRANSPARENT);

        //        }
        //    }

        //};

        auto convertImageFromEnuToProjected = [&](const cv::Mat &image, 
                                                  tl::Affine<double, 2> &georeference, 
                                                  cv::Mat &orthoimage) 
        {
            int tile_size = 25;

            // ENU coordinates
            auto top_left = mGeoreference.transform(tl::Point<double>(0., 0.));
            auto top_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(image.cols), 0.));
            auto bottom_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(image.cols), static_cast<double>(image.rows)));
            auto bottom_left = mGeoreference.transform(tl::Point<double>(0., static_cast<double>(image.rows)));
            tl::Window<tl::Point<double>> window_total_enu(tl::Point<double>(0., 0.), tl::Point<double>(image.cols, image.rows));

            /// Projected coordinates
            auto top_left_projected = convertEnuToProjected(top_left);
            auto top_right_projected = convertEnuToProjected(top_right);
            auto bottom_right_projected = convertEnuToProjected(bottom_right);
            auto bottom_left_projected = convertEnuToProjected(bottom_left);
            tl::Window<tl::Point<double>> window_utm({top_left_projected, 
                                                      top_right_projected,
                                                      bottom_right_projected,
                                                      bottom_left_projected});

            int out_width = tl::numberCast<int>(window_utm.width() / mGeoreference.scale().x());
            int out_height = tl::numberCast<int>(window_utm.height() / mGeoreference.scale().y());
            orthoimage = cv::Mat::zeros(out_height, out_width, image.type());

            ///TODO: mGeoreference.scale().y() debería venir con el signo adecuado
            georeference = tl::Affine<double, 2>(mGeoreference.scale().x(), -mGeoreference.scale().y(), window_utm.pt1.x, window_utm.pt2.y, 0.);

            int width = out_width / tile_size;
            int height = out_height / tile_size;

            for (int i = 0; i < height - 1; ++i) {
                for (int j = 0; j < width - 1; ++j) {
                    int x = j * tile_size;
                    int y = i * tile_size;
                    int w = tile_size;
                    int h = tile_size;
                    if (orthoimage.cols < x + w) w = orthoimage.cols - x;
                    if (orthoimage.rows < y + h) h = orthoimage.rows - y;

                    cv::Rect roi(x, y, w, h);
                    //cv::Mat ortoimage_ = orthoimage(roi);

                    std::vector<cv::Point2f> dst_points = {
                        cv::Point2f(0, 0),
                        cv::Point2f(w, 0),
                        cv::Point2f(w, h),
                        cv::Point2f(0, h)
                    };

                    auto top_left_projected = georeference.transform(tl::Point<double>(x, y));
                    auto top_right_projected = georeference.transform(tl::Point<double>(x + w, y));
                    auto bottom_right_projected = georeference.transform(tl::Point<double>(x + w, y + h));
                    auto bottom_left_projected = georeference.transform(tl::Point<double>(x, y + h));

                    auto top_left_enu = convertProjectedToEnu(static_cast<tl::Point3d>(top_left_projected));
                    auto top_right_enu = convertProjectedToEnu(static_cast<tl::Point3d>(top_right_projected));
                    auto bottom_right_enu = convertProjectedToEnu(static_cast<tl::Point3d>(bottom_right_projected));
                    auto bottom_left_enu = convertProjectedToEnu(static_cast<tl::Point3d>(bottom_left_projected));

                    auto georeference_inverse = mGeoreference.inverse();

                    auto top_left_image = georeference_inverse.transform(static_cast<tl::Point2d>(top_left_enu));
                    auto top_right_image = georeference_inverse.transform(static_cast<tl::Point2d>(top_right_enu));
                    auto bottom_right_image = georeference_inverse.transform(static_cast<tl::Point2d>(bottom_right_enu));
                    auto bottom_left_image = georeference_inverse.transform(static_cast<tl::Point2d>(bottom_left_enu));
             
                    tl::Window<tl::Point<double>> window_image_enu({top_left_image, 
                                                                    top_right_image,
                                                                    bottom_right_image,
                                                                    bottom_left_image});

                    /// Cuando no se carga la imagen completa no esta controlado 

                    if (window_total_enu.containsWindow(window_image_enu)) {

                        auto window_to_read = tl::windowIntersection(window_image_enu, window_total_enu);

                        cv::Point2f ini_enu(window_image_enu.pt1.x, window_image_enu.pt1.y);

                        std::vector<cv::Point2f> src_points = {
                            cv::Point2f(top_left_image.x - ini_enu.x, top_left_image.y - ini_enu.y),
                            cv::Point2f(top_right_image.x - ini_enu.x, top_right_image.y - ini_enu.y),
                            cv::Point2f(bottom_right_image.x - ini_enu.x, bottom_right_image.y - ini_enu.y),
                            cv::Point2f(bottom_left_image.x - ini_enu.x, bottom_left_image.y - ini_enu.y)};


                        

                        cv::Rect roi_enu(tl::numberCast<int>(window_to_read.pt1.x),
                                         tl::numberCast<int>(window_to_read.pt1.y),
                                         tl::numberCast<int>(window_to_read.width()),
                                         tl::numberCast<int>(window_to_read.height()));

                        cv::Mat perspective_transform = cv::getPerspectiveTransform(src_points, dst_points, 1);
                        cv::warpPerspective(image(roi_enu), orthoimage(roi), perspective_transform, roi.size(), cv::INTER_NEAREST, cv::BORDER_TRANSPARENT);

                    }

                }
            }

        };
        //tl::Path enu_ortho = ortho;
        //auto enu_ortho_name = ortho.baseName().toString() + "_enu";
        //enu_ortho.replaceBaseName(enu_ortho_name);
        //cv::imwrite(enu_ortho.toString(), mat_ortho);

        //cv::Mat orthoimage;
        //tl::Affine<double, 2> georeference;
        //convertImageFromEnuToProjected(mat_ortho, georeference, orthoimage);

        //mOrthophotoWriter->create(orthoimage.rows, orthoimage.cols, channels_ortho, data_type_ortho);
        tl::Crs crs(mCrs);
        mOrthophotoWriter->setCRS(crs.toWktFormat());
        mOrthophotoWriter->setGeoreference(affine_ortho_projected);
        mOrthophotoWriter->write(mat_ortho);
        mOrthophotoWriter->close();

    } catch (std::exception &e) {
        if (mOrthophotoWriter) mOrthophotoWriter->close();
        tl::Message::error("Orthorectified image fail: {}", ortho.fileName().toString());
        tl::printException(e);
    } catch (...) {
        if (mOrthophotoWriter) mOrthophotoWriter->close();
        tl::Message::error("Orthorectified image fail: {}", ortho.fileName().toString());
        tl::Message::error("Unhandled exception");
    }
}


} // namespace graphos
