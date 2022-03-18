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

#include "graphos/core/densification/DenseExport.h"

#include <colmap/util/ply.h>

#include <fstream>
#include <iomanip>

namespace graphos
{

DenseExport::DenseExport(const std::string &denseModel)
  : mDenseModel(denseModel),
    mOffset()
{

}

void DenseExport::setOffset(const tl::Point3<double> &point)
{
  mOffset = point;
}

void DenseExport::exportToCSV(const std::string &csv,
                              const tl::EnumFlags<DenseExport::Fields> &flag,
                              tl::BoundingBox<tl::Point3<double>> *bbox)
{
  //std::vector<colmap::PlyPoint> points = colmap::ReadPly(mDenseModel.toStdString());

  std::ofstream stream(csv, std::ios::trunc);

  if (stream.is_open()){

    tl::Point3<double> point;

    stream << "X;Y;Z";
    if (flag.isActive(Fields::rgb)){
      stream << ";R;G;B";
    }

    if (flag.isActive(Fields::normals)) {
      stream << ";Nx;Ny;Nz";
    }
    stream << std::endl;

    std::vector<colmap::PlyPoint> points = colmap::ReadPly(mDenseModel);

    //tl::BoundingBox<tl::Point3<double>> bounding_box;

    for (size_t i = 0; i < points.size(); i++){

      point.x = points[i].x;
      point.y = points[i].y;
      point.z = points[i].z;

      point += mOffset;

      if (bbox) {
        bbox->pt1.x = std::min(bbox->pt1.x, point.x);
        bbox->pt1.y = std::min(bbox->pt1.y, point.y);
        bbox->pt1.z = std::min(bbox->pt1.z, point.z);
        bbox->pt2.x = std::max(bbox->pt2.x, point.x);
        bbox->pt2.y = std::max(bbox->pt2.y, point.y);
        bbox->pt2.z = std::max(bbox->pt2.z, point.z);
      }

      stream << std::fixed << std::setprecision(3)
             << point.x << ";"
             << point.y << ";"
             << point.z;

      if (flag.isActive(Fields::rgb)){
        stream << ";" <<
                  static_cast<int>(points[i].r) << ";" <<
                  static_cast<int>(points[i].g) << ";" <<
                  static_cast<int>(points[i].b);
      }

      if (flag.isActive(Fields::normals)) {
        stream << ";" <<
                  points[i].nx << ";" <<
                  points[i].ny << ";" <<
                  points[i].nz;
      }

      stream << std::endl;
    }

    stream.close();
  }
}

} // namespace graphos
