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

//Camera::Camera()
//  : mType("OpenCV 1"),
//    mFocal(1.),
//    mWidth(0),
//    mHeight(0),
//    mBitsPerPixel(-1),
//    mSensorSize(1.),
//    mBandName("RGB"),
//    mBlackLevel(0),
//    mCalibratedHMatrix(tl::Matrix3x3f::identity()),
//    mCalibration(nullptr),
//    mPriorCalibration(nullptr),
//    mVignetting(nullptr)
//{
//}

Camera::Camera(std::string make, 
               std::string model, 
               std::string serialNumber, 
               std::string bandName)
  : mMake(std::move(make)),
    mModel(std::move(model)),
    mSerialNumber(std::move(serialNumber)),
    mType("OpenCV 1"),
    mFocal(1.),
    mWidth(0),
    mHeight(0),
    mBitsPerPixel(8),
    mSensorSize(1.),
    mBandName(bandName),
    mBlackLevel(0),
    mCalibratedHMatrix(tl::Matrix3x3f::identity()),
    mCalibration(nullptr),
    mPriorCalibration(nullptr),
    mVignetting(nullptr)
{
}

//Camera::Camera(const Camera &camera)
//  : mMake(camera.mMake),
//    mModel(camera.mModel),
//    mSerialNumber(camera.mSerialNumber),
//    mType(camera.mType),
//    mFocal(camera.mFocal),
//    mWidth(camera.mWidth),
//    mHeight(camera.mHeight),
//    mBitsPerPixel(camera.mBitsPerPixel),
//    mSensorSize(camera.mSensorSize),
//    mBandName(camera.mBandName),
//    mBlackLevel(camera.mBlackLevel),
//    mVignetteCenter(camera.mVignetteCenter),
//    mVignettePolynomial(camera.mVignettePolynomial),
//    mCalibratedHMatrix(camera.mCalibratedHMatrix),
//    mCalibration(camera.mCalibration),
//    mPriorCalibration(camera.mPriorCalibration),
//    mVignetting(camera.mVignetting)
//{
//}

auto Camera::make() const -> const std::string &
{
    return mMake;
}

void Camera::setMake(std::string make)
{
    mMake = std::move(make);
}

auto Camera::model() const -> const std::string &
{
    return mModel;
}

void Camera::setModel(std::string model)
{
    mModel = std::move(model);
}

auto Camera::serialNumber() const -> const std::string &
{
    return mSerialNumber;
}

void Camera::setSerialNumber(std::string serialNumber)
{
    mSerialNumber = std::move(serialNumber);
}

auto Camera::type() const -> const std::string &
{
    return mType;
}

void Camera::setType(std::string type)
{
    mType = std::move(type);
}

auto Camera::bandName() const -> const std::string &
{
    return mBandName;
}

void Camera::setBandName(std::string bandName)
{
    mBandName = std::move(bandName);
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

auto Camera::bitsPerPixel() const -> int
{
    return mBitsPerPixel;
}

void Camera::setBitsPerPixel(int bitsPerPixel)
{
    mBitsPerPixel = bitsPerPixel;
}

auto Camera::sensorSize() const -> double
{
    return mSensorSize;
}

void Camera::setSensorSize(double sensorSize)
{
    mSensorSize = sensorSize;
}

auto Camera::blackLevel() const -> std::optional<uint16_t>
{
    return mBlackLevel;
}

void Camera::setBlackLevel(uint16_t blackLevel)
{
    mBlackLevel = blackLevel;
}

void Camera::setVignettingModel(std::shared_ptr<Vignetting> model)
{
    mVignetting = std::move(model);
}

auto Camera::vignettingModel() const -> std::shared_ptr<Vignetting>
{
    return mVignetting;
}

auto Camera::calibration() const -> std::shared_ptr<Calibration>
{
    return mCalibration;
}

void Camera::setCalibration(std::shared_ptr<Calibration> calibration)
{
    mCalibration = std::move(calibration);
}

auto Camera::priorCalibration() const -> std::shared_ptr<Calibration>
{
    return mPriorCalibration;
}

void Camera::setPriorCalibration(std::shared_ptr<Calibration> calibration)
{
    mPriorCalibration = std::move(calibration);
}

auto Camera::calibratedHMatrix() const -> const std::optional<tl::Matrix3x3f> &
{
    return mCalibratedHMatrix;
}

void Camera::setCalibratedHMatrix(const tl::Matrix3x3f &hMatrix)
{
    mCalibratedHMatrix = hMatrix;
}

//auto Camera::hasCalibratedHMatrix() const -> bool
//{
//    return mCalibratedHMatrix != tl::Matrix3x3f::identity();
//}

auto Camera::signature() const -> std::string
{
    return mMake + "|" + mModel + "|" + mSerialNumber + "|" + mBandName;
}

//auto Camera::operator =(const Camera& camera) -> Camera&
//{
//    if (this != &camera) {
//        this->mMake = camera.mMake;
//        this->mModel = camera.mModel;
//        this->mSerialNumber = camera.mSerialNumber;
//        this->mType = camera.mType;
//        this->mFocal = camera.mFocal;
//        this->mWidth = camera.mWidth;
//        this->mHeight = camera.mHeight;
//        this->mBitsPerPixel = camera.mBitsPerPixel;
//        this->mSensorSize = camera.mSensorSize;
//        this->mBandName = camera.mBandName;
//        this->mBlackLevel = camera.mBlackLevel;
//        this->mVignetteCenter = camera.mVignetteCenter;
//        this->mVignettePolynomial = camera.mVignettePolynomial;
//        this->mCalibratedHMatrix = camera.mCalibratedHMatrix;
//        this->mCalibration = camera.mCalibration;
//        this->mPriorCalibration = camera.mPriorCalibration;
//        this->mVignetting = camera.mVignetting;
//    }
//
//    return *this;
//}

} // namespace graphos
