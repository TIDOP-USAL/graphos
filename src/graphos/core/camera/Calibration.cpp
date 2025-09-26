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

#include "graphos/core/camera/Calibration.h"

//#include <stdexcept>

#include <tidop/core/exception.h>
#include <tidop/math/algebra/rotation_convert.h>

namespace graphos
{


class CalibrationRadial1 final
  : public Calibration
{

public:

    CalibrationRadial1()
        : Calibration(CameraModel::radial1)
    {
    }

    ~CalibrationRadial1() override = default;

    auto name() const -> std::string override
    {
        return std::string{"Radial 1"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(4);
        v[0] = this->parameter(Calibration::Parameters::focal);
        v[1] = this->parameter(Calibration::Parameters::cx);
        v[2] = this->parameter(Calibration::Parameters::cy);
        v[3] = this->parameter(Calibration::Parameters::k1);
        return v;
    }
};

class CalibrationRadial2 final
  : public Calibration
{

public:

    CalibrationRadial2()
        : Calibration(CameraModel::radial2)
    {
    }

    ~CalibrationRadial2() override = default;

    auto name() const -> std::string override
    {
        return {"Radial 2"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(5);
        v[0] = this->parameter(Calibration::Parameters::focal);
        v[1] = this->parameter(Calibration::Parameters::cx);
        v[2] = this->parameter(Calibration::Parameters::cy);
        v[3] = this->parameter(Calibration::Parameters::k1);
        v[4] = this->parameter(Calibration::Parameters::k2);
        return v;
    }
};

class CalibrationRadial3 final
  : public Calibration
{

public:

    CalibrationRadial3()
        : Calibration(CameraModel::radial3)
    {
    }

    ~CalibrationRadial3() override = default;

    auto name() const -> std::string override
    {
        return {"Radial 3"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(8);
        v[0] = this->parameter(Calibration::Parameters::focal);
        v[1] = this->parameter(Calibration::Parameters::cx);
        v[2] = this->parameter(Calibration::Parameters::cy);
        v[3] = this->parameter(Calibration::Parameters::k1);
        v[4] = this->parameter(Calibration::Parameters::k2);
        v[5] = this->parameter(Calibration::Parameters::k3);
        v[6] = this->parameter(Calibration::Parameters::p1);
        v[7] = this->parameter(Calibration::Parameters::p2);
        return v;
    }
};

class CalibrationSimpleRadialFisheye final
  : public Calibration
{

public:

    CalibrationSimpleRadialFisheye()
        : Calibration(CameraModel::simple_radial_fisheye)
    {
    }

    ~CalibrationSimpleRadialFisheye() override = default;

    auto name() const -> std::string override
    {
        return {"Radial Fisheye 1"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(4);
        v[0] = this->parameter(Calibration::Parameters::focal);
        v[1] = this->parameter(Calibration::Parameters::cx);
        v[2] = this->parameter(Calibration::Parameters::cy);
        v[3] = this->parameter(Calibration::Parameters::k1);
        return v;
    }
};


class CalibrationRadialFisheye final
  : public Calibration
{

public:

    CalibrationRadialFisheye()
        : Calibration(CameraModel::radial_fisheye)
    {
    }

    ~CalibrationRadialFisheye() override = default;

    auto name() const -> std::string override
    {
        return {"Radial Fisheye 2"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(5);
        v[0] = this->parameter(Calibration::Parameters::focal);
        v[1] = this->parameter(Calibration::Parameters::cx);
        v[2] = this->parameter(Calibration::Parameters::cy);
        v[3] = this->parameter(Calibration::Parameters::k1);
        v[4] = this->parameter(Calibration::Parameters::k2);
        return v;
    }
};


class CalibrationOpenCv final
  : public Calibration
{

public:

    CalibrationOpenCv()
        : Calibration(CameraModel::opencv)
    {
    }

    ~CalibrationOpenCv() override = default;

    auto name() const -> std::string override
    {
        return {"OpenCV 1"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(8);
        v[0] = this->parameter(Calibration::Parameters::focalx);
        v[1] = this->parameter(Calibration::Parameters::focaly);
        v[2] = this->parameter(Calibration::Parameters::cx);
        v[3] = this->parameter(Calibration::Parameters::cy);
        v[4] = this->parameter(Calibration::Parameters::k1);
        v[5] = this->parameter(Calibration::Parameters::k2);
        v[6] = this->parameter(Calibration::Parameters::p1);
        v[7] = this->parameter(Calibration::Parameters::p2);
        return v;
    }
};


class CalibrationOpenCvFisheye final
  : public Calibration
{

public:

    CalibrationOpenCvFisheye()
        : Calibration(CameraModel::opencv_fisheye)
    {
    }

    ~CalibrationOpenCvFisheye() override = default;

    auto name() const -> std::string override
    {
        return {"OpenCV Fisheye"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(8);
        v[0] = this->parameter(Calibration::Parameters::focalx);
        v[1] = this->parameter(Calibration::Parameters::focaly);
        v[2] = this->parameter(Calibration::Parameters::cx);
        v[3] = this->parameter(Calibration::Parameters::cy);
        v[4] = this->parameter(Calibration::Parameters::k1);
        v[5] = this->parameter(Calibration::Parameters::k2);
        v[6] = this->parameter(Calibration::Parameters::k3);
        v[7] = this->parameter(Calibration::Parameters::k4);
        return v;
    }
};


class CalibrationOpenCvFull final
  : public Calibration
{

public:

    CalibrationOpenCvFull()
        : Calibration(CameraModel::opencv_full)
    {
    }

    ~CalibrationOpenCvFull() override = default;

    auto name() const -> std::string override
    {
        return {"OpenCV 2"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(12);
        v[0] = this->parameter(Calibration::Parameters::focalx);
        v[1] = this->parameter(Calibration::Parameters::focaly);
        v[2] = this->parameter(Calibration::Parameters::cx);
        v[3] = this->parameter(Calibration::Parameters::cy);
        v[4] = this->parameter(Calibration::Parameters::k1);
        v[5] = this->parameter(Calibration::Parameters::k2);
        v[6] = this->parameter(Calibration::Parameters::p1);
        v[7] = this->parameter(Calibration::Parameters::p2);
        v[8] = this->parameter(Calibration::Parameters::k3);
        v[9] = this->parameter(Calibration::Parameters::k4);
        v[10] = this->parameter(Calibration::Parameters::k5);
        v[11] = this->parameter(Calibration::Parameters::k6);

        return v;
    }
};


class CalibrationSimplePinhole final
  : public Calibration
{

public:

    CalibrationSimplePinhole()
        : Calibration(CameraModel::simple_pinhole)
    {
    }

    ~CalibrationSimplePinhole() override = default;

    auto name() const -> std::string override
    {
        return {"Pinhole 1"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(3);
        v[0] = this->parameter(Calibration::Parameters::focal);
        v[1] = this->parameter(Calibration::Parameters::cx);
        v[2] = this->parameter(Calibration::Parameters::cy);
        return v;
    }
};



class CalibrationPinhole final
  : public Calibration
{

public:

    CalibrationPinhole()
        : Calibration(CameraModel::pinhole)
    {
    }

    ~CalibrationPinhole() override = default;

    auto name() const -> std::string override
    {
        return {"Pinhole 2"};
    }

    auto toVector() const -> std::vector<double> override
    {
        std::vector<double> v(4);
        v[0] = this->parameter(Calibration::Parameters::focalx);
        v[1] = this->parameter(Calibration::Parameters::focaly);
        v[2] = this->parameter(Calibration::Parameters::cx);
        v[3] = this->parameter(Calibration::Parameters::cy);
        return v;
    }

};



Calibration::Calibration(CameraModel cameraModel)
  : mCameraModel(cameraModel)
{

    if (mCameraModel.isEnabled(convertFlags(Parameters::focalx)) &&
        mCameraModel.isEnabled(convertFlags(Parameters::focaly))) {
        mParameters[Parameters::focalx] = 0.;
        mParameters[Parameters::focaly] = 0.;
    } else {
        mParameters[Parameters::focal] = 0.;
    }

    mParameters[Parameters::cx] = 0.;
    mParameters[Parameters::cy] = 0.;

    if (mCameraModel.isEnabled(convertFlags(Parameters::k1))) {
        mParameters[Parameters::k1] = 0.;
    }

    if (mCameraModel.isEnabled(convertFlags(Parameters::k2))) {
        mParameters[Parameters::k2] = 0.;
    }

    if (mCameraModel.isEnabled(convertFlags(Parameters::k3))) {
        mParameters[Parameters::k3] = 0.;
    }

    if (mCameraModel.isEnabled(convertFlags(Parameters::k4))) {
        mParameters[Parameters::k4] = 0.;
    }

    if (mCameraModel.isEnabled(convertFlags(Parameters::k5))) {
        mParameters[Parameters::k5] = 0.;
    }

    if (mCameraModel.isEnabled(convertFlags(Parameters::k6))) {
        mParameters[Parameters::k6] = 0.;
    }

    if (mCameraModel.isEnabled(convertFlags(Parameters::p1))) {
        mParameters[Parameters::p1] = 0.;
    }

    if (mCameraModel.isEnabled(convertFlags(Parameters::p2))) {
        mParameters[Parameters::p2] = 0.;
    }
}

Calibration::Calibration(const Calibration &calibration)
    : mCameraModel(calibration.mCameraModel),
      mParameters(calibration.mParameters)
{
}

Calibration::Calibration(Calibration &&calibration) TL_NOEXCEPT
    : mCameraModel(std::move(calibration.mCameraModel)),
      mParameters(std::move(calibration.mParameters))
{
}

auto Calibration::cameraModel() const -> CameraModel
{
    return mCameraModel.flags();
}

auto Calibration::cameraModelId() const -> int
{
    return static_cast<int>(mCameraModel.flags());
}

auto Calibration::operator =(const Calibration& calibration) -> Calibration&
{
    if (this != &calibration) {
        mCameraModel = calibration.mCameraModel;
        mParameters = calibration.mParameters;
    }
    return *this;
}

auto Calibration::convertFlags(Parameters parameter) -> CameraModel
{
    return static_cast<CameraModel>(static_cast<std::underlying_type_t<CameraModel>>(parameter));
}

auto Calibration::convertFlags(CameraType cameraType) -> CameraModel
{
    return static_cast<CameraModel>(static_cast<std::underlying_type_t<CameraModel>>(cameraType));
}

auto Calibration::operator =(Calibration&& calibration) TL_NOEXCEPT -> Calibration&
{
    if (this != &calibration) {
        mCameraModel = std::move(calibration.mCameraModel);
        mParameters = std::move(calibration.mParameters);
    }

    return *this;
}

auto Calibration::begin() -> iterator
{
    return mParameters.begin();
}

auto Calibration::begin() const -> const_iterator
{
    return mParameters.cbegin();
}

auto Calibration::end() -> iterator
{
    return mParameters.end();
}

auto Calibration::end() const -> const_iterator
{
    return mParameters.cend();
}

auto Calibration::name() const -> std::string
{
    return {};
}

auto Calibration::toVector() const -> std::vector<double>
{
    return {};
}

auto Calibration::parameterName(Parameters parameter) -> std::string
{
    std::string parameter_name;
    switch (parameter) {
    case Parameters::focal:
        parameter_name = "f";
        break;
    case Parameters::focalx:
        parameter_name = "fx";
        break;
    case Parameters::focaly:
        parameter_name = "fy";
        break;
    case Parameters::cx:
        parameter_name = "cx";
        break;
    case Parameters::cy:
        parameter_name = "cy";
        break;
    case Parameters::k1:
        parameter_name = "k1";
        break;
    case Parameters::k2:
        parameter_name = "k2";
        break;
    case Parameters::k3:
        parameter_name = "k3";
        break;
    case Parameters::k4:
        parameter_name = "k4";
        break;
    case Parameters::k5:
        parameter_name = "k5";
        break;
    case Parameters::k6:
        parameter_name = "k6";
        break;
    case Parameters::p1:
        parameter_name = "p1";
        break;
    case Parameters::p2:
        parameter_name = "p2";
        break;
    }
    return parameter_name;
}

auto Calibration::existParameter(Parameters parameter) const -> bool
{
    return mParameters.find(parameter) != mParameters.end();
}

auto Calibration::parameter(Parameters parameter) const -> double
{
    return this->existParameter(parameter) ? mParameters.at(parameter) : 0.0;
}

void Calibration::setParameter(Parameters parameter, double value)
{
    if (this->existParameter(parameter))
        mParameters.at(parameter) = value;
}

auto Calibration::checkCameraType(CameraType cameraType) const -> bool
{
    return mCameraModel.isEnabled(convertFlags(cameraType));
}




/* ------------------------------------------------------------------ */



auto CalibrationFactory::create(const std::string& cameraType) -> std::shared_ptr<Calibration>
{
    std::shared_ptr<Calibration> calibration;

    try {

        if (cameraType == "Pinhole 1") {
            calibration = std::make_shared<CalibrationSimplePinhole>();
        } else if (cameraType == "Pinhole 2") {
            calibration = std::make_shared<CalibrationPinhole>();
        } else if (cameraType == "Radial 1") {
            calibration = std::make_shared<CalibrationRadial1>();
        } else if (cameraType == "Radial 2") {
            calibration = std::make_shared<CalibrationRadial2>();
        } else if (cameraType == "OpenCV 1") {
            calibration = std::make_shared<CalibrationOpenCv>();
        } else if (cameraType == "OpenCV Fisheye") {
            calibration = std::make_shared<CalibrationOpenCvFisheye>();
        } else if (cameraType == "OpenCV 2") {
            calibration = std::make_shared<CalibrationOpenCvFull>();
        } else if (cameraType == "Radial Fisheye 1") {
            calibration = std::make_shared<CalibrationSimpleRadialFisheye>();
        } else if (cameraType == "Radial Fisheye 2") {
            calibration = std::make_shared<CalibrationRadialFisheye>();
        } else if (cameraType == "Radial 3") {
            calibration = std::make_shared<CalibrationRadial3>();
        } else {
            TL_THROW_EXCEPTION("Invalid Camera Type: '{}'.", cameraType);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return calibration;
}

auto CalibrationFactory::create(Calibration::CameraModel cameraModel) -> std::shared_ptr<Calibration>
{
    std::shared_ptr<Calibration> calibration;
    switch (cameraModel) {
    case Calibration::CameraModel::radial1:
        calibration = std::make_shared<CalibrationRadial1>();
        break;
    case Calibration::CameraModel::radial2:
        calibration = std::make_shared<CalibrationRadial2>();
        break;
    case Calibration::CameraModel::radial3:
        calibration = std::make_shared<CalibrationRadial3>();
        break;
    case Calibration::CameraModel::simple_radial_fisheye:
        calibration = std::make_shared<CalibrationSimpleRadialFisheye>();
        break;
    case Calibration::CameraModel::radial_fisheye:
        calibration = std::make_shared<CalibrationRadialFisheye>();
        break;
    case Calibration::CameraModel::opencv:
        calibration = std::make_shared<CalibrationOpenCv>();
        break;
    case Calibration::CameraModel::opencv_fisheye:
        calibration = std::make_shared<CalibrationOpenCvFisheye>();
        break;
    case Calibration::CameraModel::opencv_full:
        calibration = std::make_shared<CalibrationOpenCvFull>();
        break;
    case Calibration::CameraModel::simple_pinhole:
        calibration = std::make_shared<CalibrationSimplePinhole>();
        break;
    case Calibration::CameraModel::pinhole:
        calibration = std::make_shared<CalibrationPinhole>();
        break;
    }
    return calibration;
}


} // namespace graphos
