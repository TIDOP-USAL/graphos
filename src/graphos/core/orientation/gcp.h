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

#ifndef GRAPHOS_CORE_GROUND_CONTROL_POINT_H
#define GRAPHOS_CORE_GROUND_CONTROL_POINT_H


#include "graphos/graphos_global.h"

#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>

#include <unordered_map>

namespace graphos
{
	
class GroundControlPoint
{

public:

  GroundControlPoint();
  ~GroundControlPoint();

  std::string name() const;
  void setName(const std::string &name);
  tl::Point3<double> point() const;
  void setPoint(const tl::Point3<double> &point);
  double x() const;
  void setX(double x);
  double y() const;
  void setY(double y);
  double z() const;
  void setZ(double z);
  void addImagePoint(size_t imageId, 
                     const tl::Point<double> &point);
  tl::Point<double> imagePoint(size_t imageId) const;
  bool existImagePoint(size_t imageId) const;
  void removeImagePoint(size_t imageId);
  const std::unordered_map<size_t, tl::Point<double>> &imagePoints() const;

protected:

  std::string mName;
  tl::Point3<double> mCoordinates;
  std::unordered_map<size_t, tl::Point<double>> mPoints;
};


///TODO: Crear una factoria de clases para lectura/escritura de 
//       diferentes formatos de puntos de control
std::vector<GroundControlPoint> groundControlPointsRead(const tl::Path &gcpFile);

///TODO: La escritura tendría que estar aqui
void groundControlPointsWrite(const tl::Path &gcpFile, const std::vector<GroundControlPoint> &gcps);

} // namespace graphos

#endif // GRAPHOS_CORE_GROUND_CONTROL_POINT_H
