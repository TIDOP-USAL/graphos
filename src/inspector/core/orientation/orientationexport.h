#ifndef INSPECTOR_CORE_ORIENTATION_EXPORT_H
#define INSPECTOR_CORE_ORIENTATION_EXPORT_H

#include "inspector/inspector_global.h"

#include <tidop/core/defs.h>

#include <QString>

namespace colmap
{
  class Reconstruction;
}


namespace inspector
{

class TL_EXPORT OrientationExport
{

public:

  OrientationExport(const QString &imagePath,
                    const colmap::Reconstruction *reconstruction);
  ~OrientationExport();

  /*!
   * \brief setImagePath
   * \param imagePath
   */
  void setImagePath(const QString &imagePath);

  /*!
   * \brief Exportación a formato binario de Colmap
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportBinary(const QString &path) const;

  /*!
   * \brief Exportación a formato texto de Colmap
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportText(const QString &path) const;

  /*!
   * \brief exportNVM
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportNVM(const QString &path) const;

  /*!
   * \brief exportPLY
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportPLY(const QString &path) const;

  /*!
   * \brief exportVRML
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportVRML(const QString &path) const;

  /*!
   * \brief Exportación de la orientación relativa a un fichero de texto
   * \param[in] path Ruta donde se crea el fichero
   * \param[in] bQuaternion Orientaciones como cuaterniones. En caso contrario se escribe la matriz de rotación
   */
  void exportRelativeOrientation(const QString &path, bool bQuaternion = true) const;

  /*!
   * \brief Exportación del resultado de la auto-calibración al formato Pix4D
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportPix4DCalibration(const QString &path) const;

  /*!
   * \brief exportMVE
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportMVE(const QString &path) const;

private:

  QString mImagePath;
  const colmap::Reconstruction *mReconstruction;

};

} // namespace inspector

#endif // INSPECTOR_CORE_ORIENTATION_EXPORT_H
