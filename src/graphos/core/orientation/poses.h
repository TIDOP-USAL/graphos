#ifndef GRAPHOS_CORE_PHOTO_ORIENTATION_H
#define GRAPHOS_CORE_PHOTO_ORIENTATION_H

#include "graphos/graphos_global.h"

#include <array>

#include <QString>

#include <tidop/geospatial/camera.h>

namespace graphos
{

class GRAPHOS_EXPORT CameraPose
  : public tl::CameraPose
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

private:

  QString mCrs;
  QString mSource;
};


} // namespace graphos

#endif // GRAPHOS_CORE_PHOTO_ORIENTATION_H
