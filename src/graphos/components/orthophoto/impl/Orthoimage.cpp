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

#include <tidop/core/task/Progress.h>
#include <tidop/graphic/layer.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/geotools/CRSsTools.h>

#ifdef HAVE_OPENCV_CUDAARITHM
#include <opencv2/cudaarithm.hpp>
#endif
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif
#ifdef HAVE_OPENCV_CUDAIMGPROC
#include <opencv2/cudaimgproc.hpp>
#endif // HAVE_OPENCV_CUDAIMGPROC
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
                       const std::string &enuCrs,
                       const std::string &crs,
                       const tl::Size<int> &sizeOrtho,
                       const tl::Affine<double, 2> &orthoGeoreference,
                       const std::string &interpolation,
                       bool cuda)
  : mImage(image),
    mOrthorectification(orthorectification),
    mEnuCrs(enuCrs),
    mCrs(crs),
    mSizeOrtho(sizeOrtho),
    mGeoreference(orthoGeoreference),
    mInterpolation(interpolation),
    bCuda(cuda),
    mReadWithOpenCV(true),
    mDataType(tl::DataType::TL_8U)
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
        tl::Rect<int> rect_dtm = mOrthorectification->rectDsm();

        cv::Mat undistort_image = readImage();

        /// georeferencia orto

        auto orthophoto_writer = tl::ImageWriterFactory::create(ortho);
        orthophoto_writer->open();
        if (!orthophoto_writer->isOpen()) throw std::runtime_error("Image open error");
        int channels_ortho = undistort_image.channels();
        tl::DataType data_type_ortho = mDataType;

        /// Ortoimagen en coordenadas proyectadas
            
        //// ENU coordinates
        //auto top_left = mEnuGeoreference.transform(tl::Point<double>(0., 0.));
        //auto top_right = mEnuGeoreference.transform(tl::Point<double>(static_cast<double>(mSizeEnuOrtho.width), 0.));
        //auto bottom_right = mEnuGeoreference.transform(tl::Point<double>(static_cast<double>(mSizeEnuOrtho.width), static_cast<double>(mSizeEnuOrtho.height)));
        //auto bottom_left = mEnuGeoreference.transform(tl::Point<double>(0., static_cast<double>(mSizeEnuOrtho.height)));
        //tl::Window<tl::Point<double>> window_total_enu(tl::Point<double>(0., 0.), tl::Point<double>(mSizeEnuOrtho.width, mSizeEnuOrtho.height));

        ///// Projected coordinates
        //auto top_left_projected = convertEnuToProjected(top_left);
        //auto top_right_projected = convertEnuToProjected(top_right);
        //auto bottom_right_projected = convertEnuToProjected(bottom_right);
        //auto bottom_left_projected = convertEnuToProjected(bottom_left);
        //tl::Window<tl::Point<double>> window_projected({top_left_projected, 
        //                                                top_right_projected,
        //                                                bottom_right_projected,
        //                                                bottom_left_projected});

        // Projected coordinates
        // ¿No tiene mas sentido pasar la ventana directamente?
        auto top_left_projected = mGeoreference.transform(tl::Point<double>(0., 0.));
        auto top_right_projected = mGeoreference.transform(tl::Point<double>(static_cast<double>(mSizeOrtho.width), 0.));
        auto bottom_right_projected = mGeoreference.transform(tl::Point<double>(static_cast<double>(mSizeOrtho.width), static_cast<double>(mSizeOrtho.height)));
        auto bottom_left_projected = mGeoreference.transform(tl::Point<double>(0., static_cast<double>(mSizeOrtho.height)));
        tl::Window<tl::Point<double>> window_projected({top_left_projected, 
                                                       top_right_projected,
                                                       bottom_right_projected,
                                                       bottom_left_projected});

        //tl::Affine<double, 2> affine_ortho_projected(mEnuGeoreference.scale().x(), -mEnuGeoreference.scale().y(),
        //                                             window_projected.pt1.x,
        //                                             window_projected.pt2.y, 0.0);

        //cv::Size orto_size(window_projected.width() / mEnuGeoreference.scale().x(), window_projected.height() / mEnuGeoreference.scale().y());
        cv::Size orto_size(mSizeOrtho.width, mSizeOrtho.height);
        //tl::Message::warning("orto size ENU [{},{}]", mSizeEnuOrtho.height, mSizeEnuOrtho.width);
        orthophoto_writer->create(mSizeOrtho.height, mSizeOrtho.width, channels_ortho, data_type_ortho);

        cv::Mat map_x(orto_size, CV_32F, cv::Scalar(-1));
        cv::Mat map_y(orto_size, CV_32F, cv::Scalar(-1));

        //tl::Affine<double, 2> affine_projected_to_enu;
        int step = 128;
        int nx = 0, ny = 0;
        std::vector<double> enuMeshX, enuMeshY;

        tl::Crs crs(mCrs);

        //if (crs.isProjected()) {

        //    std::vector<tl::Point<double>> enu_pts = {
        //        top_left, top_right, bottom_right, bottom_left
        //    };

        //    std::vector<tl::Point<double>> proj_pts = {
        //        static_cast<tl::Point<double>>(top_left_projected),
        //        static_cast<tl::Point<double>>(top_right_projected),
        //        static_cast<tl::Point<double>>(bottom_right_projected),
        //        static_cast<tl::Point<double>>(bottom_left_projected)
        //    };

        //    affine_projected_to_enu = tl::Affine2DEstimator<double>::estimate(proj_pts, enu_pts);

        //} else {

            // Precompute mesh ENU from projected coords with PROJ
            nx = (orto_size.width + step - 1) / step + 1;
            ny = (orto_size.height + step - 1) / step + 1;
            enuMeshX.resize(nx * ny);
            enuMeshY.resize(nx * ny);

            auto idx = [&](int ix, int iy) { return iy * nx + ix; };

            for (int iy = 0; iy < ny; ++iy) {
                int r = std::min(iy * step, orto_size.height);
                for (int ix = 0; ix < nx; ++ix) {
                    int c = std::min(ix * step, orto_size.width);
                    auto proj = mGeoreference.transform({(double)c,(double)r});
                    auto enu3 = convertProjectedToEnu(tl::Point3d(proj.x, proj.y, 0.0));
                    enuMeshX[idx(ix, iy)] = enu3.x;
                    enuMeshY[idx(ix, iy)] = enu3.y;
                }
            }
        //}



        // Iteración sobre la cuadrícula de la ortofoto
        tl::parallel_for(0, orto_size.height, [&](size_t r) {
        //for(int r = 0; r < out_height; ++r){
            for (int c = 0; c < orto_size.width; c++) {

                // Coordenadas de la ortofoto en el grid
                tl::Point<double> ortho_grid_coord(c, r);

                // Se pasan a coordenadas terreno
                auto terrain_coordinates = mGeoreference.transform(ortho_grid_coord);

                // Coordenadas de la ortofoto en el sistema de terreno
                //tl::Point3d enu_coordinates = convertProjectedToEnu(terrain_coordinates);
                tl::Point3d enu_coordinates;
                //if (crs.isProjected()) {
                //    enu_coordinates = affine_projected_to_enu.transform(terrain_coordinates);
                //} else {
                    int ix = std::min(c / step, nx - 2);
                    int iy = std::min(static_cast<int>(r) / step, ny - 2);
                    double sx = (c - ix * step) / static_cast<double>(step);
                    double sy = (r - iy * step) / static_cast<double>(step);

                    auto idx = [&](int ix, int iy) { return iy * nx + ix; };
                    double ex00 = enuMeshX[idx(ix, iy)], ey00 = enuMeshY[idx(ix, iy)];
                    double ex10 = enuMeshX[idx(ix + 1, iy)], ey10 = enuMeshY[idx(ix + 1, iy)];
                    double ex01 = enuMeshX[idx(ix, iy + 1)], ey01 = enuMeshY[idx(ix, iy + 1)];
                    double ex11 = enuMeshX[idx(ix + 1, iy + 1)], ey11 = enuMeshY[idx(ix + 1, iy + 1)];

                    double ex0 = ex00 * (1 - sx) + ex10 * sx;
                    double ey0 = ey00 * (1 - sx) + ey10 * sx;
                    double ex1 = ex01 * (1 - sx) + ex11 * sx;
                    double ey1 = ey01 * (1 - sx) + ey11 * sx;

                    double ex = ex0 * (1 - sy) + ex1 * sy;
                    double ey = ey0 * (1 - sy) + ey1 * sy;

                    enu_coordinates = {ex, ey, 0.0};
                //}

                auto dtm_pixel_coordinates = mOrthorectification->terrainToDsmImageCoordinates(enu_coordinates);

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

        int interpolation = cv::INTER_NEAREST; // Default to Nearest
        //if (undistort_image.depth() == CV_8U) {
            if (mInterpolation == "Bilinear") {
                interpolation = cv::INTER_LINEAR;
            } else if (mInterpolation == "Bicubic") {
                interpolation = cv::INTER_CUBIC;
            }
        //}

        cv::Mat orthoimage;
        cv::Mat mask;

        cv::Scalar no_data = 0;
        if (data_type_ortho == tl::DataType::TL_32F || data_type_ortho == tl::DataType::TL_64F) {
            no_data = tl::NoData<float>;
            orthophoto_writer->setNoDataValue(tl::NoData<float>);
        }

//#if defined HAVE_OPENCV_CUDAARITHM && defined HAVE_OPENCV_CUDAIMGPROC
//        if (bCuda) {
//
//            cv::cuda::GpuMat g_map_x(map_x);
//            cv::cuda::GpuMat g_map_y(map_y);
//            cv::cuda::GpuMat g_orthoimage;
//            cv::cuda::GpuMat g_undistort(undistort_image);
//
//            undistort_image.release();
//            map_x.release();
//            map_y.release();
//
//            cv::cuda::remap(g_undistort, g_orthoimage, g_map_x, g_map_y, interpolation, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
//            g_orthoimage.download(orthoimage);
//
//            g_map_x.release();
//            g_map_y.release();
//            g_undistort.release();
//
//            cv::cuda::GpuMat g_orthoimage_gray;
//            if (g_orthoimage.channels() == 1)
//                g_orthoimage_gray = g_orthoimage;
//            else
//                cv::cuda::cvtColor(g_orthoimage, g_orthoimage_gray, cv::COLOR_BGR2GRAY);
//
//            g_orthoimage.release();
//
//            cv::cuda::GpuMat g_mask;
//            cv::cuda::inRange(g_orthoimage_gray, cv::Scalar::all(0), cv::Scalar::all(0), g_mask);
//            g_orthoimage_gray.release();
//            cv::cuda::bitwise_not(g_mask, g_mask);
//            g_mask.download(mask);
//
//        } else {
//#endif

            cv::remap(undistort_image, orthoimage, map_x, map_y, interpolation, cv::BORDER_CONSTANT, no_data);
            undistort_image.release();
            map_x.release();
            map_y.release();

            cv::Mat orthoimage_gray;
            if (orthoimage.channels() == 1)
                orthoimage_gray = orthoimage;
            else
                cv::cvtColor(orthoimage, orthoimage_gray, cv::COLOR_BGR2GRAY);

            cv::inRange(orthoimage_gray, cv::Scalar::all(0), cv::Scalar::all(0), mask);
            cv::bitwise_not(mask, mask);

//#ifdef HAVE_OPENCV_CUDAARITHM
//        }
//#endif

        cv::Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(3, 3));

        // Apertura
        cv::erode(mask, mask, element);
        cv::dilate(mask, mask, element);

        cv::Mat ortho_with_mask = cv::Mat::zeros(orthoimage.size(), orthoimage.type());
        orthoimage.copyTo(ortho_with_mask, mask);
            
        orthophoto_writer->setCRS(crs.toWktFormat());
        orthophoto_writer->setGeoreference(mGeoreference);
        orthophoto_writer->write(ortho_with_mask);
        orthophoto_writer->close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Error generating the orthoimage: {}", ortho.fileName().toString());
    }
}

auto Orthoimage::readImage() -> cv::Mat
{
    cv::Mat mat;

    try {

        auto image_reader = tl::ImageReaderFactory::create(mImage);
        image_reader->open();
        if (!image_reader->isOpen()) throw std::runtime_error("Image open error");

        mat = image_reader->read();

        mDataType = image_reader->dataType();
        image_reader->close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Error reading image: {}", mImage.fileName().toString());
    }

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
