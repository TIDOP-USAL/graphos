#include "poses.h"

#include <tidop/core/defs.h>
#include <tidop/core/path.h>
#include <tidop/math/algebra/rotation_convert.h>

namespace graphos
{


CameraPose::CameraPose()
  : tl::CameraPose(),
    mCrs(""),
    mSource("")
{}

CameraPose::CameraPose(double x, double y, double z,
                       const tl::math::RotationMatrix<double> &rotationMatrix)
  : tl::CameraPose(x, y, z, rotationMatrix),
    mCrs(""),
    mSource("")
{

}

CameraPose::CameraPose(const tl::Point3D &center,
                       const tl::math::RotationMatrix<double> &rotationMatrix)
  : tl::CameraPose(center, rotationMatrix),
    mCrs(""),
    mSource("")
{

}

CameraPose::CameraPose(double x, double y, double z,
                       const tl::math::Quaternion<double> &quaternion)
  : tl::CameraPose(x, y, z, quaternion),
    mCrs(""),
    mSource("")
{

}

CameraPose::CameraPose(const tl::Point3D &center,
                       const tl::math::Quaternion<double> &quaternion)
  : tl::CameraPose(center, quaternion),
    mCrs(""),
    mSource("")
{

}

CameraPose::~CameraPose()
{
}


QString CameraPose::crs() const
{
  return mCrs;
}

void CameraPose::setCrs(const QString &crs)
{
  mCrs = crs;
}

QString CameraPose::source() const
{
  return mSource;
}

void CameraPose::setSource(const QString &source)
{
  mSource = source;
}


} // namespace graphos
