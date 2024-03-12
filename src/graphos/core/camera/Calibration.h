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

#ifndef GRAPHOS_CORE_CAMERA_CALIBRATION_H
#define GRAPHOS_CORE_CAMERA_CALIBRATION_H

#include <unordered_map>
#include <map>

#include <tidop/core/flags.h>
#include <tidop/core/path.h>
#include <tidop/math/algebra/rotation_matrix.h>


namespace graphos
{


class Calibration
{

public:

    enum CameraType
    {
        radial  = (1 << 0),
        fisheye = (1 << 1),
        pinhole = (1 << 2),
        opencv  = (1 << 3)
    };

    enum class Parameters
    {
        focal  = (1 << 10),
        focalx = (1 << 11),
        focaly = (1 << 12),
        cx     = (1 << 13),
        cy     = (1 << 14),
        k1     = (1 << 15),
        k2     = (1 << 16),
        k3     = (1 << 17),
        k4     = (1 << 18),
        k5     = (1 << 19),
        k6     = (1 << 20),
        p1     = (1 << 21),
        p2     = (1 << 22)
    };

    enum class CameraModel
    {
        /*!< f, cx, cy, k1 */
        radial1 = CameraType::radial |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focal) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1),
        /*!< f, cx, cy, k1, k2 */
        radial2 = CameraType::radial |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focal) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k2),
        /*!< f, cx, cy, k1, k2, k3, p1, p2 */
        radial3 = CameraType::radial |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focal) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k2) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k3) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::p1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::p2),
        /*!< f, cx, cy, k1 */
        simple_radial_fisheye = CameraType::radial |
        CameraType::fisheye |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focal) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1),
        /*!< f, cx, cy, k1, k2 */
        radial_fisheye = CameraType::radial |
        CameraType::fisheye |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focal) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k2),
        /*!< fx, fy, cx, cy, k1, k2, p1, p2 */
        opencv = CameraType::radial |
        CameraType::opencv |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focalx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focaly) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k2) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::p1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::p2),
        /*!< fx, fy, cx, cy, k1, k2, k3, k4 */
        opencv_fisheye = CameraType::radial |
        CameraType::opencv |
        CameraType::fisheye |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focalx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focaly) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k2) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k3) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k4),
        /*!< fx, fy, cx, cy, k1, k2, p1, p2, k3, k4, k5, k6 */
        opencv_full = CameraType::radial |
        CameraType::opencv |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focalx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focaly) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k2) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k3) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k4) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k5) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::k6) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::p1) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::p2),
        /*!< f, cx, cy */
        simple_pinhole = CameraType::pinhole |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focal) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy),
        /*!< fx, fy, cx, cy */
        pinhole = CameraType::pinhole |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focalx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::focaly) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cx) |
        static_cast<std::underlying_type_t<CameraModel>>(Parameters::cy)
        //fov,                      /*!< fx, fy, cx, cy, omega */
        //thin_prism                /*!< fx, fy, cx, cy, k1, k2, p1, p2, k3, k4, sx1, sy1 */
    };

    using iterator = std::map<Parameters, double>::iterator;
    using const_iterator = std::map<Parameters, double>::const_iterator;

