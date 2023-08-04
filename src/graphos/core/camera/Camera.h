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

#ifndef GRAPHOS_CORE_CAMERA_H
#define GRAPHOS_CORE_CAMERA_H

#include "graphos/core/camera/Calibration.h"

namespace graphos
{

class Camera
{

public:

    /*!
     * \brief Constructor por defecto de Camera
     */
    Camera();

    /*!
     * \brief Constructor Camera
     * \param[in] make Marca de la cámara
     * \param[in] model Modelo de la cámara
     */
    Camera(const std::string &make,
           const std::string &model);

    /*!
     * \brief Constructor de copia Camera
     * \param[in] camera
     */
    Camera(const Camera &camera);

    /*!
     * \brief Destructor
     */
    ~Camera() = default;

    /*!
     * \brief Devuelve la marca de la cámara
     * \return Marca de la cámara
     */
    std::string make() const;

    /*!
     * \brief Establece la marca de la cámara
     * \param[in] make Marca de la cámara
     */
    void setMake(const std::string &make);

    /*!
     * \brief Devuelve el modelo de la cámara
     * \return Modelo de la cámara
     */
    std::string model() const;

    /*!
     * \brief Establece el modelo de la cámara
     * \param[in] model Modelo de la cámara
     */
    void setModel(const std::string &model);

    /*!
     * \brief Devuelve el tipo de cámara
     * \return
     */
    std::string type() const;

    /*!
     * \brief Establece el tipo de cámara
     * El tipo de cámara se utiliza para la autocalibración
     * \param[in] type Tipo de cámara
     */
    void setType(const std::string &type);

    /*!
     * \brief Devuelve la focal de la cámara en milímetros
     * \return Focal de la cámara
     */
    double focal() const;

    /*!
     * \brief Establece la focal de la cámara en milímetros
     * \param[in] focal Focal de la cámara en milímetros
     */
    void setFocal(double focal);

    /*!
     * \brief Devuelve el ancho de sensor en píxeles
     * \return Ancho del sensor en píxeles
     */
    int width() const;

    /*!
     * \brief Establece el ancho del sensor en píxeles
     * \param[in] width Ancho del sensor en píxeles
     */
    void setWidth(int width);

    /*!
     * \brief Altura del sensor en píxeles
     * \return
     */
    int height() const;

    /*!
     * \brief Establece el alto del sensor en pixeles
     * \param[in] height Alto del sensor en pixeles
     */
    void setHeight(int height);

    /*!
     * \brief Tamaño del sensor en mm
     * Si no existe la cámara en la base de datos se devuelve 1
     * \return Tamaños del sensor
     */
    double sensorSize() const;

    /*!
     * \brief Establece el tamaño del sensor
     * \param[in] sensorSize Tamaño del sensor
     */
    void setSensorSize(double sensorSize);

    std::shared_ptr<Calibration> calibration() const;
    void setCalibration(std::shared_ptr<Calibration> &calibration);

    /*!
     * \brief Operador de asignación
     * \param[in] camera Objeto que se asigna
     * \return Referencia al objeto Camera
     */
    Camera &operator =(const Camera &camera);

private:

    void init();

protected:

    std::string mMake;
    std::string mModel;
    std::string mType;
    double mFocal;
    int mWidth;
    int mHeight;
    double mSensorSize;
    std::shared_ptr<Calibration> mCalibration;

};

} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_H
