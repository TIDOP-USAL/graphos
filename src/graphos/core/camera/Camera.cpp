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


#include "graphos/core/camera/Camera.h"

namespace graphos
{

Camera::Camera()
    : mType("OpenCV 1"),
      mFocal(1.),
      mWidth(0),
      mHeight(0),
      mSensorSize(1.),
      mCalibration(nullptr)
{
    init();
}

Camera::Camera(std::string make, std::string model)
    : mMake(std::move(make)),
      mModel(std::move(model)),
      mType("OpenCV 1"),
      mFocal(1.),
      mWidth(0),
      mHeight(0),
      mSensorSize(1.),
      mCalibration(nullptr)
{
    init();
}

Camera::Camera(const Camera &camera)
    : mMake(camera.mMake),
      mModel(camera.mModel),
      mType(camera.mType),
      mFocal(camera.mFocal),
      mWidth(camera.mWidth),
      mHeight(camera.mHeight),
      mSensorSize(camera.mSensorSize),
      mCalibration(camera.mCalibration)
{
}

auto Camera::make() const -> std::string
{
    return mMake;
}

void Camera::setMake(const std::string &make)
{
    mMake = make;
}

auto Camera::model() const -> std::string
{
    return mModel;
}

void Camera::setModel(const std::string &model)
{
    mModel = model;
}

auto Camera::type() const -> std::string
{
    return mType;
}

void Camera::setType(const std::string &type)
{
    mType = type;
}

auto Camera::focal() const -> double
{
    return mFocal;
}

void Camera::setFocal(double focal)
{
    mFocal = focal;
}

auto Camera::width() const -> int
{
    return mWidth;
}

void Camera::setWidth(int width)
{
    mWidth = width;
}

auto Camera::height() const -> int
{
    return mHeight;
}

void Camera::setHeight(int height)
{
    mHeight = height;
}

auto Camera::sensorSize() const -> double
{
    return mSensorSize;
}

void Camera::setSensorSize(double sensorSize)
{
    mSensorSize = sensorSize;
}

auto Camera::calibration() const -> std::shared_ptr<Calibration>
{
    return mCalibration;
}

void Camera::setCalibration(std::shared_ptr<Calibration> &calibration)
{
    mCalibration = calibration;
}

auto Camera::operator =(const Camera& camera) -> Camera&
{
    if (this != &camera) {
        this->mMake = camera.mMake;
        this->mModel = camera.mModel;
        this->mType = camera.mType;
        this->mFocal = camera.mFocal;
        this->mWidth = camera.mWidth;
        this->mHeight = camera.mHeight;
        this->mSensorSize = camera.mSensorSize;
        this->mCalibration = camera.mCalibration;
    }
    return *this;
}

void Camera::init()
{
}

} // namespace graphos
