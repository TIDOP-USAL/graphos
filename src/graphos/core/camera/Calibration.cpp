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


class CalibrationRadial1
    : public Calibration
{

public:

    CalibrationRadial1()
        : Calibration(Calibration::CameraModel::radial1)
    {
    }
    virtual ~CalibrationRadial1() override {}

    std::string name() const override
    {
        return std::string("Radial 1");
    }

};

class CalibrationRadial2
    : public Calibration
{

public:

    CalibrationRadial2()
        : Calibration(Calibration::CameraModel::radial2)
    {
    }
    virtual ~CalibrationRadial2() override {}

    std::string name() const override
    {
        return std::string("Radial 2");
    }

};

class CalibrationRadial3
    : public Calibration
{

public:

    CalibrationRadial3()
        : Calibration(Calibration::CameraModel::radial3)
    {
    }
    virtual ~CalibrationRadial3() override {}

    std::string name() const override
    {
        return std::string("Radial 3");
    }

};

class CalibrationSimpleRadialFisheye
    : public Calibration
{

public:

    CalibrationSimpleRadialFisheye()
        : Calibration(Calibration::CameraModel::simple_radial_fisheye)
    {
    }
    virtual ~CalibrationSimpleRadialFisheye() override {}

    std::string name() const override
    {
        return std::string("Radial Fisheye 1");
    }

};


class CalibrationRadialFisheye
    : public Calibration
{

public:

    CalibrationRadialFisheye()
        : Calibration(Calibration::CameraModel::radial_fisheye)
    {
    }
    virtual ~CalibrationRadialFisheye() override {}

    std::string name() const override
    {
        return std::string("Radial Fisheye 2");
    }

};


class CalibrationOpenCV
    : public Calibration
{

public:

    CalibrationOpenCV()
        : Calibration(Calibration::CameraModel::opencv)
    {
    }
    virtual ~CalibrationOpenCV() override {}

    std::string name() const override
    {
        return std::string("OpenCV 1");
    }

};


class CalibrationOpenCVFisheye
    : public Calibration
{

public:

    CalibrationOpenCVFisheye()
        : Calibration(Calibration::CameraModel::opencv_fisheye)
    {
    }
    virtual ~CalibrationOpenCVFisheye() override {}

    std::string name() const override
    {
        return std::string("OpenCV Fisheye");
    }

};


class CalibrationOpenCVFull
    : public Calibration
{

public:

    CalibrationOpenCVFull()
        : Calibration(Calibration::CameraModel::opencv_full)
    {
    }
    virtual ~CalibrationOpenCVFull() override {}

    std::string name() const override
    {
        return std::string("OpenCV 2");
    }

};


class CalibrationSimplePinhole
    : public Calibration
{

public:

    CalibrationSimplePinhole()
        : Calibration(Calibration::CameraModel::simple_pinhole)
    {
    }
    virtual ~CalibrationSimplePinhole() override {}

    std::string name() const override
    {
        return std::string("Pinhole 1");
    }

};



