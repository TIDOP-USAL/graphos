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
#include "graphos/core/multispectral/Vignetting.h"

#include <optional>

#include <tidop/math/algebra/matrix/Matrix.h>

namespace graphos
{

class Camera
{

protected:

    std::string mMake;
    std::string mModel;
    std::string mSerialNumber;
    std::string mType{"OpenCV 1"};
    std::string mBandName{"RGB"};
    double mFocal{1.0};
    int mWidth{0};
    int mHeight{0};
    int mBitsPerPixel{-1};
    double mSensorSize{1.0};
    std::optional<uint16_t> mBlackLevel; 
    std::optional<tl::Matrix3x3f>mCalibratedHMatrix;
    std::shared_ptr<Vignetting> mVignetting;
    std::shared_ptr<Calibration> mCalibration;
    std::shared_ptr<Calibration> mPriorCalibration;

public:

    /*!
     * \brief Default constructor for Camera
     */
    Camera() = default;

    /*!
     * \brief Camera constructor
     * \param[in] make Camera make
     * \param[in] model Camera model
     * \param[in] serialNumber Camera serial number
     * \param[in] bandName Camera band name
     */
    Camera(std::string make, 
           std::string model, 
           std::string serialNumber = "", 
           std::string bandName = "RGB");

    /*!
     * \brief Camera copy constructor
     * \param[in] camera
     */
    Camera(const Camera &camera) = default;
    Camera(Camera &&camera) noexcept = default;

    /*!
     * \brief Destructor
     */
    ~Camera() = default;


    /*!
     * \brief Assignment operator
     * \param[in] camera Object to assign
     * \return Reference to the Camera object
     */
    auto operator =(const Camera &camera) -> Camera & = default;
    auto operator =(Camera &&camera) noexcept -> Camera & = default;

    /*!
     * \brief Returns the camera make
     * \return Camera make
     */
    [[nodiscard]]
    auto make() const -> const std::string &;

    /*!
     * \brief Sets the camera make
     * \param[in] make Camera make
     */
    void setMake(std::string make);

    /*!
     * \brief Returns the camera model
     * \return Camera model
     */
    [[nodiscard]]
    auto model() const -> const std::string &;

    /*!
     * \brief Sets the camera model
     * \param[in] model Camera model
     */
    void setModel(std::string model);

    /*!
     * \brief Returns the camera serial number
     * \return Serial number
     */
    [[nodiscard]]
    auto serialNumber() const -> const std::string &;

    /*!
     * \brief Sets the camera serial number
     * \param[in] serialNumber Serial number
     */
    void setSerialNumber(std::string serialNumber);

    /*!
     * \brief Returns the camera type
     * \return
     */
    [[nodiscard]]
    auto type() const -> const std::string &;

    /*!
     * \brief Sets the camera type
     * The camera type is used for self-calibration
     * \param[in] type Camera type
     */
    void setType(std::string type);

    [[nodiscard]]
    auto bandName() const -> const std::string &;
    void setBandName(std::string bandName);

    /*!
     * \brief Returns the camera focal length in millimeters
     * \return Camera focal length
     */
    [[nodiscard]]
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
    [[nodiscard]]
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
    [[nodiscard]]
    auto height() const -> int;

    /*!
     * \brief Sets the sensor height in pixels
     * \param[in] height Sensor height in pixels
     */
    void setHeight(int height);

    /*!
     * \brief Bits per pixel
     * \return Bits per pixel
     */
    [[nodiscard]]
    auto bitsPerPixel() const -> int;

    /*!
     * \brief Sets the bits per pixel
     * \param[in] bitsPerPixel Bits per pixel
     */
    void setBitsPerPixel(int bitsPerPixel);

    /*!
     * \brief Sensor size in mm
     * If the camera does not exist in the database, 1 is returned
     * \return Sensor size
     */
    [[nodiscard]]
    auto sensorSize() const -> double;

    /*!
     * \brief Sets the sensor size
     * \param[in] sensorSize Sensor size
     */
    void setSensorSize(double sensorSize);

    [[nodiscard]]
    auto blackLevel() const -> std::optional<uint16_t>;
    void setBlackLevel(uint16_t blackLevel);

    [[nodiscard]]
    auto vignettingModel() const -> std::shared_ptr<Vignetting>;
    void setVignettingModel(std::shared_ptr<Vignetting> model);

    [[nodiscard]]
    auto calibration() const -> std::shared_ptr<Calibration>;
    void setCalibration(std::shared_ptr<Calibration> calibration);

    [[nodiscard]]
    auto priorCalibration() const -> std::shared_ptr<Calibration>;
    void setPriorCalibration(std::shared_ptr<Calibration> calibration);

    [[nodiscard]]
    auto calibratedHMatrix() const -> const std::optional<tl::Matrix3x3f> &;
    void setCalibratedHMatrix(const tl::Matrix3x3f &hMatrix);
    //[[nodiscard]]
    //auto hasCalibratedHMatrix() const -> bool;

    [[nodiscard]]
    auto signature() const -> std::string;
};

} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_H
