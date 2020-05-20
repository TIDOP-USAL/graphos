#include "camera.h"

namespace inspector
{



/// Cámaras predefinidas

class CalibrationRadial1
  : public Calibration
{

public:

  CalibrationRadial1()
    : Calibration(Calibration::CameraModel::radial1){}
  virtual ~CalibrationRadial1() override {}

  std::string name() const override {
    return std::string("Simple radial");
  }

};

class CalibrationRadial2
  : public Calibration
{

public:

  CalibrationRadial2()
    : Calibration(Calibration::CameraModel::radial2){}
  virtual ~CalibrationRadial2() override {}

  std::string name() const override {
    return std::string("Radial");
  }

};

class CalibrationRadial3
  : public Calibration
{

public:

  CalibrationRadial3()
    : Calibration(Calibration::CameraModel::radial3){}
  virtual ~CalibrationRadial3() override {}

  std::string name() const override {
    return std::string("Full radial");
  }

};

class CalibrationSimpleRadialFisheye
  : public Calibration
{

public:

  CalibrationSimpleRadialFisheye()
    : Calibration(Calibration::CameraModel::simple_radial_fisheye){}
  virtual ~CalibrationSimpleRadialFisheye() override {}

  std::string name() const override {
    return std::string("Simple Radial Fisheye");
  }

};


class CalibrationRadialFisheye
  : public Calibration
{

public:

  CalibrationRadialFisheye()
    : Calibration(Calibration::CameraModel::radial_fisheye){}
  virtual ~CalibrationRadialFisheye() override {}

  std::string name() const override {
    return std::string("Radial Fisheye");
  }

};


class CalibrationOpenCV
  : public Calibration
{

public:

  CalibrationOpenCV()
    : Calibration(Calibration::CameraModel::opencv){}
  virtual ~CalibrationOpenCV() override {}

  std::string name() const override {
    return std::string("OpenCV");
  }

};


class CalibrationOpenCVFisheye
  : public Calibration
{

public:

  CalibrationOpenCVFisheye()
    : Calibration(Calibration::CameraModel::opencv_fisheye){}
  virtual ~CalibrationOpenCVFisheye() override {}

  std::string name() const override {
    return std::string("OpenCV Fisheye");
  }

};


class CalibrationOpenCVFull
  : public Calibration
{

public:

  CalibrationOpenCVFull()
    : Calibration(Calibration::CameraModel::opencv_full){}
  virtual ~CalibrationOpenCVFull() override {}

  std::string name() const override {
    return std::string("OpenCV Full");
  }

};


class CalibrationSimplePinhole
  : public Calibration
{

public:

  CalibrationSimplePinhole()
    : Calibration(Calibration::CameraModel::simple_pinhole){}
  virtual ~CalibrationSimplePinhole() override {}

  std::string name() const override {
    return std::string("Simple Pinhole");
  }

};



class CalibrationPinhole
  : public Calibration
{

public:

  CalibrationPinhole()
    : Calibration(Calibration::CameraModel::pinhole){}
  virtual ~CalibrationPinhole() override {}

  std::string name() const override {
    return std::string("Pinhole");
  }

};



Calibration::Calibration(CameraModel cameraModel)
  : mCameraModel(cameraModel)
{

  if (mCameraModel.isActive(convertFlags(Parameters::focalx)) &&
      mCameraModel.isActive(convertFlags(Parameters::focaly))){
    mParameters[Parameters::focalx] = 0.;
    mParameters[Parameters::focaly] = 0.;
  } else {
    mParameters[Parameters::focal] = 0.;
  }

  mParameters[Parameters::cx] = 0.;
  mParameters[Parameters::cy] = 0.;

  if (mCameraModel.isActive(convertFlags(Parameters::k1))) {
    mParameters[Parameters::k1] = 0.;
  }

  if (mCameraModel.isActive(convertFlags(Parameters::k2))) {
    mParameters[Parameters::k2] = 0.;
  }

  if (mCameraModel.isActive(convertFlags(Parameters::k3))) {
    mParameters[Parameters::k3] = 0.;
  }

  if (mCameraModel.isActive(convertFlags(Parameters::k4))) {
    mParameters[Parameters::k4] = 0.;
  }

  if (mCameraModel.isActive(convertFlags(Parameters::k5))) {
    mParameters[Parameters::k5] = 0.;
  }

  if (mCameraModel.isActive(convertFlags(Parameters::k6))) {
    mParameters[Parameters::k6] = 0.;
  }

  if (mCameraModel.isActive(convertFlags(Parameters::p1))) {
    mParameters[Parameters::p1] = 0.;
  }

  if (mCameraModel.isActive(convertFlags(Parameters::p2))) {
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

Calibration *Calibration::create(CameraModel cameraModel)
{
  Calibration *calib;
  switch (cameraModel)
  {
  case Calibration::CameraModel::radial1:
    calib = new CalibrationRadial1();
    break;
  case Calibration::CameraModel::radial2:
    calib = new CalibrationRadial2();
    break;
  case Calibration::CameraModel::radial3:
    calib = new CalibrationRadial3();
    break;
  case Calibration::CameraModel::simple_radial_fisheye:
    calib = new CalibrationSimpleRadialFisheye();
    break;
  case Calibration::CameraModel::radial_fisheye:
    calib = new CalibrationRadialFisheye();
    break;
  case Calibration::CameraModel::opencv:
    calib = new CalibrationOpenCV();
    break;
  case Calibration::CameraModel::opencv_fisheye:
    calib = new CalibrationOpenCVFisheye();
    break;
  case Calibration::CameraModel::opencv_full:
    calib = new CalibrationOpenCVFull();
    break;
  case Calibration::CameraModel::simple_pinhole:
    calib = new CalibrationSimplePinhole();
    break;
  case Calibration::CameraModel::pinhole:
    calib = new CalibrationPinhole();
    break;
  }
  return calib;
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

Calibration::CameraModel Calibration::convertFlags(Parameters parameter)
{
  return static_cast<CameraModel>(static_cast<std::underlying_type<CameraModel>::type>(parameter));
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

//Calibration::parameter_iterator Calibration::find(Parameters parameter)
//{
//  return mParameters.find(parameter);
//}

//Calibration::parameter_const_iterator Calibration::find(Parameters parameter) const
//{
//  return parameter_const_iterator();
//}

std::string Calibration::parameterName(Parameters parameter) const
{
  switch (parameter)
  {
  case Calibration::Parameters::focal:
    return std::string("f");
  case Calibration::Parameters::focalx:
    return std::string("fx");
  case Calibration::Parameters::focaly:
    return std::string("fy");
  case Calibration::Parameters::cx:
    return std::string("cx");
  case Calibration::Parameters::cy:
    return std::string("cy");
  case Calibration::Parameters::k1:
    return std::string("k1");
  case Calibration::Parameters::k2:
    return std::string("k2");
  case Calibration::Parameters::k3:
    return std::string("k3");
  case Calibration::Parameters::k4:
    return std::string("k4");
  case Calibration::Parameters::k5:
    return std::string("k5");
  case Calibration::Parameters::k6:
    return std::string("k6");
  case Calibration::Parameters::p1:
    return std::string("p1");
  case Calibration::Parameters::p2:
    return std::string("p2");
    }
}

double Calibration::parameter(Calibration::Parameters parameter) const
{
  return mParameters.at(parameter);
}

void Calibration::setParameter(Calibration::Parameters parameter, double value)
{
  mParameters.at(parameter) = value;
}


/* ------------------------------------------------------------------ */



Camera::Camera()
    : mMake(""),
      mModel(""),
      mType("RADIAL"),
      mFocal(1.),
      mWidth(0),
      mHeight(0),
      mSensorSize(1.)
{
  init();
}

Camera::Camera(const QString &make, const QString &model)
    : mMake(make),
      mModel(model),
      mType("RADIAL"),
      mFocal(1.),
      mWidth(0),
      mHeight(0),
      mSensorSize(1.)
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
    mSensorSize(camera.mSensorSize)
{
}

QString Camera::make() const
{
  return mMake;
}

void Camera::setMake(const QString &make)
{
  mMake = make;
}

QString Camera::model() const
{
  return mModel;
}

void Camera::setModel(const QString &model)
{
  mModel = model;
}

QString Camera::type() const
{
  return mType;
}

void Camera::setType(const QString &type)
{
  mType = type;
}

double Camera::focal() const
{
  return mFocal;
}

void Camera::setFocal(double focal)
{
  mFocal = focal;
}

int Camera::width() const
{
  return mWidth;
}

void Camera::setWidth(int width)
{
  mWidth = width;
}

int Camera::height() const
{
  return mHeight;
}

void Camera::setHeight(int height)
{
  mHeight = height;
}

double Camera::sensorSize() const
{
    return mSensorSize;
}

void Camera::setSensorSize(double sensorSize)
{
    mSensorSize = sensorSize;
}

Camera &Camera::operator =(const Camera &camera)
{
  if (this != &camera){
    this->mMake = camera.mMake;
    this->mModel = camera.mModel;
    this->mType = camera.mType;
    this->mFocal = camera.mFocal;
    this->mWidth = camera.mWidth;
    this->mHeight = camera.mHeight;
    this->mSensorSize = camera.mSensorSize;
  }
  return *this;
}

void Camera::init()
{
}

} // namespace inspector
