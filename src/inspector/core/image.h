#ifndef INSPECTOR_IMAGE_H
#define INSPECTOR_IMAGE_H

#include "inspector/inspector_global.h"

#include <tidop/math/algebra/rotations.h>

#include <QString>

namespace inspector
{

class INSPECTOR_EXPORT Image
{

public:

  class CameraPosition
  {

  public:

    CameraPosition() {}

    QString crs() const;
    void setCrs(const QString &crs);

    double x() const;
    void double setX();
    double y() const;
    void double setY();
    double z() const;
    void double setZ();
    void rotation() const;
    void setRotation();
  };

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

  /*!
   * \brief longitudeExif
   * \return
   */
  double longitudeExif() const;

  /*!
   * \brief setLongitudeExif
   * \param longitudeExif
   */
  void setLongitudeExif(double longitudeExif);

  /*!
   * \brief latitudeExif
   * \return
   */
  double latitudeExif() const;

  /*!
   * \brief setLatitudeExif
   * \param latitudeExif
   */
  void setLatitudeExif(double latitudeExif);

  /*!
   * \brief altitudeExif
   * \return
   */
  double altitudeExif() const;

  /*!
   * \brief setAltitudeExif
   * \param altitudeExif
   */
  void setAltitudeExif(double altitudeExif);

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
  double mLongitudeExif;
  double mLatitudeExif;
  double mAltitudeExif;

};

} // namespace inspector

#endif // INSPECTOR_IMAGE_H
