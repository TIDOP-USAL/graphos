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
        mat_ortho = cv::Scalar(0, 0, 0);

        //for (int r = rect_dtm.y; r < rect_dtm.y + rect_dtm.height - 1; r++) {
        tl::parallel_for(rect_dtm.y, rect_dtm.y + rect_dtm.height - 1, [&](size_t r) {

            std::vector<tl::Point3<double>> dtm_grid_terrain_points(4);
            std::vector<tl::Point<double>> ortho_image_coordinates(4);
            std::vector<tl::Point<double>> photo_photocoordinates(4);
            std::vector<tl::Point<double>> photo_image_coordinates(4);

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

                        //cv::Rect roi_ortho(tl::numberCast<int>(window_ortho_extend.pt1.x),
                        //                   tl::numberCast<int>(window_ortho_extend.pt1.y),
                        //                   tl::numberCast<int>(window_ortho_extend.width()),
                        //                   tl::numberCast<int>(window_ortho_extend.height()));

                        cv::Size out_size(tl::numberCast<int>(window_ortho_extend.width()), tl::numberCast<int>(window_ortho_extend.height()));

                        /// Calculo una perspectiva sin desplazamiento en la imagen transformada
                        for (int i = 0; i < 4; i++) {
                            cv_photo_image_coordinates[i] -= cv::Point2f(window_image_extend.pt1.x, window_image_extend.pt1.y);
                            cv_orthoimage_coordinates[i] -= cv::Point2f(window_ortho_extend.pt1.x, window_ortho_extend.pt1.y);
                        }

                        cv::Mat h = cv::getPerspectiveTransform(cv_photo_image_coordinates, cv_orthoimage_coordinates);

                        cv::Mat in = undistort_image(roi_image);
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

                    } catch (std::exception &e) {
                        //tl::printException(e);
                    }
                }
            }
        });

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
