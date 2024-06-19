/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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

#ifndef GRAPHOS_CORE_ORIENTATION_EXPORT_H
#define GRAPHOS_CORE_ORIENTATION_EXPORT_H

#include "graphos/graphos_global.h"

#include <tidop/core/defs.h>
#include <tidop/geometry/entities/point.h>

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
                      const tl::Point3<double> &offset = tl::Point3<double>(0, 0, 0));
    ~OrientationExport();

    /*!
     * \brief Exportación a formato binario de Colmap
     * \param[in] path Ruta donde se crea el fichero
     */
    void exportBinary(const tl::Path &path) const;

    /*!
     * \brief Exportación a formato texto de Colmap
     * \param[in] path Ruta donde se crea el fichero
     */
    void exportText(const tl::Path &path) const;

    /*!
     * \brief exportNVM
     * \param[in] path Ruta donde se crea el fichero
     */
    //void exportNVM(const tl::Path &path) const;

    /*!
     * \brief exportPLY
     * \param[in] path Ruta donde se crea el fichero
     */
    void exportPLY(const tl::Path &path) const;

    /*!
     * \brief exportVRML
     * \param[in] path Ruta donde se crea el fichero
     */
    //void exportVRML(const tl::Path &path) const;

    /*!
     * \brief Exportación de la orientación al formato Bundler
     * \param[in] oriFile Fichero Bundler de orientaciones
     */
     //void exportBundler(const QString &oriFile, const QString &imageListFile) const;
    //void exportBundler(const tl::Path &oriFile) const;

    /*!
     * \brief Exportación de la orientación relativa a un fichero de texto
     * \param[in] path Ruta donde se crea el fichero
     * \param[in] bQuaternion Orientaciones como cuaterniones. En caso contrario se escribe la matriz de rotación
     */
    //void exportOrientation(const tl::Path &path, bool bQuaternion = true) const;

    /*!
     * \brief Exportación del resultado de la auto-calibración al formato Pix4D
     * \param[in] path Ruta donde se crea el fichero
     */
    //void exportPix4DCalibration(const tl::Path &path, const tl::Path &imagePath) const;

    /*!
     * \brief exportMVE
     * \param[in] path Ruta donde se crea el fichero
     */
    //void exportMVE(const tl::Path &path) const;

private:

    const colmap::Reconstruction *mReconstruction;
    tl::Point3<double> mOffset;
};

} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_EXPORT_H
