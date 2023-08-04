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

#include "graphos/core/sfm/poses.h"

#include <tidop/core/path.h>
#include <tidop/math/algebra/rotation_convert.h>

using namespace tl;

namespace graphos
{


CameraPose::CameraPose()
    : mPosition(),
      mRotation(nullptr),
      mCrs(""),
      mSource("")
{
}

CameraPose::CameraPose(double x, double y, double z,
                       const RotationMatrix<double> &rotationMatrix)
    : mPosition(x, y, z),
      mRotation(new RotationMatrix<double>(rotationMatrix)),
      mCrs(""),
      mSource("")
{

}

CameraPose::CameraPose(const Point3<double> &center,
                       const RotationMatrix<double> &rotationMatrix)
    : mPosition(center),
      mRotation(new RotationMatrix<double>(rotationMatrix)),
      mCrs(""),
      mSource("")
{

}

CameraPose::CameraPose(double x, double y, double z,
                       const Quaternion<double> &quaternion)
    : mPosition(x, y, z),
      mRotation(new Quaternion<double>(quaternion)),
      mCrs(""),
      mSource("")
{

}

CameraPose::CameraPose(const Point3<double> &center,
                       const Quaternion<double> &quaternion)
    : mPosition(center),
      mRotation(new Quaternion<double>(quaternion)),
      mCrs(""),
      mSource("")
{

}

CameraPose::~CameraPose()
{
}

Point3<double> CameraPose::position() const
{
    return mPosition;
}

void CameraPose::setPosition(const Point3<double> &position)
{
    mPosition = position;
}

Quaterniond CameraPose::quaternion() const
{
    Quaterniond quaternion = Quaterniond::zero();

    if (mRotation) {

        Orientation::Type type = mRotation->type();
        if (type == Orientation::Type::quaternion) {
            quaternion = *dynamic_cast<Quaterniond *>(mRotation.get());
        } else if (type == Orientation::Type::rotation_matrix) {
            RotationConverter<double>::convert(*dynamic_cast<RotationMatrix<double> *>(mRotation.get()), quaternion);
        }

    }

    return quaternion;
}

void CameraPose::setQuaternion(const Quaterniond &quaternion)
{
    mRotation = std::make_shared<Quaternion<double>>(quaternion);
}

RotationMatrix<double> CameraPose::rotationMatrix() const
{
    RotationMatrix<double> rotation_matrix = RotationMatrix<double>::zero();

    if (mRotation) {
        Orientation::Type type = mRotation->type();
        if (type == Orientation::Type::quaternion) {
            RotationConverter<double>::convert(*dynamic_cast<Quaternion<double> *>(mRotation.get()), rotation_matrix);
        } else if (type == Orientation::Type::rotation_matrix) {
            rotation_matrix = *dynamic_cast<RotationMatrix<double> *>(mRotation.get());
        }
    }

    return rotation_matrix;
}

void CameraPose::setRotationMatrix(const RotationMatrix<double> &rotationMatrix)
{
    mRotation = std::make_shared<RotationMatrix<double>>(rotationMatrix);
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
    return mPosition == Point3<double>();
}

} // namespace graphos
