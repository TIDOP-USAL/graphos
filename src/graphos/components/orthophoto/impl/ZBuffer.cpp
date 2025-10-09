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

#include "graphos/components/orthophoto/impl/ZBuffer.h"

#include <opencv2/imgproc.hpp>

#include <tidop/core/chrono.h>

namespace graphos
{


ZBuffer::ZBuffer(Orthorectification *orthorectification,
                 const tl::Size<int> &sizeOrtho,
                 const tl::Affine<double, 2> &georeference)
  : mOrthorectification(orthorectification),
    mRectOrtho(0, 0, sizeOrtho.width, sizeOrtho.height),
    mEnuGeoreference(georeference)
{
    tl::Rect<int> rect_image = mOrthorectification->rectImage();

    mDistances = cv::Mat::ones(rect_image.height, rect_image.width, CV_32F) * FLT_MAX;
    mY = cv::Mat(rect_image.height, rect_image.width, CV_32S, -1);
    mX = cv::Mat(rect_image.height, rect_image.width, CV_32S, -1);
}

ZBuffer::~ZBuffer()
{
}

std::vector<tl::Point<int>> spiralOrder(const tl::Rect<int> &rect)
{
    int w = rect.width;
    int h = rect.height;
    int cx = rect.x + w / 2;
    int cy = rect.y + h / 2;

    std::vector<tl::Point<int>> out;
    out.reserve(static_cast<size_t>(w) * h);

    int x = cx;
    int y = cy;
    out.emplace_back(x, y);

    // pasos a tomar en cada dirección: right, down, left, up
    for (int layer = 1; layer <= std::max(w, h); ++layer) {
        // move right 1
        x += 1;
        if (x >= rect.x && x < rect.x + w && y >= rect.y && y < rect.y + h) out.emplace_back(x, y);

        // then down (2*layer -1) steps
        for (int step = 0; step < (2 * layer - 1); ++step) {
            y += 1;
            if (x >= rect.x && x < rect.x + w && y >= rect.y && y < rect.y + h) out.emplace_back(x, y);
        }
        // left (2*layer) steps
        for (int step = 0; step < (2 * layer); ++step) {
            x -= 1;
            if (x >= rect.x && x < rect.x + w && y >= rect.y && y < rect.y + h) out.emplace_back(x, y);
        }
        // up (2*layer) steps
        for (int step = 0; step < (2 * layer); ++step) {
            y -= 1;
            if (x >= rect.x && x < rect.x + w && y >= rect.y && y < rect.y + h) out.emplace_back(x, y);
        }
        // right (2*layer) steps
        for (int step = 0; step < (2 * layer); ++step) {
            x += 1;
            if (x >= rect.x && x < rect.x + w && y >= rect.y && y < rect.y + h) out.emplace_back(x, y);
        }
        // finish when we've covered all pixels
        if (static_cast<size_t>(out.size()) >= static_cast<size_t>(w) * static_cast<size_t>(h)) break;
    }

    // In case some pixels not added (due to even sizes / center choice), add remaining by scanning.
    if (out.size() < static_cast<size_t>(w) * static_cast<size_t>(h)) {
        for (int r = rect.y; r < rect.y + rect.height; ++r) {
            for (int c = rect.x; c < rect.x + rect.width; ++c) {
                out.emplace_back(c, r);
            }
        }
    }

    return out;
}

void ZBuffer::run()
{
    try {

        tl::ChronoAuto chrono("ZBuffer");

        auto gsd = mEnuGeoreference.scale().x();

        tl::Rect<int> rect_image = mOrthorectification->rectImage();
        tl::Rect<int> rect_dtm = mOrthorectification->rectDsm();

        // generar recorrido en espiral por el DTM
        tl::Rect<int> iterRect(rect_dtm.x, rect_dtm.y, rect_dtm.width - 1, rect_dtm.height - 1);
        std::vector<tl::Point<int>> cells = spiralOrder(iterRect);

        auto inverse_transform = mEnuGeoreference.inverse();

        tl::parallel_for(size_t(0), cells.size(), [&](size_t idx){

            int c = cells[idx].x;
            int r = cells[idx].y;

            if (c < rect_dtm.x || (c + 1) >= (rect_dtm.x + rect_dtm.width) ||
                r < rect_dtm.y || (r + 1) >= (rect_dtm.y + rect_dtm.height)) {
                return;
            }

            std::vector<tl::Point3<double>> dem_terrain_points(4);

            dem_terrain_points[0] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c, r));
            dem_terrain_points[1] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c + 1, r));
            dem_terrain_points[2] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c + 1, r + 1));
            dem_terrain_points[3] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c, r + 1));

            if (mOrthorectification->hasNodataValue()) {
                double nodata_value = mOrthorectification->nodataValue();
                if (dem_terrain_points[0].z == nodata_value ||
                    dem_terrain_points[1].z == nodata_value ||
                    dem_terrain_points[2].z == nodata_value ||
                    dem_terrain_points[3].z == nodata_value) {
                    return;
                }
            }

            std::vector<tl::Point<double>> dem_image_coordinates(4);
            std::vector<tl::Point<double>> photocoordinates(4);
            std::vector<tl::Point<double>> image_coordinates(4);

            for (int i = 0; i < 4; ++i) {
                dem_image_coordinates[i] = inverse_transform.transform(static_cast<tl::Point<double>>(dem_terrain_points[i]));
                photocoordinates[i] = mOrthorectification->terrainToPhotoCoordinates(dem_terrain_points[i]);
                image_coordinates[i] = mOrthorectification->photoCoordinatesToImageCoordinates(photocoordinates[i]);
            }

            if (!(rect_image.contains(image_coordinates[0]) &&
                rect_image.contains(image_coordinates[1]) &&
                rect_image.contains(image_coordinates[2]) &&
                rect_image.contains(image_coordinates[3]) &&
                mRectOrtho.contains(dem_image_coordinates[0]) &&
                mRectOrtho.contains(dem_image_coordinates[1]) &&
                mRectOrtho.contains(dem_image_coordinates[2]) &&
                mRectOrtho.contains(dem_image_coordinates[3]))) {
                return;
            }

            tl::WindowI window_ortho_in = boundingWindow(dem_image_coordinates.begin(), dem_image_coordinates.end());
            if (!window_ortho_in.isValid()) return;

            tl::WindowI window_image_in = boundingWindow(image_coordinates.begin(), image_coordinates.end());
            if (!window_image_in.isValid()) return;

            // preparar puntos OpenCV
            cv::Point2f cv_photo_image_coordinates[4];
            cv::Point2f cv_ortho_image_coordinates[4];
            for (int i = 0; i < 4; i++) {
                cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(image_coordinates[i].x - window_image_in.pt1.x),
                                                            static_cast<float>(image_coordinates[i].y - window_image_in.pt1.y));
                cv_ortho_image_coordinates[i] = cv::Point2f(static_cast<float>(dem_image_coordinates[i].x - window_ortho_in.pt1.x),
                                                            static_cast<float>(dem_image_coordinates[i].y - window_ortho_in.pt1.y));
            }

            cv::Mat h = cv::getPerspectiveTransform(cv_ortho_image_coordinates, cv_photo_image_coordinates);
            cv::Mat hInv;
            cv::invert(h, hInv, cv::DECOMP_LU);

            std::vector<cv::Point2f> points_ortho;
            std::vector<cv::Point2f> points_image;
            points_ortho.emplace_back(static_cast<float>(window_ortho_in.center().x - window_ortho_in.pt1.x),
                static_cast<float>(window_ortho_in.center().y - window_ortho_in.pt1.y));

            cv::perspectiveTransform(points_ortho, points_image, hInv);

            tl::Point<int> pt_image = window_image_in.center();

            if (rect_image.contains(pt_image)) {

                tl::Point3<double> terrain_point = dem_terrain_points[0];
                for (size_t j = 1; j < dem_terrain_points.size(); j++) {
                    terrain_point += (dem_terrain_points[j] - terrain_point) / (j + 1);
                }

                double distance = tl::distance3D(terrain_point, mOrthorectification->orientation().position());
                float z_buffer_distance = mDistances.at<float>(pt_image.y, pt_image.x);

                if (distance < static_cast<double>(z_buffer_distance) + gsd * 5.0) {

                    std::vector<cv::Point> pts(4);
                    for (size_t k = 0; k < image_coordinates.size(); k++) {
                        pts[k] = cv::Point(tl::roundToInteger(image_coordinates[k].x - window_image_in.pt1.x),
                                           tl::roundToInteger(image_coordinates[k].y - window_image_in.pt1.y));
                    }

                    cv::Mat mask_image = cv::Mat::zeros(window_image_in.height(), window_image_in.width(), CV_8U);
                    cv::fillConvexPoly(mask_image, pts, cv::Scalar(255));

                    cv::Mat image_distances(window_image_in.height(), window_image_in.width(), CV_32F, distance);
                    image_distances.copyTo(mDistances.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                                           .rowRange(window_image_in.pt1.y, window_image_in.pt2.y),
                                           mask_image);

                    cv::Mat image_z_buffer_y(window_image_in.height(), window_image_in.width(), CV_32S, r - rect_dtm.y);
                    image_z_buffer_y.copyTo(mY.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                                            .rowRange(window_image_in.pt1.y, window_image_in.pt2.y),
                                            mask_image);

                    cv::Mat image_z_buffer_x(window_image_in.height(), window_image_in.width(), CV_32S, c - rect_dtm.x);
                    image_z_buffer_x.copyTo(mX.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                                            .rowRange(window_image_in.pt1.y, window_image_in.pt2.y),
                                            mask_image);
                }
            }
        });

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Z-Buffer error");
    }
}

auto ZBuffer::distances() const -> cv::Mat
{
    return mDistances;
}

auto ZBuffer::mapX() const -> cv::Mat
{
    return mX;
}

auto ZBuffer::mapY() const -> cv::Mat
{
    return mY;
}

void ZBuffer::clear()
{
    mDistances.release();
    mY.release();
    mX.release();
}

} // End namespace graphos
