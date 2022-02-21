/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#ifndef GRAPHOS_CORE_DENSE_EXPORT_H
#define GRAPHOS_CORE_DENSE_EXPORT_H

#include <tidop/core/flags.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geometry/entities/bbox.h>

namespace graphos
{

class DenseExport
{

public:

  enum class Fields
  {
    xyz = 1 << 0,
    rgb = 1 << 1,
    normals = 1 << 2
  };

public:

  DenseExport(const std::string &denseModel);

  void setOffset(const tl::Point3<double> &point);
  void exportToCSV(const std::string &csv,
                   const tl::EnumFlags<Fields> &flag,
                   tl::BoundingBox<tl::Point3<double>> *bbox = nullptr);

protected:

  std::string mDenseModel;
  tl::Point3<double> mOffset;
};

} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_EXPORT_H
