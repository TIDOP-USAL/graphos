#include "camera.h"

#include <colmap/base/reconstruction.h>


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

  QString name() const override {
    return QString("Simple radial");
  }

};

class CalibrationRadial2
  : public Calibration
{

public:

  CalibrationRadial2()
    : Calibration(Calibration::CameraModel::radial2){}
  virtual ~CalibrationRadial2() override {}

  QString name() const override {
    return QString("Radial");
  }

};

class CalibrationRadial3
  : public Calibration
{

public:

  CalibrationRadial3()
    : Calibration(Calibration::CameraModel::radial3){}
  virtual ~CalibrationRadial3() override {}

  QString name() const override {
    return QString("Full radial");
  }

};

class CalibrationSimpleRadialFisheye
  : public Calibration
{

public:

  CalibrationSimpleRadialFisheye()
    : Calibration(Calibration::CameraModel::simple_radial_fisheye){}
  virtual ~CalibrationSimpleRadialFisheye() override {}

  QString name() const override {
    return QString("Simple Radial Fisheye");
  }

};


class CalibrationRadialFisheye
  : public Calibration
{

public:

  CalibrationRadialFisheye()
    : Calibration(Calibration::CameraModel::radial_fisheye){}
  virtual ~CalibrationRadialFisheye() override {}

  QString name() const override {
    return QString("Radial Fisheye");
  }

};


class CalibrationOpenCV
  : public Calibration
{

public:

  CalibrationOpenCV()
    : Calibration(Calibration::CameraModel::opencv){}
  virtual ~CalibrationOpenCV() override {}

  QString name() const override {
    return QString("OpenCV");
  }

};


class CalibrationOpenCVFisheye
  : public Calibration
{

public:

  CalibrationOpenCVFisheye()
    : Calibration(Calibration::CameraModel::opencv_fisheye){}
  virtual ~CalibrationOpenCVFisheye() override {}

  QString name() const override {
    return QString("OpenCV Fisheye");
  }

};


class CalibrationOpenCVFull
  : public Calibration
{

public:

  CalibrationOpenCVFull()
    : Calibration(Calibration::CameraModel::opencv_full){}
  virtual ~CalibrationOpenCVFull() override {}

  QString name() const override {
    return QString("OpenCV Full");
  }

};


class CalibrationSimplePinhole
  : public Calibration
{

public:

  CalibrationSimplePinhole()
    : Calibration(Calibration::CameraModel::simple_pinhole){}
  virtual ~CalibrationSimplePinhole() override {}

  QString name() const override {
    return QString("Simple Pinhole");
  }

};



