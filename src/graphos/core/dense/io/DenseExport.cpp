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

#include "graphos/core/dense/DenseExport.h"

#include <colmap/util/ply.h>

#include <fstream>
#include <iomanip>

namespace graphos
{

DenseExport::DenseExport(tl::Path denseModel)
  : mDenseModel(std::move(denseModel))
{

}

void DenseExport::setOffset(const tl::Vector3d &offset)
{
    mOffset = offset;
}

void DenseExport::exportToCSV(const std::string &csv,
                              const tl::EnumFlags<Fields> &flag,
                              tl::BoundingBox<tl::Point3d> *bbox) const
{

    std::ofstream stream(csv, std::ios::trunc);

    if (stream.is_open()) {

        tl::Point3d point;

        stream << "X;Y;Z";
        if (flag.isEnabled(Fields::rgb)) {
            stream << ";R;G;B";
        }

        if (flag.isEnabled(Fields::normals)) {
            stream << ";Nx;Ny;Nz";
        }
        stream << '\n';

        std::vector<colmap::PlyPoint> colmap_points = colmap::ReadPly(mDenseModel.toString());

        for (auto &colmap_point : colmap_points)
        {

            point.x() = colmap_point.x;
            point.y() = colmap_point.y;
            point.z() = colmap_point.z;

            point += mOffset;

            if (bbox) {
                bbox->min().x() = std::min(bbox->min().x(), point.x());
                bbox->min().y() = std::min(bbox->min().y(), point.y());
                bbox->min().z() = std::min(bbox->min().z(), point.z());
                bbox->max().x() = std::max(bbox->max().x(), point.x());
                bbox->max().y() = std::max(bbox->max().y(), point.y());
                bbox->max().z() = std::max(bbox->max().z(), point.z());
            }

            stream << std::fixed << std::setprecision(3)
                   << point.x() << ";"
                   << point.y() << ";"
                   << point.z();

            if (flag.isEnabled(Fields::rgb)) {
                stream << ";" 
                       << static_cast<int>(colmap_point.r) << ";" 
                       << static_cast<int>(colmap_point.g) << ";" 
                       << static_cast<int>(colmap_point.b);
            }

            if (flag.isEnabled(Fields::normals)) {
                stream << ";" 
                       << colmap_point.nx << ";" 
                       << colmap_point.ny << ";" 
                       << colmap_point.nz;
            }

            stream << std::endl;
        }

        stream.close();
    }
}

} // namespace graphos
