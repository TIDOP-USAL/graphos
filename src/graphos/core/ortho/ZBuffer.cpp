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

#include "graphos/core/ortho/ZBuffer.h"

#include <opencv2/imgproc.hpp>

namespace graphos
{


ZBuffer::ZBuffer(Orthorectification *orthorectification,
                 const tl::Rect<int> &rectOrtho,
                 const tl::Affine<tl::PointD> &georeference)
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

    tl::Rect<int> rect_image = mOrthorectification->rectImage();
    tl::Rect<int> rect_dtm = mOrthorectification->rectDtm();

    std::vector<tl::Point3D> dtm_grid_terrain_points(4);
    std::vector<tl::PointD> ortho_image_coordinates(4);
    std::vector<tl::PointD> photo_photocoordinates(4);
    std::vector<tl::PointD> photo_image_coordinates(4);

    for (int r = rect_dtm.y; r < rect_dtm.y + rect_dtm.height - 1; r++) {
      for (int c = rect_dtm.x; c < rect_dtm.x + rect_dtm.width - 1; c++) {

        dtm_grid_terrain_points[0] = mOrthorectification->dtmToTerrain(tl::PointI(c, r));
        dtm_grid_terrain_points[1] = mOrthorectification->dtmToTerrain(tl::PointI(c + 1, r));
        dtm_grid_terrain_points[2] = mOrthorectification->dtmToTerrain(tl::PointI(c + 1, r + 1));
        dtm_grid_terrain_points[3] = mOrthorectification->dtmToTerrain(tl::PointI(c, r + 1));

        if (mOrthorectification->hasNodataValue()) {
          double nodata_value = mOrthorectification->nodataValue();
          if (dtm_grid_terrain_points[0].z == nodata_value || 
              dtm_grid_terrain_points[1].z == nodata_value ||
              dtm_grid_terrain_points[2].z == nodata_value ||
              dtm_grid_terrain_points[3].z == nodata_value) {
            continue;
          }
        }

        ortho_image_coordinates[0] = mGeoreference.transform(dtm_grid_terrain_points[0], tl::Transform::Order::inverse);
        ortho_image_coordinates[1] = mGeoreference.transform(dtm_grid_terrain_points[1], tl::Transform::Order::inverse);
        ortho_image_coordinates[2] = mGeoreference.transform(dtm_grid_terrain_points[2], tl::Transform::Order::inverse);
        ortho_image_coordinates[3] = mGeoreference.transform(dtm_grid_terrain_points[3], tl::Transform::Order::inverse);

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

          tl::WindowI window_ortho_in = boundingWindow(ortho_image_coordinates.begin(), ortho_image_coordinates.end());
          if (!window_ortho_in.isValid()) continue;

          tl::WindowI window_image_in = boundingWindow(photo_image_coordinates.begin(), photo_image_coordinates.end());
          if (!window_image_in.isValid()) continue;


          /// Estoy expandiendo despues de comprobar si se sale de los limites de la imagen... Solucionar cuando refactorice
          /// Habría que calcular la intersección de window_aux con la ventana imagen total.
          tl::WindowI window_aux = tl::expandWindow(window_image_in, 1);
          if (rect_image.contains(window_aux.pt1) &&
            rect_image.contains(window_aux.pt2)) {
            window_image_in = window_aux;
          }

          cv::Point2f cv_photo_image_coordinates[4];
          cv::Point2f cv_ortho_image_coordinates[4];
          for (int i = 0; i < 4; i++) {
            cv_photo_image_coordinates[i] = cv::Point2f(static_cast<float>(photo_image_coordinates[i].x - window_image_in.pt1.x),
              static_cast<float>(photo_image_coordinates[i].y - window_image_in.pt1.y));
            cv_ortho_image_coordinates[i] = cv::Point2f(static_cast<float>(ortho_image_coordinates[i].x - window_ortho_in.pt1.x),
              static_cast<float>(ortho_image_coordinates[i].y - window_ortho_in.pt1.y));
          }
          cv::Mat h = cv::getPerspectiveTransform(cv_ortho_image_coordinates, cv_photo_image_coordinates);

          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
           //Se comprueba si ya se han tomado esos valores

          {

            std::vector<cv::Point2f> points_image;
            std::vector<cv::Point2f> points_ortho;
            points_ortho.emplace_back(window_ortho_in.center().x - window_ortho_in.pt1.x, window_ortho_in.center().y - window_ortho_in.pt1.y);

            cv::perspectiveTransform(points_ortho, points_image, h.inv());

            tl::PointI pt_image = window_image_in.center();

            if (rect_image.contains(pt_image)) {
              tl::Point3D terrain_point = dtm_grid_terrain_points[0];
              for (size_t j = 1; j < dtm_grid_terrain_points.size(); j++) {
                terrain_point += (dtm_grid_terrain_points[j] - terrain_point) / (j + 1);
              }

              double distance = tl::distance3D(terrain_point, mOrthorectification->orientation().position());
              double z_buffer_distance = mDistances.at<float>(pt_image.y, pt_image.x);
              double old_row = mY.at<int>(pt_image.y, pt_image.x);
              double old_col = mX.at<int>(pt_image.y, pt_image.x);

              {
                cv::Mat mask_image = cv::Mat::zeros(window_image_in.height(), window_image_in.width(), CV_8U);
                std::vector<cv::Point> pts;
                for (int k = 0; k < photo_image_coordinates.size(); k++) {
                  pts.emplace_back(tl::roundToInteger(photo_image_coordinates[k].x - window_image_in.pt1.x),
                                   tl::roundToInteger(photo_image_coordinates[k].y - window_image_in.pt1.y));
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
                cv::Mat image_z_buffer_x(window_image_in.height(), window_image_in.width(), CV_32S, c  - rect_dtm.x);
                image_z_buffer_x.copyTo(mX.colRange(window_image_in.pt1.x, window_image_in.pt2.x)
                  .rowRange(window_image_in.pt1.y, window_image_in.pt2.y), mask_image);
              }

            }

          }

        }
      }
    }

  } catch (std::exception &e) {
    //msgError("Z-Buffer compute fail: %s", photo.name().c_str());
    msgError(e.what());
  } catch (...) {
    //msgError("Z-Buffer compute fail: %s", photo.name().c_str());
    msgError("Unhandled exception");
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
