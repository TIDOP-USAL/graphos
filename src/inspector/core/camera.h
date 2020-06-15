#ifndef INSPECTOR_CAMERA_H
#define INSPECTOR_CAMERA_H

#include "inspector/inspector_global.h"

#include <map>
#include <memory>

#include <QString>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>

namespace colmap
{
class Reconstruction;
}

namespace inspector
{


class INSPECTOR_EXPORT Calibration
{

public:

  enum CameraType
  {
    radial  = (0 << 0),
    fisheye = (1 << 0),
    pinhole = (1 << 1),
    opencv  = (1 << 2)
  };

  enum class Parameters
  {
    focal = (1 << 10),
    focalx = (1 << 11),
    focaly = (1 << 12),
    cx = (1 << 13),
    cy = (1 << 14),
    k1 = (1 << 15),
    k2 = (1 << 16),
    k3 = (1 << 17),
    k4 = (1 << 18),
    k5 = (1 << 19),
    k6 = (1 << 20),
    p1 = (1 << 21),
    p2 = (1 << 22)
  };

  enum class CameraModel
  {
    /*!< f, cx, cy, k1 */
    radial1 = CameraType::radial |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::focal) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1),
    /*!< f, cx, cy, k1, k2 */
    radial2 = CameraType::radial |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::focal) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::k2),
    /*!< f, cx, cy, k1, k2, k3, p1, p2 */
    radial3 = CameraType::radial |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::focal) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::k2) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::k3) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::p1) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::p2),
    /*!< f, cx, cy, k1 */
    simple_radial_fisheye = CameraType::radial |
                            CameraType::fisheye |
                            static_cast<std::underlying_type<CameraModel>::type>(Parameters::focal) |
                            static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
                            static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
                            static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1),
    /*!< f, cx, cy, k1, k2 */
    radial_fisheye = CameraType::radial |
                     CameraType::fisheye |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::focal) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::k2),
    /*!< fx, fy, cx, cy, k1, k2, p1, p2 */
    opencv = CameraType::opencv |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::focalx) |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::focaly) |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1) |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::k2) |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::p1) |
             static_cast<std::underlying_type<CameraModel>::type>(Parameters::p2),
    /*!< fx, fy, cx, cy, k1, k2, k3, k4 */
    opencv_fisheye = CameraType::opencv |
                     CameraType::fisheye |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::focalx) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::focaly) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::k2) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::k3) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::k4),
    /*!< fx, fy, cx, cy, k1, k2, p1, p2, k3, k4, k5, k6 */
    opencv_full = CameraType::opencv |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::focalx) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::focaly) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::k1) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::k2) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::k3) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::k4) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::k5) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::k6) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::p1) |
                  static_cast<std::underlying_type<CameraModel>::type>(Parameters::p2),
    /*!< f, cx, cy */
    simple_pinhole = CameraType::pinhole |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::focal) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
                     static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy),
    /*!< fx, fy, cx, cy */
    pinhole = CameraType::pinhole |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::focalx) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::focaly) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cx) |
              static_cast<std::underlying_type<CameraModel>::type>(Parameters::cy)
    //fov,                      /*!< fx, fy, cx, cy, omega */
    //thin_prism                /*!< fx, fy, cx, cy, k1, k2, p1, p2, k3, k4, sx1, sy1 */
  };

  typedef std::map<Calibration::Parameters, double>::iterator parameter_iterator;
  typedef std::map<Calibration::Parameters, double>::const_iterator parameter_const_iterator;

public:

  Calibration(CameraModel cameraModel);
  Calibration(const Calibration &calibration);
  Calibration(Calibration &&calibration) TL_NOEXCEPT;

  virtual ~Calibration() = default;

  static std::shared_ptr<Calibration> create(CameraModel cameraModel);

  CameraModel cameraModel() const;
  int cameraModelId() const;

  /*!
   * \brief Operador de asignación
   */
  Calibration &operator = (const Calibration &calibration);

  /*!
   * \brief Operador de asignación de movimiento
   */
  Calibration &operator = (Calibration &&calibration) TL_NOEXCEPT;

  /*!
   * \brief Devuelve un iterador al inicio del listado de parametros de la calibración
   * \return Iterador al primer parámetro de calibración
   */
  parameter_iterator parametersBegin();

  /*!
   * \brief Devuelve un iterador constante al inicio del listado de parametros de la calibración
   * \return Iterador al primer parámetro de calibración
   */
  parameter_const_iterator parametersBegin() const;

  /*!
   * \brief Devuelve un iterador al siguiente elemento después del último parámetro de la calibración
   * Este elemento actúa como un marcador de posición, intentar acceder a él resulta en un comportamiento no definido
   * \return Iterador al siguiente elemento después del último parámetro de la calibración
   */
  parameter_iterator parametersEnd();

  /*!
   * \brief Devuelve un iterador constante al siguiente elemento después del último parámetro de la calibración
   * Este elemento actúa como un marcador de posición, intentar acceder a él resulta en un comportamiento no definido
   * \return Iterador constante al siguiente elemento después del último parámetro de la calibración
   */
  parameter_const_iterator parametersEnd() const;

