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
     * \brief Default constructor for Camera
     */
    Camera();

    /*!
     * \brief Camera constructor
     * \param[in] make Camera make
     * \param[in] model Camera model
     */
    Camera(std::string make, std::string model);

    /*!
     * \brief Camera copy constructor
     * \param[in] camera
     */
    Camera(const Camera &camera);

    /*!
     * \brief Destructor
     */
    ~Camera() = default;

    /*!
     * \brief Returns the camera make
     * \return Camera make
     */
    auto make() const -> std::string;

    /*!
     * \brief Sets the camera make
     * \param[in] make Camera make
     */
    void setMake(const std::string &make);

    /*!
     * \brief Returns the camera model
     * \return Camera model
     */
    auto model() const -> std::string;

    /*!
     * \brief Sets the camera model
     * \param[in] model Camera model
     */
    void setModel(const std::string &model);

    /*!
     * \brief Returns the camera type
     * \return
     */
    auto type() const -> std::string;

    /*!
     * \brief Sets the camera type
     * The camera type is used for self-calibration
     * \param[in] type Camera type
     */
    void setType(const std::string &type);

    /*!
     * \brief Returns the camera focal length in millimeters
     * \return Camera focal length
     */
    auto focal() const -> double;

    /*!
     * \brief Sets the camera focal length in millimeters
     * \param[in] focal Camera focal length in millimeters
     */
    void setFocal(double focal);

    /*!
     * \brief Returns the sensor width in pixels
     * \return Sensor width in pixels
     */
    auto width() const -> int;

    /*!
     * \brief Sets the sensor width in pixels
     * \param[in] width Sensor width in pixels
     */
    void setWidth(int width);

    /*!
     * \brief Sensor height in pixels
     * \return
     */
    auto height() const -> int;

    /*!
     * \brief Sets the sensor height in pixels
     * \param[in] height Sensor height in pixels
     */
    void setHeight(int height);

    /*!
     * \brief Sensor size in mm
     * If the camera does not exist in the database, 1 is returned
     * \return Sensor size
     */
    auto sensorSize() const -> double;

    /*!
     * \brief Sets the sensor size
     * \param[in] sensorSize Sensor size
     */
    void setSensorSize(double sensorSize);

    auto calibration() const -> std::shared_ptr<Calibration>;
    void setCalibration(std::shared_ptr<Calibration> &calibration);

    /*!
     * \brief Assignment operator
     * \param[in] camera Object to assign
     * \return Reference to the Camera object
     */
    auto operator =(const Camera& camera) -> Camera&;

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
