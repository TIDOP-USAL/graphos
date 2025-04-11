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
                 const tl::Rect<int> &rectOrtho,
                 const tl::Affine<double, 2> &georeference)
  : mOrthorectification(orthorectification),
    mRectOrtho(rectOrtho),
    mGeoreference(georeference)
{
    tl::Rect<int> rect_image = mOrthorectification->rectImage();

    mDistances = cv::Mat::zeros(rect_image.height, rect_image.width, CV_32F);
    mY = cv::Mat(rect_image.height, rect_image.width, CV_32S, -1);
    mX = cv::Mat(rect_image.height, rect_image.width, CV_32S, -1);
}

ZBuffer::~ZBuffer()
{
}

void ZBuffer::run()
{

    try {

        tl::ChronoAuto chrono("ZBuffer");

        auto gsd = mGeoreference.scale().x();

        tl::Rect<int> rect_image = mOrthorectification->rectImage();
        tl::Rect<int> rect_dtm = mOrthorectification->rectDtm();
        
        tl::parallel_for(rect_dtm.y, rect_dtm.y + rect_dtm.height - 1, [&](size_t r) {
        
            std::vector<tl::Point3<double>> dem_terrain_points(4);
            std::vector<tl::Point<double>> dem_image_coordinates(4);
            std::vector<tl::Point<double>> photocoordinates(4);
            std::vector<tl::Point<double>> image_coordinates(4);

            for (int c = rect_dtm.x; c < rect_dtm.x + rect_dtm.width - 1; c++) {

                dem_terrain_points[0] = mOrthorectification->dtmImageCoordinatesToTerrain(tl::Point<int>(c, r));
                dem_terrain_points[1] = mOrthorectification->dtmImageCoordinatesToTerrain(tl::Point<int>(c + 1, r));
                dem_terrain_points[2] = mOrthorectification->dtmImageCoordinatesToTerrain(tl::Point<int>(c + 1, r + 1));
                dem_terrain_points[3] = mOrthorectification->dtmImageCoordinatesToTerrain(tl::Point<int>(c, r + 1));

                if (mOrthorectification->hasNodataValue()) {
                    double nodata_value = mOrthorectification->nodataValue();
                    if (dem_terrain_points[0].z == nodata_value ||
                        dem_terrain_points[1].z == nodata_value ||
                        dem_terrain_points[2].z == nodata_value ||
                        dem_terrain_points[3].z == nodata_value) {
                        continue;
                    }
                }

                auto inverse_transform = mGeoreference.inverse();
                dem_image_coordinates[0] = inverse_transform.transform(static_cast<tl::Point<double>>(dem_terrain_points[0]));
                dem_image_coordinates[1] = inverse_transform.transform(static_cast<tl::Point<double>>(dem_terrain_points[1]));
                dem_image_coordinates[2] = inverse_transform.transform(static_cast<tl::Point<double>>(dem_terrain_points[2]));
                dem_image_coordinates[3] = inverse_transform.transform(static_cast<tl::Point<double>>(dem_terrain_points[3]));

                photocoordinates[0] = mOrthorectification->terrainToPhotoCoordinates(dem_terrain_points[0]);
                photocoordinates[1] = mOrthorectification->terrainToPhotoCoordinates(dem_terrain_points[1]);
                photocoordinates[2] = mOrthorectification->terrainToPhotoCoordinates(dem_terrain_points[2]);
                photocoordinates[3] = mOrthorectification->terrainToPhotoCoordinates(dem_terrain_points[3]);

                image_coordinates[0] = mOrthorectification->photoCoordinatesToImageCoordinates(photocoordinates[0]);
                image_coordinates[1] = mOrthorectification->photoCoordinatesToImageCoordinates(photocoordinates[1]);
                image_coordinates[2] = mOrthorectification->photoCoordinatesToImageCoordinates(photocoordinates[2]);
                image_coordinates[3] = mOrthorectification->photoCoordinatesToImageCoordinates(photocoordinates[3]);

                if (rect_image.contains(image_coordinates[0]) &&
                    rect_image.contains(image_coordinates[1]) &&
                    rect_image.contains(image_coordinates[2]) &&
                    rect_image.contains(image_coordinates[3]) &&
                    mRectOrtho.contains(dem_image_coordinates[0]) &&
                    mRectOrtho.contains(dem_image_coordinates[1]) &&
                    mRectOrtho.contains(dem_image_coordinates[2]) &&
                    mRectOrtho.contains(dem_image_coordinates[3])) {

                    tl::WindowI window_ortho_in = boundingWindow(dem_image_coordinates.begin(), dem_image_coordinates.end());
                    if (!window_ortho_in.isValid()) continue;

                    tl::WindowI window_image_in = boundingWindow(image_coordinates.begin(), image_coordinates.end());
                    if (!window_image_in.isValid()) continue;

                    cv::Point2f cv_photo_image_coordinates[4];
                    cv::Point2f cv_ortho_image_coordinates[4];
                    for (int i = 0; i < 4; i++) {
                        cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(image_coordinates[i].x - window_image_in.pt1.x),
                                                                    static_cast<float>(image_coordinates[i].y - window_image_in.pt1.y));
                        cv_ortho_image_coordinates[i] = cv::Point2f(static_cast<float>(dem_image_coordinates[i].x - window_ortho_in.pt1.x),
                                                                    static_cast<float>(dem_image_coordinates[i].y - window_ortho_in.pt1.y));
                    }
                    cv::Mat h = cv::getPerspectiveTransform(cv_ortho_image_coordinates, cv_photo_image_coordinates);

                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                     //Se comprueba si ya se han tomado esos valores

                    {

                        std::vector<cv::Point2f> points_image;
                        std::vector<cv::Point2f> points_ortho;
                        points_ortho.emplace_back(static_cast<float>(window_ortho_in.center().x - window_ortho_in.pt1.x),
                                                  static_cast<float>(window_ortho_in.center().y - window_ortho_in.pt1.y));

                        cv::perspectiveTransform(points_ortho, points_image, h.inv());

                        tl::Point<int> pt_image = window_image_in.center();

                        if (rect_image.contains(pt_image)) {
                            tl::Point3<double> terrain_point = dem_terrain_points[0];
                            for (size_t j = 1; j < dem_terrain_points.size(); j++) {
                                terrain_point += (dem_terrain_points[j] - terrain_point) / (j + 1);
                            }

                            // Revisar
                            double distance = tl::distance3D(terrain_point, mOrthorectification->orientation().position());
                            double z_buffer_distance = mDistances.at<float>(pt_image.y, pt_image.x);

                            if (z_buffer_distance == 0. || distance < z_buffer_distance + gsd * 5./*0.25*/) {
                                cv::Mat mask_image = cv::Mat::zeros(window_image_in.height(), window_image_in.width(), CV_8U);
                                std::vector<cv::Point> pts(4);
                                for (size_t k = 0; k < image_coordinates.size(); k++) {
                                    pts[k] = cv::Point(tl::roundToInteger(image_coordinates[k].x - window_image_in.pt1.x),
                                        tl::roundToInteger(image_coordinates[k].y - window_image_in.pt1.y));
                                }
                                const cv::Point *cpts = (const cv::Point *)cv::Mat(pts).data;
                                int npts = static_cast<int>(pts.size());
                                cv::fillPoly(mask_image, &cpts, &npts, 1, cv::Scalar(255));

                                cv::Mat image_distances(window_image_in.height(), window_image_in.width(), CV_32F);
                                image_distances = distance;
                                image_distances.copyTo(mDistances.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                                               .rowRange(window_image_in.pt1.y, window_image_in.pt2.y), mask_image);

                                cv::Mat image_z_buffer_y(window_image_in.height(), window_image_in.width(), CV_32S, r - rect_dtm.y);
                                image_z_buffer_y.copyTo(mY.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                                                .rowRange(window_image_in.pt1.y, window_image_in.pt2.y), mask_image);
                                cv::Mat image_z_buffer_x(window_image_in.height(), window_image_in.width(), CV_32S, c - rect_dtm.x);
                                image_z_buffer_x.copyTo(mX.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                                                .rowRange(window_image_in.pt1.y, window_image_in.pt2.y), mask_image);
                            }

                        }

                    }

                }
            }
        });

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Z-Buffer error");
    }

}

cv::Mat ZBuffer::distances() const
{
    return mDistances;
}

cv::Mat ZBuffer::mapX() const
{
    return mX;
}

cv::Mat ZBuffer::mapY() const
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