//  parameter_iterator find(Parameters parameter);
//  parameter_const_iterator find(Parameters parameter) const;

  virtual QString name() const = 0;

  QString parameterName(Parameters parameter) const;

  bool existParameter(Parameters parameter) const;
  double parameter(Parameters parameter) const;
  void setParameter(Parameters parameter, double value);

protected:

  CameraModel convertFlags(Parameters parameter);

protected:

  tl::EnumFlags<CameraModel> mCameraModel;
  std::map<Parameters, double> mParameters;
};

ALLOW_BITWISE_FLAG_OPERATIONS(Calibration::CameraModel)



/* ------------------------------------------------------------------ */




class INSPECTOR_EXPORT CalibrationFactory
{

private:

  CalibrationFactory() {}

public:

  static std::shared_ptr<Calibration> create(const QString &cameraType);
};




/* ------------------------------------------------------------------ */




class INSPECTOR_EXPORT Camera
{

public:

  /*!
   * \brief Constructor por defecto de Camera
   */
  Camera();

  /*!
   * \brief Constructor Camera
   * \param[in] make Marca de la cámara
   * \param[in] model Modelo de la cámara
   */
  Camera(const QString &make, const QString &model);

  /*!
   * \brief Constructor de copia Camera
   * \param[in] camera
   */
  Camera(const Camera &camera);

  /*!
   * \brief Destructor
   */
  ~Camera() = default;

  /*!
   * \brief Devuelve la marca de la cámara
   * \return Marca de la cámara
   */
  QString make() const;

  /*!
   * \brief Establece la marca de la cámara
   * \param[in] make Marca de la cámara
   */
  void setMake(const QString &make);

  /*!
   * \brief Devuelve el modelo de la cámara
   * \return Modelo de la cámara
   */
  QString model() const;

  /*!
   * \brief Establece el modelo de la cámara
   * \param[in] model Modelo de la cámara
   */
  void setModel(const QString &model);

  /*!
   * \brief Devuelve el tipo de cámara
   * \return
   */
  QString type() const;

  /*!
   * \brief Establece el tipo de cámara
   * El tipo de cámara se utiliza para la autocalibración
   * \param[in] type Tipo de cámara
   */
  void setType(const QString &type);

  /*!
   * \brief Devuelve la focal de la cámara en milímetros
   * \return Focal de la cámara
   */
  double focal() const;

  /*!
   * \brief Establece la focal de la cámara en milímetros
   * \param[in] focal Focal de la cámara en milímetros
   */
  void setFocal(double focal);

  /*!
   * \brief Devuelve el ancho de sensor en píxeles
   * \return Ancho del sensor en píxeles
   */
  int width() const;

  /*!
   * \brief Establece el ancho del sensor en píxeles
   * \param[in] width Ancho del sensor en píxeles
   */
  void setWidth(int width);

  /*!
   * \brief Altura del sensor en píxeles
   * \return
   */
  int height() const;

  /*!
   * \brief Establece el alto del sensor en pixeles
   * \param[in] height Alto del sensor en pixeles
   */
  void setHeight(int height);

  /*!
   * \brief Tamaño del sensor en mm
   * Si no existe la cámara en la base de datos se devuelve 1
   * \return Tamaños del sensor
   */
  double sensorSize() const;

  /*!
   * \brief Establece el tamaño del sensor
   * \param[in] sensorSize Tamaño del sensor
   */
  void setSensorSize(double sensorSize);

  std::shared_ptr<Calibration> calibration() const;
  void setCalibration(std::shared_ptr<Calibration> &calibration);

  /*!
   * \brief Operador de asignación
   * \param[in] camera Objeto que se asigna
   * \return Referencia al objeto Camera
   */
  Camera &operator =(const Camera &camera);

private:

  void init();

protected:

  QString mMake;
  QString mModel;
  QString mType;
  double mFocal;
  int mWidth;
  int mHeight;
  double mSensorSize;
  std::shared_ptr<Calibration> mCalibration;

};



/* ------------------------------------------------------------------ */



class INSPECTOR_EXPORT ReadCalibration
{

public:

  ReadCalibration();
  ~ReadCalibration();

  void open(const QString &path);
  std::shared_ptr<Calibration> calibration(int cameraId) const;

protected:

  colmap::Reconstruction *mReconstruction;

};



/* ------------------------------------------------------------------ */


QString cameraToColmapType(const Camera &camera);

} // namespace inspector

#endif // INSPECTOR_CAMERA_H