public:

    explicit Calibration(CameraModel cameraModel);
    Calibration(const Calibration &calibration);
    Calibration(Calibration &&calibration) TL_NOEXCEPT;

    virtual ~Calibration() = default;

    /*!
     * \brief Camera model used
     * Available camera types are radial1, radial2, radial3, simple_radial_fisheye,
     * radial_fisheye, opencv, opencv_fisheye, opencv_full, simple_pinhole and pinhole.
     * \return Camera model
     * \see CameraModel
     */
    auto cameraModel() const -> CameraModel;

    /*!
     * \brief Camera model ID
     */
    auto cameraModelId() const -> int;

    auto operator =(const Calibration& calibration) -> Calibration&;
    auto operator =(Calibration&& calibration) TL_NOEXCEPT -> Calibration&;

    auto begin() -> iterator;
    auto begin() const -> const_iterator;
    auto end() -> iterator;
    auto end() const -> const_iterator;

    /*!
     * \brief Camera model name
     * The names corresponding to each camera model are:
     * - radial1 -> Radial 1
     * - radial2 -> Radial 2
     * - radial3 -> Radial 3
     * - simple_radial_fisheye -> Radial Fisheye 1
     * - radial_fisheye -> Radial Fisheye 2
     * - opencv -> OpenCV 1
     * - opencv_fisheye -> OpenCV Fisheye
     * - opencv_full -> OpenCV 2
     * - simple_pinhole -> Pinhole 1
     * - pinhole -> Pinhole 2
     * \return 
     */
    virtual auto name() const -> std::string;

    /*!
     * \brief Check if the parameter exists for the camera model
     * \param[in] parameter Parameter
     * \see Parameters
     */
    auto existParameter(Parameters parameter) const -> bool;

    /*!
     * \brief Returns the value of the parameter. If the parameter does not exist it returns 0
     * \param[in] parameter Parameter
     * \return Value of the parameter
     */
    auto parameter(Parameters parameter) const -> double;

    /*!
     * \brief Sets the value of a parameter
     * \param[in] parameter Parameter key
     * \param[in] value Parameter value
     */
    void setParameter(Parameters parameter, double value);

    /*!
     * \brief Check if the camera model corresponds to the type of camera.
     * There are four types of cameras:
     * - radial
     * - fisheye
     * - pinhole
     * - opencv
     * \param[in] cameraType Camera type
     * \return
     * \see CameraType
     */
    auto checkCameraType(CameraType cameraType) const -> bool;

    /*!
     * \brief string with the name of the parameter
     * \param[in] parameter Parameter key
     * \return 
     */
    static auto parameterName(Parameters parameter) -> std::string;

protected:

    static auto convertFlags(Parameters parameter) -> CameraModel;
    static auto convertFlags(CameraType cameraType) -> CameraModel;

protected:

    tl::EnumFlags<CameraModel> mCameraModel;
    std::map<Parameters, double> mParameters;
};

ALLOW_BITWISE_FLAG_OPERATIONS(Calibration::CameraModel)


class CalibrationFactory
{

private:

    CalibrationFactory() {}

public:

    static auto create(const std::string &cameraType) -> std::shared_ptr<Calibration>;
    static auto create(Calibration::CameraModel cameraModel) -> std::shared_ptr<Calibration>;
};



/* Calibration reader */

class CalibrationReader
{

public:

    CalibrationReader();
    virtual ~CalibrationReader() = default;

    virtual void read(const tl::Path &path) = 0;
    virtual auto format() const -> std::string = 0;

    auto calibrations() -> std::unordered_map<size_t, std::shared_ptr<Calibration>>;

private:

    std::unordered_map<size_t, std::shared_ptr<Calibration>> mCalibrations;

};


class CalibrationReaderFactory
{

private:

    CalibrationReaderFactory() = default;

public:

    static auto create(const std::string& format) -> std::unique_ptr<CalibrationReader>;

};



/* Calibration writer */


class CalibrationWriter
{

public:

    CalibrationWriter();
    virtual ~CalibrationWriter() = default;

    virtual void write(const tl::Path &path) = 0;
    virtual auto format() const -> std::string = 0;

    void setCalibrations(const std::unordered_map<size_t, std::shared_ptr<Calibration>> &calibrations);

private:

    std::unordered_map<size_t, std::shared_ptr<Calibration>> mCalibrations;

};

class CalibrationWriterFactory
{

private:

    CalibrationWriterFactory() = default;

public:

    static auto create(const std::string& format) -> std::unique_ptr<CalibrationWriter>;

};

} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_CALIBRATION_H
