#include "invdistnn.h"

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/process.h>
#include <tidop/geometry/size.h>

// BOOST
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


using namespace inspector;
using namespace tl;

namespace fs = boost::filesystem;

namespace inspector
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

QString DtmInvDistNNProperties::name() const
{
  return QString("INVDISTNN");
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

bool DtmInvDistNNAlgorithm::run(const QString &pointCloud,
                                const QString &dtmFile,
                                const tl::Size<int> &size)
{
  
  fs::path gdal_vrt_file = pointCloud.toStdString();
  gdal_vrt_file.replace_extension(".vrt");
  std::string layer_name = gdal_vrt_file.stem().string();
  std::ofstream file(gdal_vrt_file.string(), std::ios::trunc);
  if (!file.is_open()) return false;
  file << "<OGRVRTDataSource>" << std::endl;
  file << "    <OGRVRTLayer name=\"" << layer_name << "\">" << std::endl;
  file << "        <SrcDataSource>" << pointCloud.toStdString() << "</SrcDataSource>"  << std::endl;
  file << "        <GeometryType>wkbPoint</GeometryType>"  << std::endl;
  file << "        <GeometryField encoding=\"PointFromColumns\" x=\"X\" y=\"Y\" z=\"Z\"/>"  << std::endl;
  file << "    </OGRVRTLayer>"  << std::endl;
  file << "</OGRVRTDataSource>"  << std::endl;
  file.close();

  fs::path app_path(tl::getRunfile());

  std::string cmd("/c \"");
  cmd.append(app_path.parent_path().string());
  cmd.append("\\gdal_grid.exe\" ");
  cmd.append(" -a invdistnn");
  cmd.append(":power=").append(std::to_string(DtmInvDistNNProperties::power()));
  cmd.append(":smoothing=").append(std::to_string(DtmInvDistNNProperties::smoothing()));
  cmd.append(":radius=").append(std::to_string(DtmInvDistNNProperties::radius()));
  cmd.append(":max_points=").append(std::to_string(DtmInvDistNNProperties::maxPoints()));
  cmd.append(":min_points=").append(std::to_string(DtmInvDistNNProperties::minPoints()));
  cmd.append(" -outsize ").append(std::to_string(size.width)).append(" ").append(std::to_string(size.height));
  cmd.append(" -of GTiff -ot Float32 -l ").append(layer_name).append(" ");
  cmd.append(gdal_vrt_file.string()).append(" ");
  cmd.append(dtmFile.toStdString()).append(" ");
  cmd.append(" --config GDAL_NUM_THREADS ALL_CPUS");

  CmdProcess process(cmd);

  if (process.run() == Process::Status::error) {
    return true;
  }

  return false;
}

} // namespace inspector





