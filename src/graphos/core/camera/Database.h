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

#ifndef GRAPHOS_CORE_CAMERA_DATABASE_H
#define GRAPHOS_CORE_CAMERA_DATABASE_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QString>

class QSqlDatabase;

namespace colmap
{
class Reconstruction;
}

namespace graphos
{


/*!
 * \brief Camera database management
 */
class DatabaseCameras
{

public:

    DatabaseCameras(const QString &database);
    ~DatabaseCameras();

    /*!
     * \brief Open the database
     */
    void open();

    /*!
     * \brief Check if the database is open
     * \return 
     */
    auto isOpen() const -> bool;

    /*!
     * \brief Close the database
     */
    void close();

    /*!
     * \brief Check if the camera brand exists in the database.
     * \param[in] cameraMake Camera 
     * \return 
     */
    auto existCameraMakeId(const QString &cameraMake) const -> bool;

    /*!
     * \brief Check if the camera make and model exists in the database.
     * \param[in] cameraMake Camera make
     * \param[in] cameraModel Camera model
     * \return 
     */
    auto existCameraModel(int cameraMake, const QString &cameraModel) const -> bool;

    /*!
     * \brief Camera make identifier
     * \param[in] cameraMake Camera make
     * \return Identifier
     */
    auto cameraMakeId(const QString &cameraMake) const -> int;

    /*!
     * \brief Camera sensor size
     * \param[in] cameraMake Camera make
     * \param[in] cameraModel Camera model
     * \return 
     */
    auto cameraSensorSize(int cameraMake, const QString &cameraModel) const -> double;

private:

    QSqlDatabase *mDatabase;
};


} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_DATABASE_H
