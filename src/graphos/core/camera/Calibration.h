/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#ifndef GRAPHOS_CORE_CAMERA_CALIBRATION_H
#define GRAPHOS_CORE_CAMERA_CALIBRATION_H


#include <map>

#include <tidop/core/flags.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/quaternion.h>

namespace graphos
{

	
class Calibration
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

  virtual std::string name() const = 0;

  std::string parameterName(Parameters parameter) const;

  bool existParameter(Parameters parameter) const;
  double parameter(Parameters parameter) const;
  void setParameter(Parameters parameter, double value);

  bool checkCameraType(CameraType camera_type) const;

protected:

  CameraModel convertFlags(Parameters parameter) const;
  CameraModel convertFlags(CameraType cameraType) const;

protected:

  tl::EnumFlags<CameraModel> mCameraModel;
  std::map<Parameters, double> mParameters;
};

ALLOW_BITWISE_FLAG_OPERATIONS(Calibration::CameraModel)


class CalibrationFactory
{

private:

  CalibrationFactory() {}

public:

  static std::shared_ptr<Calibration> create(const std::string &cameraType);
  static std::shared_ptr<Calibration> create(Calibration::CameraModel cameraModel);
};


} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_CALIBRATION_H