class CalibrationPinhole
    : public Calibration
{

public:

    CalibrationPinhole()
        : Calibration(Calibration::CameraModel::pinhole)
    {
    }
    virtual ~CalibrationPinhole() override {}

    std::string name() const override
    {
        return std::string("Pinhole 2");
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

Calibration::CameraModel Calibration::cameraModel() const
{
    return mCameraModel.flags();
}

int Calibration::cameraModelId() const
{
    return static_cast<int>(mCameraModel.flags());
}

Calibration &Calibration::operator = (const Calibration &calibration)
{
    if (this != &calibration) {
        mCameraModel = calibration.mCameraModel;
        mParameters = calibration.mParameters;
    }
    return *this;
}

Calibration::CameraModel Calibration::convertFlags(Parameters parameter) const
{
    return static_cast<CameraModel>(static_cast<std::underlying_type<CameraModel>::type>(parameter));
}

Calibration::CameraModel Calibration::convertFlags(CameraType cameraType) const
{
    return static_cast<CameraModel>(static_cast<std::underlying_type<CameraModel>::type>(cameraType));
}

Calibration &Calibration::operator = (Calibration &&calibration) TL_NOEXCEPT
{
    if (this != &calibration) {
        mCameraModel = std::move(calibration.mCameraModel);
        mParameters = std::move(calibration.mParameters);
    }

    return *this;
}

Calibration::parameter_iterator Calibration::parametersBegin()
{
    return mParameters.begin();
}

Calibration::parameter_const_iterator Calibration::parametersBegin() const
{
    return mParameters.cbegin();
}

Calibration::parameter_iterator Calibration::parametersEnd()
{
    return mParameters.end();
}

Calibration::parameter_const_iterator Calibration::parametersEnd() const
{
    return mParameters.cend();
}

std::string Calibration::parameterName(Parameters parameter) const
{
    std::string parameter_name;
    switch (parameter) {
    case Calibration::Parameters::focal:
        parameter_name = "f";
        break;
    case Calibration::Parameters::focalx:
        parameter_name = "fx";
        break;
    case Calibration::Parameters::focaly:
        parameter_name = "fy";
        break;
    case Calibration::Parameters::cx:
        parameter_name = "cx";
        break;
    case Calibration::Parameters::cy:
        parameter_name = "cy";
        break;
    case Calibration::Parameters::k1:
        parameter_name = "k1";
        break;
    case Calibration::Parameters::k2:
        parameter_name = "k2";
        break;
    case Calibration::Parameters::k3:
        parameter_name = "k3";
        break;
    case Calibration::Parameters::k4:
        parameter_name = "k4";
        break;
    case Calibration::Parameters::k5:
        parameter_name = "k5";
        break;
    case Calibration::Parameters::k6:
        parameter_name = "k6";
        break;
    case Calibration::Parameters::p1:
        parameter_name = "p1";
        break;
    case Calibration::Parameters::p2:
        parameter_name = "p2";
        break;
    }
    return parameter_name;
}

bool Calibration::existParameter(Calibration::Parameters parameter) const
{
    return mParameters.find(parameter) != mParameters.end();
}

double Calibration::parameter(Calibration::Parameters parameter) const
{
    return this->existParameter(parameter) ? mParameters.at(parameter) : 0.0;
}

void Calibration::setParameter(Calibration::Parameters parameter, double value)
{
    if (this->existParameter(parameter))
        mParameters.at(parameter) = value;
}

bool Calibration::checkCameraType(CameraType camera_type) const
{
    return mCameraModel.isEnabled(convertFlags(camera_type));
}




/* ------------------------------------------------------------------ */



std::shared_ptr<Calibration> CalibrationFactory::create(const std::string &cameraType)
{
    std::shared_ptr<Calibration> calibration;

    try {

        if (cameraType.compare("Pinhole 1") == 0) {
            calibration = std::make_shared<CalibrationSimplePinhole>();
        } else if (cameraType.compare("Pinhole 2") == 0) {
            calibration = std::make_shared<CalibrationPinhole>();
        } else if (cameraType.compare("Radial 1") == 0) {
            calibration = std::make_shared<CalibrationRadial1>();
        } else if (cameraType.compare("Radial 2") == 0) {
            calibration = std::make_shared<CalibrationRadial2>();
        } else if (cameraType.compare("OpenCV 1") == 0) {
            calibration = std::make_shared<CalibrationOpenCV>();
        } else if (cameraType.compare("OpenCV Fisheye") == 0) {
            calibration = std::make_shared<CalibrationOpenCVFisheye>();
        } else if (cameraType.compare("OpenCV 2") == 0) {
            calibration = std::make_shared<CalibrationOpenCVFull>();
        } else if (cameraType.compare("Radial Fisheye 1") == 0) {
            calibration = std::make_shared<CalibrationSimpleRadialFisheye>();
        } else if (cameraType.compare("Radial Fisheye 2") == 0) {
            calibration = std::make_shared<CalibrationRadialFisheye>();
        } else if (cameraType.compare("Radial 3") == 0) {
            calibration = std::make_shared<CalibrationRadial3>();
        } else {
            TL_THROW_EXCEPTION("Invalid Camera Type: '{}'.", cameraType);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return calibration;
}

std::shared_ptr<Calibration> CalibrationFactory::create(Calibration::CameraModel cameraModel)
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
        calibration = std::make_shared<CalibrationOpenCV>();
        break;
    case Calibration::CameraModel::opencv_fisheye:
        calibration = std::make_shared<CalibrationOpenCVFisheye>();
        break;
    case Calibration::CameraModel::opencv_full:
        calibration = std::make_shared<CalibrationOpenCVFull>();
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



/* Calibration reader */

CalibrationReader::CalibrationReader()
{
}

std::unordered_map<size_t, std::shared_ptr<Calibration>> CalibrationReader::calibrations()
{
    return mCalibrations;
}


/* Calibration reader factory */

std::unique_ptr<CalibrationReader> CalibrationReaderFactory::create(const std::string &format)
{
    return std::unique_ptr<CalibrationReader>();
}


/* Calibration writer */

CalibrationWriter::CalibrationWriter()
{
}

void CalibrationWriter::setCalibrations(const std::unordered_map<size_t, std::shared_ptr<Calibration>> &calibrations)
{
    mCalibrations = calibrations;
}


/* GraphosXMLCalibrationWriter */

class GraphosXMLCalibrationWriter
    : public CalibrationWriter
{

public:

    GraphosXMLCalibrationWriter()
    {
    }

    ~GraphosXMLCalibrationWriter()
    {
    }

private:


    // CalibrationWriter

    void write(const tl::Path &path) override
    {
    }

    virtual std::string format() const override
    {
        return std::string("GRAPHOS_XML");
    }

};


/* Camera Poses Writer Factory */

std::unique_ptr<CalibrationWriter> CalibrationWriterFactory::create(const std::string &format)
{
    std::unique_ptr<CalibrationWriter> writer;

    try {

        if (format == "GRAPHOS_XML") {
            writer = std::make_unique<GraphosXMLCalibrationWriter>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format.c_str());
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return writer;
}

} // namespace graphos
