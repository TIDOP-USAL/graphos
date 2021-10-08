#ifndef GRAPHOS_H
#define GRAPHOS_H

#include "graphos/graphos_global.h"
#include "graphos/core/orientation/poses.h"

#include <QString>

#include <memory>

namespace graphos
{


class GRAPHOS_EXPORT Image
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

  /*!
   * \brief Posición y orientación de la imagen
   * Posición (coordenadas XYZ) y orientación de la cámara en el caso
   * de que sean conocidas.
   * \return
   */
  CameraPose cameraPose() const;

  /*!
   * \brief Establece la Posición y orientación de la imagen
   * \param[in] cameraPose
   */
  void setCameraPose(const CameraPose &cameraPose);

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
  CameraPose mCameraPose;
};

} // namespace graphos

#endif // GRAPHOS_H