class CalibrationPinhole
  : public Calibration
{

public:

  CalibrationPinhole()
    : Calibration(Calibration::CameraModel::pinhole){}
  virtual ~CalibrationPinhole() override {}

  QString name() const override {
    return QString("Pinhole");
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

std::shared_ptr<Calibration> Calibration::create(CameraModel cameraModel)
{
  std::shared_ptr<Calibration> calib;
  switch (cameraModel)
  {
  case Calibration::CameraModel::radial1:
    calib = std::make_shared<CalibrationRadial1>();
    break;
  case Calibration::CameraModel::radial2:
    calib =std::make_shared<CalibrationRadial2>();
    break;
  case Calibration::CameraModel::radial3:
    calib = std::make_shared<CalibrationRadial3>();
    break;
  case Calibration::CameraModel::simple_radial_fisheye:
    calib = std::make_shared<CalibrationSimpleRadialFisheye>();
    break;
  case Calibration::CameraModel::radial_fisheye:
    calib = std::make_shared<CalibrationRadialFisheye>();
    break;
  case Calibration::CameraModel::opencv:
    calib = std::make_shared<CalibrationOpenCV>();
    break;
  case Calibration::CameraModel::opencv_fisheye:
    calib = std::make_shared<CalibrationOpenCVFisheye>();
    break;
  case Calibration::CameraModel::opencv_full:
    calib = std::make_shared<CalibrationOpenCVFull>();
    break;
  case Calibration::CameraModel::simple_pinhole:
    calib = std::make_shared<CalibrationSimplePinhole>();
    break;
  case Calibration::CameraModel::pinhole:
    calib = std::make_shared<CalibrationPinhole>();
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

QString Calibration::parameterName(Parameters parameter) const
{
  QString parameter_name;
  switch (parameter)
  {
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



/* ------------------------------------------------------------------ */


std::shared_ptr<Calibration> CalibrationFactory::create(const QString &cameraType)
{
  std::shared_ptr<Calibration> calibration;

  if (cameraType.compare("Simple Pinhole") == 0){
    calibration = Calibration::create(Calibration::CameraModel::simple_pinhole);
  } else if (cameraType.compare("Pinhole") == 0){
    calibration = Calibration::create(Calibration::CameraModel::pinhole);
  } else if (cameraType.compare("Simple radial") == 0){
    calibration = Calibration::create(Calibration::CameraModel::radial1);
  } else if (cameraType.compare("Radial") == 0){
    calibration = Calibration::create(Calibration::CameraModel::radial2);
  } else if (cameraType.compare("OpenCV") == 0){
    calibration = Calibration::create(Calibration::CameraModel::opencv);
  } else if (cameraType.compare("OpenCV Fisheye") == 0){
    calibration = Calibration::create(Calibration::CameraModel::opencv_fisheye);
  } else if (cameraType.compare("OpenCV Full") == 0){
    calibration = Calibration::create(Calibration::CameraModel::opencv_full);
  } else if (cameraType.compare("Simple Radial Fisheye") == 0){
    calibration = Calibration::create(Calibration::CameraModel::simple_radial_fisheye);
  } else if (cameraType.compare("Radial Fisheye") == 0){
    calibration = Calibration::create(Calibration::CameraModel::radial_fisheye);
  } else if (cameraType.compare("Full radial") == 0){
    calibration = Calibration::create(Calibration::CameraModel::radial3);
  } else {
    throw std::runtime_error("Invalid Camera Type");
  }

  return calibration;
}


/* ------------------------------------------------------------------ */




Camera::Camera()
    : mMake(""),
      mModel(""),
      mType("Radial"),
      mFocal(1.),
      mWidth(0),
      mHeight(0),
      mSensorSize(1.),
      mCalibration(nullptr)
{
  init();
}

Camera::Camera(const QString &make, const QString &model)
    : mMake(make),
      mModel(model),
      mType("Radial"),
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

std::shared_ptr<Calibration> Camera::calibration() const
{
  return mCalibration;
}

void Camera::setCalibration(std::shared_ptr<Calibration> &calibration)
{
  mCalibration = calibration;
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
    this->mCalibration = camera.mCalibration;
  }
  return *this;
}

void Camera::init()
{
}





ReadCalibration::ReadCalibration()
  : mReconstruction(new colmap::Reconstruction)
{

}

ReadCalibration::~ReadCalibration()
{
  if (mReconstruction){
    delete mReconstruction;
    mReconstruction = nullptr;
  }
}

void ReadCalibration::open(const QString &path)
{
#ifdef _DEBUG
  TL_TODO("Excepción al leer el binario en debug")
  mReconstruction->ReadText(path.toStdString());
#else
  mReconstruction->ReadBinary(path.toStdString());
#endif
}

std::shared_ptr<Calibration> ReadCalibration::calibration(int cameraId) const
{
  std::shared_ptr<Calibration> calibration;
  if (mReconstruction->ExistsCamera(static_cast<colmap::image_t>(cameraId))){

    colmap::Camera &camera = mReconstruction->Camera(static_cast<colmap::image_t>(cameraId));
    std::vector<double> params = camera.Params();

    std::string model_name = camera.ModelName();

    if (model_name.compare("SIMPLE_PINHOLE") == 0){

      calibration = Calibration::create(Calibration::CameraModel::simple_pinhole);
      calibration->setParameter(Calibration::Parameters::focal, params[0]);
      calibration->setParameter(Calibration::Parameters::cx, params[1]);
      calibration->setParameter(Calibration::Parameters::cy, params[2]);

    } else if (model_name.compare("PINHOLE") == 0){

      calibration = Calibration::create(Calibration::CameraModel::pinhole);
      calibration->setParameter(Calibration::Parameters::focalx, params[0]);
      calibration->setParameter(Calibration::Parameters::focaly, params[1]);
      calibration->setParameter(Calibration::Parameters::cx, params[2]);
      calibration->setParameter(Calibration::Parameters::cy, params[3]);

    } else if (model_name.compare("SIMPLE_RADIAL") == 0){

      calibration = Calibration::create(Calibration::CameraModel::radial1);
      calibration->setParameter(Calibration::Parameters::focal, params[0]);
      calibration->setParameter(Calibration::Parameters::cx, params[1]);
      calibration->setParameter(Calibration::Parameters::cy, params[2]);
      calibration->setParameter(Calibration::Parameters::k1, params[3]);

    } else if (model_name.compare("RADIAL") == 0){

      calibration = Calibration::create(Calibration::CameraModel::radial2);
      calibration->setParameter(Calibration::Parameters::focal, params[0]);
      calibration->setParameter(Calibration::Parameters::cx, params[1]);
      calibration->setParameter(Calibration::Parameters::cy, params[2]);
      calibration->setParameter(Calibration::Parameters::k1, params[3]);
      calibration->setParameter(Calibration::Parameters::k2, params[4]);

    } else if (model_name.compare("OPENCV") == 0){

      calibration = Calibration::create(Calibration::CameraModel::opencv);
      calibration->setParameter(Calibration::Parameters::focalx, params[0]);
      calibration->setParameter(Calibration::Parameters::focaly, params[1]);
      calibration->setParameter(Calibration::Parameters::cx, params[2]);
      calibration->setParameter(Calibration::Parameters::cy, params[3]);
      calibration->setParameter(Calibration::Parameters::k1, params[4]);
      calibration->setParameter(Calibration::Parameters::k2, params[5]);
      calibration->setParameter(Calibration::Parameters::p1, params[6]);
      calibration->setParameter(Calibration::Parameters::p2, params[7]);

    } else if (model_name.compare("OPENCV_FISHEYE") == 0){

      calibration = Calibration::create(Calibration::CameraModel::opencv_fisheye);
      calibration->setParameter(Calibration::Parameters::focalx, params[0]);
      calibration->setParameter(Calibration::Parameters::focaly, params[1]);
      calibration->setParameter(Calibration::Parameters::cx, params[2]);
      calibration->setParameter(Calibration::Parameters::cy, params[3]);
      calibration->setParameter(Calibration::Parameters::k1, params[4]);
      calibration->setParameter(Calibration::Parameters::k2, params[5]);
      calibration->setParameter(Calibration::Parameters::k3, params[6]);
      calibration->setParameter(Calibration::Parameters::k4, params[7]);

    } else if (model_name.compare("FULL_OPENCV") == 0){

      calibration = Calibration::create(Calibration::CameraModel::opencv_full);
      calibration->setParameter(Calibration::Parameters::focalx, params[0]);
      calibration->setParameter(Calibration::Parameters::focaly, params[1]);
      calibration->setParameter(Calibration::Parameters::cx, params[2]);
      calibration->setParameter(Calibration::Parameters::cy, params[3]);
      calibration->setParameter(Calibration::Parameters::k1, params[4]);
      calibration->setParameter(Calibration::Parameters::k2, params[5]);
      calibration->setParameter(Calibration::Parameters::p1, params[6]);
      calibration->setParameter(Calibration::Parameters::p2, params[7]);
      calibration->setParameter(Calibration::Parameters::k3, params[8]);
      calibration->setParameter(Calibration::Parameters::k4, params[9]);
      calibration->setParameter(Calibration::Parameters::k5, params[10]);
      calibration->setParameter(Calibration::Parameters::k6, params[11]);

    } else if (model_name.compare("SIMPLE_RADIAL_FISHEYE") == 0){

      calibration = Calibration::create(Calibration::CameraModel::simple_radial_fisheye);
      calibration->setParameter(Calibration::Parameters::focal, params[0]);
      calibration->setParameter(Calibration::Parameters::cx, params[1]);
      calibration->setParameter(Calibration::Parameters::cy, params[2]);
      calibration->setParameter(Calibration::Parameters::k1, params[3]);

    } else if (model_name.compare("RADIAL_FISHEYE") == 0){

      calibration = Calibration::create(Calibration::CameraModel::radial_fisheye);
      calibration->setParameter(Calibration::Parameters::focal, params[0]);
      calibration->setParameter(Calibration::Parameters::cx, params[1]);
      calibration->setParameter(Calibration::Parameters::cy, params[2]);
      calibration->setParameter(Calibration::Parameters::k1, params[3]);
      calibration->setParameter(Calibration::Parameters::k2, params[4]);

    } else if (model_name.compare("FULL_RADIAL") == 0){

      calibration = Calibration::create(Calibration::CameraModel::radial3);
      calibration->setParameter(Calibration::Parameters::focal, params[0]);
      calibration->setParameter(Calibration::Parameters::cx, params[1]);
      calibration->setParameter(Calibration::Parameters::cy, params[2]);
      calibration->setParameter(Calibration::Parameters::k1, params[3]);
      calibration->setParameter(Calibration::Parameters::k2, params[4]);
      calibration->setParameter(Calibration::Parameters::k3, params[5]);
      calibration->setParameter(Calibration::Parameters::p1, params[6]);
      calibration->setParameter(Calibration::Parameters::p2, params[7]);

    } else {
      TL_TODO("camara no soportada")
      return nullptr;
    }

  }

  return calibration;
}

QString cameraToColmapType(const Camera &camera)
{
  QString colmap_camera;
  QString inspector_camera = camera.type();

  if (inspector_camera.compare("Simple Pinhole") == 0){
    colmap_camera = "SIMPLE_PINHOLE";
  } else if (inspector_camera.compare("Pinhole") == 0){
    colmap_camera = "SIMPLE_RADIAL";
  } else if (inspector_camera.compare("Simple Radial") == 0){
    colmap_camera = "SIMPLE_RADIAL";
  } else if (inspector_camera.compare("Radial") == 0){
    colmap_camera = "RADIAL";
  } else if (inspector_camera.compare("OpenCV") == 0){
    colmap_camera = "OPENCV";
  } else if (inspector_camera.compare("OpenCV Fisheye") == 0){
    colmap_camera = "OPENCV_FISHEYE";
  } else if (inspector_camera.compare("OpenCV Full") == 0){
    colmap_camera = "FULL_OPENCV";
  } else if (inspector_camera.compare("Simple Radial Fisheye") == 0){
    colmap_camera = "SIMPLE_RADIAL_FISHEYE";
  } else if (inspector_camera.compare("Radial Fisheye") == 0){
    colmap_camera = "RADIAL_FISHEYE";
  } else if (inspector_camera.compare("Full radial") == 0){
    colmap_camera = "FULL_RADIAL";
  }

  return colmap_camera;
}



} // namespace inspector
