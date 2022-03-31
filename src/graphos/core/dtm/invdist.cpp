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

#include "graphos/core/dtm/invdist.h"

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/task.h>
#include <tidop/core/app.h>
#include <tidop/geometry/size.h>
#include <tidop/core/path.h>

#include <fstream>

using namespace graphos;
using namespace tl;

namespace tl
{
template<typename T> class Size;
}


namespace graphos
{

constexpr auto InvDistPowerDefault = 2.0;
constexpr auto InvDistSmoothingDefault = 0.0;
constexpr auto InvDistRadius1Default = 0.0;
constexpr auto InvDistRadius2Default = 0.0;
constexpr auto InvDistAngleDefault = 0.0;
constexpr auto InvDistMaxPointsDefault = 0;
constexpr auto InvDistMinPointsDefault = 0;


DtmInvDistProperties::DtmInvDistProperties()
  : DtmInvDist(),
    mPower(InvDistPowerDefault),
    mSmoothing(InvDistSmoothingDefault),
    mRadius1(InvDistRadius1Default),
    mRadius2(InvDistRadius2Default),
    mAngle(InvDistAngleDefault),
    mMaxPoints(InvDistMaxPointsDefault),
    mMinPoints(InvDistMinPointsDefault)
{
}

DtmInvDistProperties::~DtmInvDistProperties()
{

}

double DtmInvDistProperties::power() const
{
  return mPower;
}

double DtmInvDistProperties::smoothing() const
{
  return mSmoothing;
}

double DtmInvDistProperties::radius1() const
{
  return mRadius1;
}

double DtmInvDistProperties::radius2() const
{
  return mRadius2;
}

double DtmInvDistProperties::angle() const
{
  return mAngle;
}

int DtmInvDistProperties::maxPoints() const
{
  return mMaxPoints;
}

int DtmInvDistProperties::minPoints() const
{
  return mMinPoints;
}

void DtmInvDistProperties::setPower(double power)
{
  mPower = power;
}

void DtmInvDistProperties::setSmoothing(double smoothing)
{
  mSmoothing = smoothing;
}

void DtmInvDistProperties::setRadius1(double radius1)
{
  mRadius1 = radius1;
}

void DtmInvDistProperties::setRadius2(double radius2)
{
  mRadius2 = radius2;
}

void DtmInvDistProperties::setAngle(double angle)
{
  mAngle = angle;
}

void DtmInvDistProperties::setMaxPoints(int maxPoints)
{
  mMaxPoints = maxPoints;
}

void DtmInvDistProperties::setMinPoints(int minPoints)
{
  mMinPoints = minPoints;
}

std::string DtmInvDistProperties::name() const
{
  return std::string("INVDIST");
}

void DtmInvDistProperties::reset()
{
  mPower = InvDistPowerDefault;
  mSmoothing = InvDistSmoothingDefault;
  mRadius1 = InvDistRadius1Default;
  mRadius2 = InvDistRadius2Default;
  mAngle = InvDistAngleDefault;
  mMaxPoints = InvDistMaxPointsDefault;
  mMinPoints = InvDistMinPointsDefault;
}









DtmInvDistAlgorithm::DtmInvDistAlgorithm()
  : DtmInvDistProperties(),
    DtmAlgorithm()
{

}

DtmInvDistAlgorithm::DtmInvDistAlgorithm(double power, 
                                     double smoothing,
                                     double radius1, 
                                     double radius2, 
                                     double angle, 
                                     int maxPoints, 
                                     int minPoints)
  : DtmInvDistProperties(),
    DtmAlgorithm()
{
  DtmInvDistProperties::setPower(power);
  DtmInvDistProperties::setSmoothing(smoothing);
  DtmInvDistProperties::setRadius1(radius1);
  DtmInvDistProperties::setRadius2(radius2);
  DtmInvDistProperties::setAngle(angle);
  DtmInvDistProperties::setMaxPoints(maxPoints);
  DtmInvDistProperties::setMinPoints(minPoints);
}

DtmInvDistAlgorithm::~DtmInvDistAlgorithm()
{

}

bool DtmInvDistAlgorithm::run(const std::string &pointCloud,
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
  cmd.append(" -a invdist");
  cmd.append(":power=").append(std::to_string(DtmInvDistProperties::power()));
  cmd.append(":smoothing=").append(std::to_string(DtmInvDistProperties::smoothing()));
  cmd.append(":radius1=").append(std::to_string(DtmInvDistProperties::radius1()));
  cmd.append(":radius2=").append(std::to_string(DtmInvDistProperties::radius2()));
  cmd.append(":angle=").append(std::to_string(DtmInvDistProperties::angle()));
  cmd.append(":max_points=").append(std::to_string(DtmInvDistProperties::maxPoints()));
  cmd.append(":min_points=").append(std::to_string(DtmInvDistProperties::minPoints()));
  cmd.append(":nodata=-9999");
  cmd.append(" -outsize ").append(std::to_string(size.width)).append(" ").append(std::to_string(size.height));
  cmd.append(" -of GTiff -ot Float32 -l ").append(layer_name).append(" \"");
  cmd.append(gdal_vrt_file.toString()).append("\" \"");
  cmd.append(dtmFile).append("\" ");
  cmd.append(" --config GDAL_NUM_THREADS ALL_CPUS");

  Process process(cmd);
  process.run();

  return false;
}

bool DtmInvDistAlgorithm::run(const std::string &pointCloud,
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
  cmd.append(" -a invdist");
  cmd.append(":power=").append(std::to_string(DtmInvDistProperties::power()));
  cmd.append(":smoothing=").append(std::to_string(DtmInvDistProperties::smoothing()));
  cmd.append(":radius1=").append(std::to_string(DtmInvDistProperties::radius1()));
  cmd.append(":radius2=").append(std::to_string(DtmInvDistProperties::radius2()));
  cmd.append(":angle=").append(std::to_string(DtmInvDistProperties::angle()));
  cmd.append(":max_points=").append(std::to_string(DtmInvDistProperties::maxPoints()));
  cmd.append(":min_points=").append(std::to_string(DtmInvDistProperties::minPoints()));
  cmd.append(":nodata=-9999");
  cmd.append(" -txe ").append(std::to_string(bbox.pt1.x)).append(" ").append(std::to_string(bbox.pt2.x));
  cmd.append(" -tye ").append(std::to_string(bbox.pt1.y)).append(" ").append(std::to_string(bbox.pt2.y));
  cmd.append(" -outsize ").append(std::to_string(size.width)).append(" ").append(std::to_string(size.height));
  cmd.append(" -of GTiff -ot Float32 -l ").append(layer_name).append(" \"");
  cmd.append(gdal_vrt_file.toString()).append("\" \"");
  cmd.append(dtmFile).append("\" ");
  cmd.append(" --config GDAL_NUM_THREADS ALL_CPUS");

  Process process(cmd);
  process.run();

  return false;
}

} // namespace graphos
