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

#ifndef GRAPHOS_CORE_ORIENTATION_POSES_H
#define GRAPHOS_CORE_ORIENTATION_POSES_H

#include <QString>

#include <tidop/geometry/entities/point.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/quaternion.h>

namespace graphos
{

class CameraPose
{

public:

  CameraPose();
  CameraPose(double x, double y, double z,
             const tl::math::RotationMatrix<double> &rotationMatrix);
  CameraPose(const tl::Point3D &center,
             const tl::math::RotationMatrix<double> &rotationMatrix);
  CameraPose(double x, double y, double z,
             const tl::math::Quaternion<double> &quaternion);
  CameraPose(const tl::Point3D &center,
             const tl::math::Quaternion<double> &quaternion);
  ~CameraPose();

  tl::Point3D position() const;
  void setPosition(const tl::Point3D &position);
  
  /*!
   * \brief Rotación como cuaterniones
   * \return
   */
  tl::math::Quaterniond quaternion() const;

  /*!
   * \brief Establece la orientación de la cámaras como cuaterniones
   * \param[in] quaternion Orientación de la cámara
   */
  void setQuaternion(const tl::math::Quaterniond &quaternion);

  /*!
   * \brief Rotación como matriz de rotación
   * \return
   */
  tl::math::RotationMatrix<double> rotationMatrix() const;

  /*!
   * \brief Establece la orientación de la cámaras como matriz de rotación
   * \param[in] rotationMatrix Orientación de la cámara
   */
  void setRotationMatrix(const tl::math::RotationMatrix<double> &rotationMatrix);
  
  /*!
   * \brief Sistema de referencia como código EPSG
   * \return
   */
  QString crs() const;

  /*!
   * \brief Establece el sistema de referencia como código EPSG
   * \param[in] crs Código EPSG
   */
  void setCrs(const QString &crs);

  /*!
   * \brief Fichero fuente con las posiciones/orientaciones o EXIF
   * \return
   */
  QString source() const;

  /*!
   * \brief Establece el fichero fuente con las posiciones/orientaciones o EXIF
   * \param[in] source
   */
  void setSource(const QString &source);

  bool isEmpty() const;
  
private:

  tl::Point3D mPosition;
  std::shared_ptr<tl::math::Rotation> mRotation;
  QString mCrs;
  QString mSource;
};


} // namespace graphos

#endif // GRAPHOS_CORE_PHOTO_ORIENTATION_H
