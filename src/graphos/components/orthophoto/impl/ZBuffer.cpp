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

#include <tidop/core/base/Chrono.h>
#include <tidop/core/base/TypeConversions.h>
#include <tidop/geometry/algorithms/spatial/Envelope.h>
#include <tidop/geometry/algorithms/measurement/Distance.h>

namespace graphos
{


ZBuffer::ZBuffer(Orthorectification *orthorectification,
                 const tl::Size<int> &sizeOrtho,
                 const tl::Affine<double, 2> &georeference)
  : mOrthorectification(orthorectification),
    mRectOrtho(0, 0, sizeOrtho.width(), sizeOrtho.height()),
    mEnuGeoreference(georeference)
{
    tl::Rect<int> rect_image = mOrthorectification->rectImage();

    mDistances = cv::Mat::ones(rect_image.height(), rect_image.width(), CV_32F) * FLT_MAX;
    mY = cv::Mat(rect_image.height(), rect_image.width(), CV_32S, -1);
    mX = cv::Mat(rect_image.height(), rect_image.width(), CV_32S, -1);
}

ZBuffer::~ZBuffer()
{
}

struct Tile
{
    tl::Rect<int> rect;      // area covered by the tile
    tl::Point<int> origin;   // starting pixel (depends on relative position)
    tl::Point<int> step;     // traversal direction: (1,1)=down-right, (-1,1)=down-left, etc.
};

/*!
 * \brief Build a grid of tiles covering the given rectangle.
 * Each tile knows its traversal origin and direction relative to the center of the area.
 * \param rect Full area to cover (e.g. DTM area)
 * \param tileSize Size of each tile in pixels (e.g. 64 or 128)
 */
static auto buildTiles(const tl::Rect<int> &rect, int tileSize) -> std::vector<Tile>
{
    std::vector<Tile> tiles;

    int cols = (rect.width() + tileSize - 1) / tileSize;
    int rows = (rect.height() + tileSize - 1) / tileSize;

    int cx = rect.x() + rect.width() / 2;
    int cy = rect.y() + rect.height() / 2;

    for (int ty = 0; ty < rows; ++ty) {
        for (int tx = 0; tx < cols; ++tx) {

            int x0 = rect.x() + tx * tileSize;
            int y0 = rect.y() + ty * tileSize;
            int w = std::min(tileSize, rect.x() + rect.width() - x0);
            int h = std::min(tileSize, rect.y() + rect.height() - y0);

            tl::Rect<int> tileRect(x0, y0, w, h);

            double dx = (x0 + w / 2.0) - cx;
            double dy = (y0 + h / 2.0) - cy;

            Tile t;
            t.rect = tileRect;

            // Choose traversal origin and direction
            if (dx >= 0 && dy >= 0) {               // bottom-right
                t.origin = tl::Point2i{x0, y0};
                t.step = tl::Point2i{1, 1};
            } else if (dx < 0 && dy >= 0) {         // bottom-left
                t.origin = tl::Point2i{x0 + w - 1, y0};
                t.step = tl::Point2i{-1, 1};
            } else if (dx < 0 && dy < 0) {          // top-left
                t.origin = tl::Point2i{x0 + w - 1, y0 + h - 1};
                t.step = tl::Point2i{-1, -1};
            } else {                                // top-right
                t.origin = tl::Point2i{x0, y0 + h - 1};
                t.step = tl::Point2i{1, -1};
            }

            tiles.push_back(std::move(t));
        }
    }

    return tiles;
}

void ZBuffer::run()
{
    try {

        tl::ChronoAuto chrono("ZBuffer");

        auto gsd = mEnuGeoreference.scale().x();

        tl::Rect<int> rect_image = mOrthorectification->rectImage();
        tl::Rect<int> rect_dtm = mOrthorectification->rectDsm();

        tl::Rect<int> iterRect(rect_dtm.x(), rect_dtm.y(), rect_dtm.width() - 1, rect_dtm.height() - 1);
        auto tiles = buildTiles(iterRect, 128);  // tile size configurable (64, 128, etc.)

        auto inverse_transform = mEnuGeoreference.inverse();

        tl::parallel_for(size_t(0), tiles.size(), [&](size_t t){

            const auto &tile = tiles[t];

            for (int y = 0; y < tile.rect.height(); ++y) {
                for (int x = 0; x < tile.rect.width(); ++x) {

                    int c = tile.origin.x() + x * tile.step.x();
                    int r = tile.origin.y() + y * tile.step.y();
                    if (c < tile.rect.x() || c >= tile.rect.x() + tile.rect.width() ||
                        r < tile.rect.y() || r >= tile.rect.y() + tile.rect.height())
                        continue;

                    std::vector<tl::Point3d> dem_enu_coordinates(4);

                    dem_enu_coordinates[0] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c, r));
                    dem_enu_coordinates[1] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c + 1, r));
                    dem_enu_coordinates[2] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c + 1, r + 1));
                    dem_enu_coordinates[3] = mOrthorectification->dsmImageCoordinatesToTerrain(tl::Point<int>(c, r + 1));

                    if (mOrthorectification->hasNodataValue()) {
                        double nodata_value = mOrthorectification->nodataValue();
                        if (dem_enu_coordinates[0].z() == nodata_value ||
                            dem_enu_coordinates[1].z() == nodata_value ||
                            dem_enu_coordinates[2].z() == nodata_value ||
                            dem_enu_coordinates[3].z() == nodata_value) {
                            continue;
                        }
                    }

