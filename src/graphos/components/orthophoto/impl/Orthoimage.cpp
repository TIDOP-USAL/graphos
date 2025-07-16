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
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
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
//cv::Mat createBlackPixelMask(const cv::Mat &image, double areaThreshold, bool upper)
//{
//    try {
//        cv::Mat gray;
//        if (image.channels() != 1)
//            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
//        else
//            gray = image.clone();
//
//        cv::Mat blackMask = (gray == 0);
//        cv::Mat labels, stats, centroids;
//        int numComponents = cv::connectedComponentsWithStats(blackMask, labels, stats, centroids);
//
//        cv::Mat finalMask = cv::Mat::zeros(image.size(), CV_8U);
//        for (int i = 0; i < numComponents; i++) {
//            int area = stats.at<int>(i, cv::CC_STAT_AREA);
//            if ((upper && area > areaThreshold) || area <= areaThreshold) {
//                finalMask |= (labels == i);
//            }
//        }
//
//        // Morfología para suavizar la máscara
//        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
//        cv::dilate(finalMask, finalMask, kernel);
//        cv::erode(finalMask, finalMask, kernel);
//
//        return finalMask;
//    } catch (std::exception &e) {
//        tl::printException(e);
//    }
//}

Orthoimage::Orthoimage(const tl::Path &image,
                       Orthorectification *orthorectification,
                       const std::string &enuCrs,
                       const std::string &crs,
                       const tl::Rect<int> &rectOrtho,
                       const tl::Affine<double, 2> &georeference,
                       const std::string &interpolation,
                       bool cuda)
  : mImage(image),
    mOrthorectification(orthorectification),
    mEnuCrs(enuCrs),
    mCrs(crs),
    mRectOrtho(rectOrtho),
    mGeoreference(georeference),
    mInterpolation(interpolation),
    bCuda(cuda),
    mReadWithOpenCV(true)
{
}

Orthoimage::~Orthoimage()
{
}

void Orthoimage::run(const tl::Path &ortho, const cv::Mat &visibilityMap)
{
    TL_TODO("Comprobar que visibilityMap tenga el tamaño adecuado")

    tl::ChronoAuto chrono("Orthoimage");

    tl::GeoTools *geo_tools = tl::GeoTools::getInstance();

    auto convertEnuToProjected = [&](const tl::Point3d &point) -> tl::Point3d 
    {
        tl::Point3d projected = point;
        geo_tools->ptrCRSsTools()->crsOperation(mEnuCrs, mCrs, projected.x, projected.y, projected.z);
        return projected;
    };

    auto convertProjectedToEnu = [&](const tl::Point3d &point) -> tl::Point3d 
    {
        tl::Point3d enu = point;
        geo_tools->ptrCRSsTools()->crsOperation(mCrs, mEnuCrs, enu.x, enu.y, enu.z);
        return enu;
    };

    try {

        tl::Rect<int> rect_image = mOrthorectification->rectImage();
        tl::Rect<int> rect_dtm = mOrthorectification->rectDtm();

        //tl::Chrono chrono_grid("Read image");
        //chrono_grid.run();

        //// Carga de la imagen
        cv::Mat image = readImage();

        //chrono_grid.stop();

        //chrono_grid.reset();
        //chrono_grid.setMessage("Undistort image");
        //chrono_grid.run();

        /// Undistort
        //Esto no tiene que estar en Orthorectification
        cv::Mat undistort_image = mOrthorectification->undistort(image);
        image.release();

        /// georeferencia orto

        auto orthophoto_writer = tl::ImageWriterFactory::create(ortho);
        orthophoto_writer->open();
        if (!orthophoto_writer->isOpen()) throw std::runtime_error("Image open error");
        int channels_ortho = image.channels();
        tl::DataType data_type_ortho = tl::DataType::TL_8U;// mImageReader->dataType();

        /// Ortoimagen en coordenadas proyectadas
            
        // ENU coordinates
        auto top_left = mGeoreference.transform(tl::Point<double>(0., 0.));
        auto top_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(mRectOrtho.width), 0.));
        auto bottom_right = mGeoreference.transform(tl::Point<double>(static_cast<double>(mRectOrtho.width), static_cast<double>(mRectOrtho.height)));
        auto bottom_left = mGeoreference.transform(tl::Point<double>(0., static_cast<double>(mRectOrtho.height)));
        tl::Window<tl::Point<double>> window_total_enu(tl::Point<double>(0., 0.), tl::Point<double>(mRectOrtho.width, mRectOrtho.height));

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

        orthophoto_writer->create(mRectOrtho.height, mRectOrtho.width, channels_ortho, data_type_ortho);

        cv::Mat map_x(mRectOrtho.height, mRectOrtho.width, CV_32F, cv::Scalar(-1));
        cv::Mat map_y(mRectOrtho.height, mRectOrtho.width, CV_32F, cv::Scalar(-1));

        //chrono_grid.stop();

        //chrono_grid.reset();
        //chrono_grid.setMessage("Grid iteration");
        //chrono_grid.run();

        // Iteración sobre la cuadrícula de la ortofoto
        tl::parallel_for(0, mRectOrtho.height, [&](size_t r) {
        //for(int r = 0; r < out_height; ++r){
            for (int c = 0; c < mRectOrtho.width; c++) {

                // Coordenadas de la ortofoto en el grid
                tl::Point<double> ortho_grid_coord(c, r);

                // Se pasan a coordenadas terreno
                auto terrain_coordinates = affine_ortho_projected.transform(ortho_grid_coord);

                // Coordenadas de la ortofoto en el sistema de terreno
                tl::Point3<double> enu_coordinates = convertProjectedToEnu(terrain_coordinates);

                auto dtm_pixel_coordinates = mOrthorectification->terrainToDTMImageCoordinates(enu_coordinates);

                auto y = dtm_pixel_coordinates.y - rect_dtm.y;
                auto x = dtm_pixel_coordinates.x - rect_dtm.x;
                if (x < 0 || y < 0 || x >= visibilityMap.cols || y >= visibilityMap.rows) continue;  // visibilityMap tiene el tamaño del trozo del DTM cargado con lo cual esto debería ser redundante con -> if (!rect_dtm.contains(check)) continue;
                if (!visibilityMap.empty() && visibilityMap.at<uchar>(y, x) == 0) continue;

                enu_coordinates.z = mOrthorectification->z(enu_coordinates);
                if (mOrthorectification->nodataValue() == enu_coordinates.z) continue;

                // Coordenadas fotogrametricas
                tl::Point<double> photo_photocoord = mOrthorectification->terrainToPhotoCoordinates(enu_coordinates);
                // Coordenadas de la imagen
                tl::Point<double> photo_image_coord = mOrthorectification->photoCoordinatesToImageCoordinates(photo_photocoord);

                if (rect_image.contains(photo_image_coord)) {
                    map_x.at<float>(static_cast<int>(r), c) = static_cast<float>(photo_image_coord.x);
                    map_y.at<float>(static_cast<int>(r), c) = static_cast<float>(photo_image_coord.y);
                }
            }
        });

        //chrono_grid.stop();

        //chrono_grid.reset();
        //chrono_grid.setMessage("Remap");
        //chrono_grid.run();

        int interpolation = cv::INTER_NEAREST; // Default to Nearest
        if (mInterpolation == "Bilinear") {
            interpolation = cv::INTER_LINEAR;
        } else if (mInterpolation == "Bicubic") {
            interpolation = cv::INTER_CUBIC;
        }

