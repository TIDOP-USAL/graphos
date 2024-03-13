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

#ifndef GRAPHOS_CORE_ORIENTATION_POSES_H
#define GRAPHOS_CORE_ORIENTATION_POSES_H

#include <QString>

#include <tidop/geometry/entities/point.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/quaternion.h>

namespace graphos
{

/*!
* \brief Represents the pose of a camera in 3D space
*
* The `CameraPose` class encapsulates the position and orientation of a camera in 3D space.
*/
class CameraPose
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs an empty `CameraPose` object.
     */
    CameraPose();

    /*!
     * \brief Constructor.
     *
     * Constructs a `CameraPose` object with the given coordinates and rotation matrix.
     *
     * \param[in] x X-coordinate of the camera position.
     * \param[in] y Y-coordinate of the camera position.
     * \param[in] z Z-coordinate of the camera position.
     * \param[in] rotationMatrix Rotation matrix representing the orientation of the camera.
     */
    CameraPose(double x, double y, double z,
               const tl::RotationMatrix<double> &rotationMatrix);

    /*!
     * \brief Constructor.
     *
     * Constructs a `CameraPose` object with the given center point and rotation matrix.
     *
     * \param[in] center Center point of the camera.
     * \param[in] rotationMatrix Rotation matrix representing the orientation of the camera.
     */
    CameraPose(const tl::Point3<double> &center,
               const tl::RotationMatrix<double> &rotationMatrix);

    /*!
     * \brief Constructor.
     *
     * Constructs a `CameraPose` object with the given coordinates and quaternion.
     *
     * \param[in] x X-coordinate of the camera position.
     * \param[in] y Y-coordinate of the camera position.
     * \param[in] z Z-coordinate of the camera position.
     * \param[in] quaternion Quaternion representing the orientation of the camera.
     */
    CameraPose(double x, double y, double z,
               const tl::Quaternion<double> &quaternion);

    /*!
     * \brief Constructor.
     *
     * Constructs a `CameraPose` object with the given center point and quaternion.
     *
     * \param[in] center Center point of the camera.
     * \param[in] quaternion Quaternion representing the orientation of the camera.
     */
    CameraPose(const tl::Point3<double> &center,
               const tl::Quaternion<double> &quaternion);

    ~CameraPose();

    /*!
     * \brief Get the position of the camera.
     *
     * \return The position of the camera as a 3D point.
     */
    auto position() const -> tl::Point3<double>;

    /*!
     * \brief Set the position of the camera.
     *
     * \param[in] position The position of the camera as a 3D point.
     */
    void setPosition(const tl::Point3<double> &position);

    /*!
     * \brief Get the orientation of the camera as a quaternion.
     *
     * \return The orientation of the camera as a quaternion.
     */
    auto quaternion() const -> tl::Quaterniond;

    /*!
     * \brief Set the orientation of the camera as a quaternion.
     *
     * \param[in] quaternion The orientation of the camera as a quaternion.
     */
    void setQuaternion(const tl::Quaterniond &quaternion);

    /*!
     * \brief Get the orientation of the camera as a rotation matrix.
     *
     * \return The orientation of the camera as a rotation matrix.
     */
    auto rotationMatrix() const -> tl::RotationMatrix<double>;

    /*!
     * \brief Set the orientation of the camera as a rotation matrix.
     *
     * \param[in] rotationMatrix The orientation of the camera as a rotation matrix.
     */
    void setRotationMatrix(const tl::RotationMatrix<double> &rotationMatrix);

    /*!
     * \brief Get the coordinate reference system (CRS) of the camera.
     *
     * \return The CRS of the camera as EPSG code.
     */
    auto crs() const -> QString;

    /*!
      * \brief Set the coordinate reference system (CRS) of the camera.
      *
      * \param crs The CRS of the camera as EPSG code.
      */
    void setCrs(const QString &crs);

    /*!
     * \brief Get the source of the camera pose data.
     *
     * \return The source of the camera pose data as a string.
     */
    auto source() const -> QString;

    /*!
     * \brief Establece el fichero fuente con las posiciones/orientaciones o EXIF
     * \param[in] source
     */
     /*!
      * \brief Sets the file with the orientation data or the `EXIF` string if read from the camera metadata.
      *
      * \param[in] source The source of the camera pose data as a string.
      */
    void setSource(const QString &source);

    /*!
     * \brief Check if the camera pose is empty.
     *
     * \return True if the camera pose is empty, false otherwise.
     */
    auto isEmpty() const -> bool;

private:

    tl::Point3<double> mPosition;
    std::shared_ptr<tl::Orientation> mRotation;
    QString mCrs;
    QString mSource;
};


} // namespace graphos

#endif // GRAPHOS_CORE_PHOTO_ORIENTATION_H
