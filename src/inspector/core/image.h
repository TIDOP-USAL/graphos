#ifndef INSPECTOR_IMAGE_H
#define INSPECTOR_IMAGE_H

#include "inspector/inspector_global.h"

#include <tidop/math/algebra/rotations.h>
#include <tidop/math/algebra/quaternion.h>

#include <QString>

#include <memory>

namespace inspector
{

class CameraPosition
{

public:

  CameraPosition();
  ~CameraPosition() = default;

  QString crs() const;
  void setCrs(const QString &crs);

  double x() const;
  void setX(double x);
  double y() const;
  void setY(double y);
  double z() const;
  void setZ(double z);
  tl::math::Quaterniond quaternion() const;
  void setQuaternion(const tl::math::Quaterniond &quaternion);

  QString source() const;
  void setSource(const QString &source);

  bool isEmpty() const;
  //std::shared_ptr<tl::math::Rotation> rotation() const;
  //void setRotation(std::shared_ptr<tl::math::Rotation> &rotation);

private:

  QString mCrs;
  double mX;
  double mY;
  double mZ;
  tl::math::Quaterniond mQuaternion;
  QString mSource;
  //std::shared_ptr<tl::math::Rotation> mRotation;
};



class INSPECTOR_EXPORT Image
{

public:

  /*!
   * \brief Constructor Image por defecto
   */
  Image();

  /*!
   * \brief Constructor Image
   * \param[in] file Fichero imagen
   */
  Image(const QString &file);

  /*!
   * \brief Constructor de copia Image
   * \param[in] image Objeto que se copia
   */
  Image(const Image &image);

  /*!
   * \brief Constructor de movimiento Image
   * \param[in] image Objeto que se copia
   */
  Image(Image &&image) noexcept;

  /*!
   * \brief Destructor Image
   */
  ~Image() = default;

  /*!
   * \brief Devuelve la ruta del fichero imagen
   * \return Ruta del fichero
   */
  QString path() const;

  /*!
   * \brief Establece la ruta del fichero imagen
   * \param[in] file Ruta del fichero
   */
  void setPath(const QString &file);

  /*!
   * \brief Nombre del fichero
   * \return Nombre del fichero
   */
  QString name() const;

  /*!
   * \brief Devuelve el identificador de la cámara
   * \return
   */
  int cameraId() const;

  /*!
   * \brief Establece el identificador de la cámara
   * \param[in] cameraId Identificador de cámara
   */
  void setCameraId(int cameraId);

  CameraPosition cameraPosition() const;
  void setCameraPosition(const CameraPosition &cameraPosition);

//  /*!
//   * \brief longitudeExif
//   * \return
//   */
//  double longitudeExif() const;

//  /*!
//   * \brief setLongitudeExif
//   * \param longitudeExif
//   */
//  void setLongitudeExif(double longitudeExif);

//  /*!
//   * \brief latitudeExif
//   * \return
//   */
//  double latitudeExif() const;

//  /*!
//   * \brief setLatitudeExif
//   * \param latitudeExif
//   */
//  void setLatitudeExif(double latitudeExif);

//  /*!
//   * \brief altitudeExif
//   * \return
//   */
//  double altitudeExif() const;

//  /*!
//   * \brief setAltitudeExif
//   * \param altitudeExif
//   */
//  void setAltitudeExif(double altitudeExif);

  /*!
   * \brief Operador de asignación
   * \param[in] image Objeto que se asigna
   * \return Referencia al objeto Image
   */
  Image &operator =(const Image &image);

  /*!
   * \brief Operador de asignación de movimiento
   * \param[in] image Objeto que se mueve
   * \return Referencia al objeto Image
   */
  Image &operator =(Image &&image) noexcept;

private:

  void update();

protected:

  QString mFilePath;
  QString mName;
  int mCameraId;
//  double mLongitudeExif;
//  double mLatitudeExif;
//  double mAltitudeExif;
  CameraPosition mCameraPosition;
};

} // namespace inspector

#endif // INSPECTOR_IMAGE_H
