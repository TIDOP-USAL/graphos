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

#ifndef GRAPHOS_CORE_IMAGE_H
#define GRAPHOS_CORE_IMAGE_H

#include <memory>

#include <QString>

#include <tidop/core/path.h>

#include "graphos/core/sfm/poses.h"

namespace graphos
{


class Image
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
     * \brief Devuelve el identificador de la imagen
     * \return
     */
    size_t id() const;

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
    //QString mName;
    size_t mId;
    int mCameraId;
    CameraPose mCameraPose;
};

} // namespace graphos

#endif // GRAPHOS_CORE_IMAGE_H