                    std::vector<tl::Point2i> dem_image_coordinates(4);
                    std::vector<tl::Point2i> photocoordinates(4);
                    std::vector<tl::Point2i> image_coordinates(4);

                    for (int i = 0; i < 4; ++i) {
                        dem_image_coordinates[i] = static_cast<tl::Point2i>(inverse_transform.transform(static_cast<tl::Point2d>(dem_enu_coordinates[i])));
                        photocoordinates[i] = static_cast<tl::Point2i>(mOrthorectification->terrainToPhotoCoordinates(dem_enu_coordinates[i]));
                        image_coordinates[i] = static_cast<tl::Point2i>(mOrthorectification->photoCoordinatesToImageCoordinates(static_cast<tl::Point2d>(photocoordinates[i])));
                    }

                    if (!(rect_image.contains(image_coordinates[0]) &&
                          rect_image.contains(image_coordinates[1]) &&
                          rect_image.contains(image_coordinates[2]) &&
                          rect_image.contains(image_coordinates[3]) &&
                          mRectOrtho.contains(dem_image_coordinates[0]) &&
                          mRectOrtho.contains(dem_image_coordinates[1]) &&
                          mRectOrtho.contains(dem_image_coordinates[2]) &&
                          mRectOrtho.contains(dem_image_coordinates[3]))) {
                        continue;
                    }

                    tl::BoundingBox2i window_ortho_in = tl::envelope(dem_image_coordinates[0],
                                                                     dem_image_coordinates[1],
                                                                     dem_image_coordinates[2],
                                                                     dem_image_coordinates[3]);
                    if (!window_ortho_in.isValid()) continue;

                    tl::BoundingBox2i window_image_in = tl::envelope(image_coordinates[0], 
                                                                     image_coordinates[1], 
                                                                     image_coordinates[2], 
                                                                     image_coordinates[3]);
                    if (!window_image_in.isValid()) continue;

                    // preparar puntos OpenCV
                    cv::Point2f cv_photo_image_coordinates[4];
                    cv::Point2f cv_ortho_image_coordinates[4];
                    for (int i = 0; i < 4; i++) {
                        cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(image_coordinates[i].x() - window_image_in.min().x()),
                                                                    static_cast<float>(image_coordinates[i].y() - window_image_in.min().y()));
                        cv_ortho_image_coordinates[i] = cv::Point2f(static_cast<float>(dem_image_coordinates[i].x() - window_ortho_in.min().x()),
                                                                    static_cast<float>(dem_image_coordinates[i].y() - window_ortho_in.min().y()));
                    }

                    cv::Mat h = cv::getPerspectiveTransform(cv_ortho_image_coordinates, cv_photo_image_coordinates);
                    cv::Mat hInv;
                    cv::invert(h, hInv, cv::DECOMP_LU);

                    std::vector<cv::Point2f> points_ortho;
                    std::vector<cv::Point2f> points_image;
                    points_ortho.emplace_back(static_cast<float>(window_ortho_in.center().x() - window_ortho_in.min().x()),
                                              static_cast<float>(window_ortho_in.center().y() - window_ortho_in.min().y()));
                    cv::perspectiveTransform(points_ortho, points_image, hInv);

                    tl::Point<int> pt_image = window_image_in.center();

                    if (rect_image.contains(pt_image)) {

                        tl::Point3d terrain_point = dem_enu_coordinates[0];
                        for (size_t j = 1; j < dem_enu_coordinates.size(); j++) {
                            terrain_point += (dem_enu_coordinates[j] - terrain_point) / (static_cast<double>(j) + 1.);
                        }

                        double distance = tl::distance(terrain_point, mOrthorectification->orientation().position());
                        float z_buffer_distance = mDistances.at<float>(pt_image.y(), pt_image.x());

                        if (distance < static_cast<double>(z_buffer_distance) + gsd * 5.0) {

                            std::vector<cv::Point> pts(4);
                            for (size_t k = 0; k < image_coordinates.size(); k++) {
                                pts[k] = cv::Point(tl::numberCast<int>(image_coordinates[k].x() - window_image_in.min().x()),
                                                   tl::numberCast<int>(image_coordinates[k].y() - window_image_in.min().y()));
                            }

                            //TODO: revisar. Antes nunca era cero
                            if (window_image_in.height() == 0 || window_image_in.width() == 0) continue;

                            cv::Mat mask_image = cv::Mat::zeros(window_image_in.height(), window_image_in.width(), CV_8U);
                            cv::fillConvexPoly(mask_image, pts, cv::Scalar(255));

                            cv::Mat image_distances(window_image_in.height(), window_image_in.width(), CV_32F, distance);
                            image_distances.copyTo(mDistances.colRange(window_image_in.min().x(), window_image_in.max().x())
                                                             .rowRange(window_image_in.min().y(), window_image_in.max().y()),
                                                   mask_image);

                            cv::Mat image_z_buffer_y(window_image_in.height(), window_image_in.width(), CV_32S, r - rect_dtm.y());
                            image_z_buffer_y.copyTo(mY.colRange(window_image_in.min().x(), window_image_in.max().x())
                                                      .rowRange(window_image_in.min().y(), window_image_in.max().y()),
                                                    mask_image);

                            cv::Mat image_z_buffer_x(window_image_in.height(), window_image_in.width(), CV_32S, c - rect_dtm.x());
                            image_z_buffer_x.copyTo(mX.colRange(window_image_in.min().x(), window_image_in.max().x())
                                .rowRange(window_image_in.min().y(), window_image_in.max().y()),
                                mask_image);
                        }
                    }
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
