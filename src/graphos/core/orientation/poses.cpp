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

#include "graphos/core/orientation/poses.h"

#include <tidop/core/path.h>
#include <tidop/math/algebra/rotation_convert.h>

namespace graphos
{


CameraPose::CameraPose()
  : mPosition(),
    mRotation(nullptr),
    mCrs(""),
    mSource("")
{}

CameraPose::CameraPose(double x, double y, double z,
                       const tl::math::RotationMatrix<double> &rotationMatrix)
  : mPosition(x, y, z),
    mRotation(new tl::math::RotationMatrix<double>(rotationMatrix)),
    mCrs(""),
    mSource("")
{

}

CameraPose::CameraPose(const tl::Point3D &center,
                       const tl::math::RotationMatrix<double> &rotationMatrix)
  : mPosition(center),
    mRotation(new tl::math::RotationMatrix<double>(rotationMatrix)),
    mCrs(""),
    mSource("")
{

}

CameraPose::CameraPose(double x, double y, double z,
                       const tl::math::Quaternion<double> &quaternion)
  : mPosition(x, y, z),
    mRotation(new tl::math::Quaternion<double>(quaternion)),
    mCrs(""),
    mSource("")
{

}

CameraPose::CameraPose(const tl::Point3D &center,
                       const tl::math::Quaternion<double> &quaternion)
  : mPosition(center),
    mRotation(new tl::math::Quaternion<double>(quaternion)),
    mCrs(""),
    mSource("")
{

}

CameraPose::~CameraPose()
{
}

tl::Point3D CameraPose::position() const
{
  return mPosition;
}

void CameraPose::setPosition(const tl::Point3D &position)
{
  mPosition = position;
}

tl::math::Quaterniond CameraPose::quaternion() const
{
  tl::math::Quaterniond quaternion = tl::math::Quaterniond::zero();

  if (mRotation) {

    tl::math::Rotation::Type type = mRotation->rotationType();
    if (type == tl::math::Rotation::Type::quaternion) {
      quaternion = *dynamic_cast<tl::math::Quaterniond *>(mRotation.get());
    } else if (type == tl::math::Rotation::Type::rotation_matrix) {
      tl::math::RotationConverter<double>::convert(*dynamic_cast<tl::math::RotationMatrix<double> *>(mRotation.get()), quaternion);
    }

  }

  return quaternion;
}

void CameraPose::setQuaternion(const tl::math::Quaterniond &quaternion)
{
  mRotation = std::make_shared<tl::math::Quaternion<double>>(quaternion);
}

tl::math::RotationMatrix<double> CameraPose::rotationMatrix() const
{
  tl::math::RotationMatrix<double> rotation_matrix = tl::math::RotationMatrix<double>::zero();

  if (mRotation) {
    tl::math::Rotation::Type type = mRotation->rotationType();
    if (type == tl::math::Rotation::Type::quaternion) {
      tl::math::RotationConverter<double>::convert(*dynamic_cast<tl::math::Quaternion<double> *>(mRotation.get()), rotation_matrix);
    } else if (type == tl::math::Rotation::Type::rotation_matrix) {
      rotation_matrix = *dynamic_cast<tl::math::RotationMatrix<double> *>(mRotation.get());
    }
  }

  return rotation_matrix;
}

void CameraPose::setRotationMatrix(const tl::math::RotationMatrix<double> &rotationMatrix)
{
  mRotation = std::make_shared<tl::math::RotationMatrix<double>>(rotationMatrix);
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

bool CameraPose::isEmpty() const
{
  return mPosition == tl::Point3D();
}

} // namespace graphos