#ifdef HAVE_OPENCV_CUDAARITHM
        cv::cuda::GpuMat g_map_x(map_x), g_map_y(map_y), g_output;
        cv::cuda::GpuMat g_undistort(undistort_image);
        cv::cuda::remap(g_undistort, g_output, g_map_x, g_map_y, interpolation, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        cv::Mat ortho_image;
        g_output.download(ortho_image);
#else
        cv::Mat ortho_image;
        cv::remap(undistort_image, ortho_image, map_x, map_y, interpolation, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
#endif

        //chrono_grid.stop();

        //chrono_grid.reset();
        //chrono_grid.setMessage("Mask orthoimage");
        //chrono_grid.run();

        /// Mascara
        cv::Mat gray;
        if (ortho_image.channels() == 1)
            gray = ortho_image;
        else
            cv::cvtColor(ortho_image, gray, cv::COLOR_BGR2GRAY);
        cv::Mat mask(ortho_image.size(), CV_8U);
        mask.setTo(cv::Scalar::all(0));
        mask.setTo(cv::Scalar::all(255), gray > 0);

        //cv::Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT,
        //                                        cv::Size(2 * 2 + 1, 2 * 2 + 1),
        //                                        cv::Point(2, 2));
        cv::Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(3, 3));

        // Apertura
        cv::erode(mask, mask, element);
        cv::dilate(mask, mask, element);

        //cv::erode(mask, mask, element);

        //chrono_grid.stop();

        //chrono_grid.reset();
        //chrono_grid.setMessage("Apply mask");
        //chrono_grid.run();

        cv::Mat ortho_with_mask = cv::Mat::zeros(ortho_image.size(), ortho_image.type());
        ortho_image.copyTo(ortho_with_mask, mask);

        //chrono_grid.stop();

        //chrono_grid.reset();
        //chrono_grid.setMessage("Write ortoimage");
        //chrono_grid.run();

        tl::Crs crs(mCrs);
        orthophoto_writer->setCRS(crs.toWktFormat());
        orthophoto_writer->setGeoreference(affine_ortho_projected);
        orthophoto_writer->write(ortho_with_mask);
        orthophoto_writer->close();
        
        //chrono_grid.stop();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Error generating the orthoimage: {}", ortho.fileName().toString());
    }
}

cv::Mat Orthoimage::readImage() 
{
    cv::Mat mat;

    //if (mReadWithOpenCV) {
    //
    //    mat = cv::imread(mImage.toString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_COLOR | cv::IMREAD_ANYDEPTH);
    //   
    //    if (mat.empty()) {
    //        mReadWithOpenCV = false;
    //    }
    //}

    //if (!mReadWithOpenCV) {

        auto image_reader = tl::ImageReaderFactory::create(mImage);
        image_reader->open();
        if (!image_reader->isOpen()) throw std::runtime_error("Image open error");

        mat = image_reader->read();
        image_reader->close();
    //}

    normalizeImage(mat);

    return mat;
}

void Orthoimage::normalizeImage(cv::Mat &mat) const
{
    if (mat.depth() != CV_8U) {
#ifdef HAVE_OPENCV_CUDAARITHM
        if (bCuda) {
            cv::cuda::GpuMat gImgIn(mat);
            cv::cuda::GpuMat gImgOut;
            cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
            gImgOut.download(mat);
        } else {
#endif

            cv::normalize(mat, mat, 0., 255., cv::NORM_MINMAX, CV_8U);

#ifdef HAVE_OPENCV_CUDAARITHM
        }
#endif
    }
}


} // namespace graphos
