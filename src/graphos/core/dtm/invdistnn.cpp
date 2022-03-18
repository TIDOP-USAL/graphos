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

#include "graphos/core/dtm/invdistnn.h"

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/process.h>
#include <tidop/core/path.h>
#include <tidop/core/app.h>
#include <tidop/geometry/size.h>

#include <fstream>

using namespace graphos;
using namespace tl;


namespace graphos
{

constexpr auto InvDistNNPowerDefault = 2.0;
constexpr auto InvDistNNSmoothingDefault = 0.0;
constexpr auto InvDistNNRadiusDefault = 1.0;
constexpr auto InvDistNNMaxPointsDefault = 12;
constexpr auto InvDistNNMinPointsDefault = 0;


DtmInvDistNNProperties::DtmInvDistNNProperties()
  : DtmInvDistNN(),
    mPower(InvDistNNPowerDefault),
    mSmoothing(InvDistNNSmoothingDefault),
    mRadius(InvDistNNRadiusDefault),
    mMaxPoints(InvDistNNMaxPointsDefault),
    mMinPoints(InvDistNNMinPointsDefault)
{
}

DtmInvDistNNProperties::~DtmInvDistNNProperties()
{

}

double DtmInvDistNNProperties::power() const
{
  return mPower;
}

double DtmInvDistNNProperties::smoothing() const
{
  return mSmoothing;
}

double DtmInvDistNNProperties::radius() const
{
  return mRadius;
}

int DtmInvDistNNProperties::maxPoints() const
{
  return mMaxPoints;
}

int DtmInvDistNNProperties::minPoints() const
{
  return mMinPoints;
}

void DtmInvDistNNProperties::setPower(double power)
{
  mPower = power;
}

void DtmInvDistNNProperties::setSmoothing(double smoothing)
{
  mSmoothing = smoothing;
}

void DtmInvDistNNProperties::setRadius(double radius)
{
  mRadius = radius;
}

void DtmInvDistNNProperties::setMaxPoints(int maxPoints)
{
  mMaxPoints = maxPoints;
}

void DtmInvDistNNProperties::setMinPoints(int minPoints)
{
  mMinPoints = minPoints;
}

std::string DtmInvDistNNProperties::name() const
{
  return std::string("INVDISTNN");
}

void DtmInvDistNNProperties::reset()
{
  mPower = InvDistNNPowerDefault;
  mSmoothing = InvDistNNSmoothingDefault;
  mRadius = InvDistNNRadiusDefault;
  mMaxPoints = InvDistNNMaxPointsDefault;
  mMinPoints = InvDistNNMinPointsDefault;
}









DtmInvDistNNAlgorithm::DtmInvDistNNAlgorithm()
  : DtmInvDistNNProperties(),
    DtmAlgorithm()
{

}

DtmInvDistNNAlgorithm::DtmInvDistNNAlgorithm(double power, 
                                             double smoothing,
                                             double radius, 
                                             int maxPoints, 
                                             int minPoints)
  : DtmInvDistNNProperties(),
    DtmAlgorithm()
{
  DtmInvDistNNProperties::setPower(power);
  DtmInvDistNNProperties::setSmoothing(smoothing);
  DtmInvDistNNProperties::setRadius(radius);
  DtmInvDistNNProperties::setMaxPoints(maxPoints);
  DtmInvDistNNProperties::setMinPoints(minPoints);
}

DtmInvDistNNAlgorithm::~DtmInvDistNNAlgorithm()
{

}

bool DtmInvDistNNAlgorithm::run(const std::string &pointCloud,
                                const std::string &dtmFile,
                                const tl::Size<int> &size)
{
  
  tl::Path gdal_vrt_file(pointCloud);
  gdal_vrt_file.replaceExtension(".vrt");
  std::string layer_name = gdal_vrt_file.baseName().toString();
  std::ofstream file(gdal_vrt_file.toString(), std::ios::trunc);
  if (!file.is_open()) return false;
  file << "<OGRVRTDataSource>" << std::endl;
  file << "    <OGRVRTLayer name=\"" << layer_name << "\">" << std::endl;
  file << "        <SrcDataSource>" << pointCloud << "</SrcDataSource>"  << std::endl;
  file << "        <GeometryType>wkbPoint</GeometryType>"  << std::endl;
  file << "        <GeometryField encoding=\"PointFromColumns\" x=\"X\" y=\"Y\" z=\"Z\"/>"  << std::endl;
  file << "    </OGRVRTLayer>"  << std::endl;
  file << "</OGRVRTDataSource>"  << std::endl;
  file.close();

  tl::Path app_path = tl::App::instance().path();

  std::string cmd("\"");
  cmd.append(app_path.parentPath().toString());
  cmd.append("\\gdal_grid.exe\"");
  cmd.append(" -a invdistnn");
  cmd.append(":power=").append(std::to_string(DtmInvDistNNProperties::power()));
  cmd.append(":smoothing=").append(std::to_string(DtmInvDistNNProperties::smoothing()));
  cmd.append(":radius=").append(std::to_string(DtmInvDistNNProperties::radius()));
  cmd.append(":max_points=").append(std::to_string(DtmInvDistNNProperties::maxPoints()));
  cmd.append(":min_points=").append(std::to_string(DtmInvDistNNProperties::minPoints()));
  cmd.append(":nodata=-9999");
  cmd.append(" -outsize ").append(std::to_string(size.width)).append(" ").append(std::to_string(size.height));
  cmd.append(" -of GTiff -ot Float32 -l ").append(layer_name).append(" \"");
  cmd.append(gdal_vrt_file.toString()).append("\" \"");
  cmd.append(dtmFile).append("\" ");
  cmd.append(" --config GDAL_NUM_THREADS ALL_CPUS");

  ExternalProcess process(cmd);
  process.run();

  return false;
}

bool DtmInvDistNNAlgorithm::run(const std::string &pointCloud,
                                const std::string &dtmFile,
                                const tl::BoundingBox<tl::Point3<double>> &bbox,
                                double gsd)
{

  tl::Path gdal_vrt_file(pointCloud);
  gdal_vrt_file.replaceExtension(".vrt");
  std::string layer_name = gdal_vrt_file.baseName().toString();
  std::ofstream file(gdal_vrt_file.toString(), std::ios::trunc);
  if (!file.is_open()) return false;
  file << "<OGRVRTDataSource>" << std::endl;
  file << "    <OGRVRTLayer name=\"" << layer_name << "\">" << std::endl;
  file << "        <SrcDataSource>" << pointCloud << "</SrcDataSource>" << std::endl;
  file << "        <GeometryType>wkbPoint</GeometryType>" << std::endl;
  file << "        <GeometryField encoding=\"PointFromColumns\" x=\"X\" y=\"Y\" z=\"Z\"/>" << std::endl;
  file << "    </OGRVRTLayer>" << std::endl;
  file << "</OGRVRTDataSource>" << std::endl;
  file.close();

  tl::Path app_path = tl::App::instance().path();

  tl::Size<int> size(bbox.width() / gsd, bbox.height() / gsd);

  std::string cmd("\"");
  cmd.append(app_path.parentPath().toString());
  cmd.append("\\gdal_grid.exe\"");
  cmd.append(" -a invdistnn");
  cmd.append(":power=").append(std::to_string(DtmInvDistNNProperties::power()));
  cmd.append(":smoothing=").append(std::to_string(DtmInvDistNNProperties::smoothing()));
  cmd.append(":radius=").append(std::to_string(DtmInvDistNNProperties::radius()));
  cmd.append(":max_points=").append(std::to_string(DtmInvDistNNProperties::maxPoints()));
  cmd.append(":min_points=").append(std::to_string(DtmInvDistNNProperties::minPoints()));
  cmd.append(":nodata=-9999");
  cmd.append(" -txe ").append(std::to_string(bbox.pt1.x)).append(" ").append(std::to_string(bbox.pt2.x));
  cmd.append(" -tye ").append(std::to_string(bbox.pt1.y)).append(" ").append(std::to_string(bbox.pt2.y));
  cmd.append(" -outsize ").append(std::to_string(size.width)).append(" ").append(std::to_string(size.height));
  cmd.append(" -of GTiff -ot Float32 -l ").append(layer_name).append(" \"");
  cmd.append(gdal_vrt_file.toString()).append("\" \"");
  cmd.append(dtmFile).append("\" ");
  cmd.append(" --config GDAL_NUM_THREADS ALL_CPUS");

  ExternalProcess process(cmd);
  process.run();

  return false;
}

} // namespace graphos





