#ifndef GRAPHOS_CORE_ORIENTATION_EXPORT_H
#define GRAPHOS_CORE_ORIENTATION_EXPORT_H

#include "graphos/graphos_global.h"

#include <tidop/core/defs.h>
#include <tidop/geometry/entities/point.h>

#include <QString>

namespace colmap
{
  class Reconstruction;
}


namespace graphos
{

class TL_EXPORT OrientationExport
{

public:

  OrientationExport(const colmap::Reconstruction *reconstruction, 
                    const tl::Point3D &offset = tl::Point3D(0,0,0));
  ~OrientationExport();

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
   * \brief Exportación de la orientación al formato Bundler
   * \param[in] oriFile Fichero Bundler de orientaciones
   */
  //void exportBundler(const QString &oriFile, const QString &imageListFile) const;
  void exportBundler(const QString &oriFile) const;

  /*!
   * \brief Exportación de la orientación relativa a un fichero de texto
   * \param[in] path Ruta donde se crea el fichero
   * \param[in] bQuaternion Orientaciones como cuaterniones. En caso contrario se escribe la matriz de rotación
   */
  void exportOrientation(const QString &path, bool bQuaternion = true) const;

  /*!
   * \brief Exportación del resultado de la auto-calibración al formato Pix4D
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportPix4DCalibration(const QString &path, const QString &imagePath) const;

  /*!
   * \brief exportMVE
   * \param[in] path Ruta donde se crea el fichero
   */
  void exportMVE(const QString &path) const;

  void exportCMVS(const QString &path) const;

private:

  const colmap::Reconstruction *mReconstruction;
  tl::Point3D mOffset;
};

} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_EXPORT_H
