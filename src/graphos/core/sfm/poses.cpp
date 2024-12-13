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
    : mAccuracy({10., 10., 10.}),
      mRotation(nullptr),
      mCrs(""),
      mSource(""),
      mRtkFlag(0)
{
}

CameraPose::CameraPose(const CameraPose &pose) = default;

CameraPose::CameraPose(CameraPose &&pose) noexcept
  : mPosition(std::move(pose.mPosition)),
    mAccuracy(std::move(pose.mAccuracy)),
    mRotation(std::move(pose.mRotation)),
    mCrs(std::move(pose.mCrs)),
    mSource(std::move(pose.mSource)),
    mRtkFlag(pose.mRtkFlag)
{

}

CameraPose::CameraPose(double x, double y, double z,
                       const RotationMatrix<double> &rotationMatrix)
    : mPosition(x, y, z),
      mAccuracy({10., 10., 10.}),
      mRotation(new RotationMatrix<double>(rotationMatrix)),
      mCrs(""),
      mSource(""),
      mRtkFlag(0)
{

}

CameraPose::CameraPose(const Point3<double> &center,
                       const RotationMatrix<double> &rotationMatrix)
    : mPosition(center),
      mAccuracy({10., 10., 10.}),
      mRotation(new RotationMatrix<double>(rotationMatrix)),
      mCrs(""),
      mSource(""),
      mRtkFlag(0)
{
}

CameraPose::CameraPose(double x, double y, double z,
                       const Quaternion<double> &quaternion)
    : mPosition(x, y, z),
      mAccuracy({10., 10., 10.}),
      mRotation(new Quaternion<double>(quaternion)),
      mCrs(""),
      mSource(""),
      mRtkFlag(0)
{
}

CameraPose::CameraPose(const Point3<double> &center,
                       const Quaternion<double> &quaternion)
    : mPosition(center),
      mAccuracy({10., 10., 10.}),
      mRotation(new Quaternion<double>(quaternion)),
      mCrs(""),
      mSource(""),
      mRtkFlag(0)
{
}

CameraPose::~CameraPose() = default;

auto CameraPose::operator=(const CameraPose &pose) -> CameraPose &
{
    if (this != &pose) {
        mPosition = pose.mPosition;
        mAccuracy = pose.mAccuracy;
        mRotation = pose.mRotation;
        mCrs = pose.mCrs;
        mSource = pose.mSource;
        mRtkFlag = pose.mRtkFlag;
    }

    return *this;
}

auto CameraPose::operator=(CameraPose &&pose) noexcept -> CameraPose&
{
    if (this != &pose) {
        mPosition = std::move(pose.mPosition);
        mAccuracy = std::move(pose.mAccuracy);
        mRotation = std::move(pose.mRotation);
        mCrs = std::move(pose.mCrs);
        mSource = std::move(pose.mSource);
        mRtkFlag = pose.mRtkFlag;
    }

    return *this;
}

auto CameraPose::position() const -> Point3<double>
{
    return mPosition;
}

void CameraPose::setPosition(const Point3<double> &position)
{
    mPosition = position;
}

auto CameraPose::accuracy() const -> tl::Vector3d
{
    return mAccuracy;
}

void CameraPose::setAccuracy(const tl::Vector3d &accuracy)
{
    mAccuracy = accuracy;
}

auto CameraPose::quaternion() const -> Quaterniond
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

auto CameraPose::rotationMatrix() const -> RotationMatrix<double>
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

auto CameraPose::crs() const -> QString
{
    return mCrs;
}

void CameraPose::setCrs(const QString &crs)
{
    mCrs = crs;
}

auto CameraPose::rtkFlag() const -> int
{
    return mRtkFlag;
}

void CameraPose::setRtkFlag(int rtkFlag)
{
    mRtkFlag = rtkFlag;
}

auto CameraPose::source() const -> QString
{
    return mSource;
}

void CameraPose::setSource(const QString &source)
{
    mSource = source;
}

auto CameraPose::isEmpty() const -> bool
{
    return mPosition == Point3<double>();
}

} // namespace graphos
