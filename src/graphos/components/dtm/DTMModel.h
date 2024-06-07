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

#ifndef GRAPHOS_DTM_MODEL_INTERFACE_H
#define GRAPHOS_DTM_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"

#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>

namespace graphos
{


/*!
 * \brief The DtmModel class provides an interface for accessing Digital Terrain Model (DTM) data.
 */
class DtmModel
  : public Model
{

public:

    /*!
     * \brief Constructor for DtmModel.
     * \param[in] parent The parent QObject (default is nullptr).
     */
    DtmModel(QObject *parent = nullptr) : Model(parent) {}

    /*!
     * \brief Destructor for DtmModel.
     */
    ~DtmModel() override = default;

    /*!
     * \brief Retrieves the offset values.
     * \return An array containing the offset values [x, y, z].
     */
    virtual auto offset() const -> tl::Point3<double> = 0;

    /*!
     * \brief Retrieves the path to the project folder.
     * \return The path to the project folder.
     */
    virtual auto projectPath() const -> tl::Path = 0;

    /*!
     * \brief Retrieves the path to the dense model.
     * \return The path to the dense model.
     */
    virtual auto denseModel() const -> tl::Path = 0;

    /*!
     * \brief Retrieves the Coordinate Reference System (CRS) information.
     * \return The CRS information as a string.
     */
    virtual auto crs() const -> QString = 0;

    /*!
     * \brief Retrieves the Ground Sample Distance (GSD).
     * \return The GSD value.
     */
    virtual auto gsd() const -> double = 0;

    /*!
     * \brief Retrieves the path to the Digital Terrain Model (DTM) file.
     * \return The path to the DTM file.
     */
    virtual auto dtmPath() const -> tl::Path = 0;

    /*!
     * \brief Retrieves the path to the Digital Surface Model (DSM) file.
     * \return The path to the DSM file.
     */
    virtual auto dsmPath() const -> tl::Path = 0;

    /*!
     * \brief Sets the Ground Sample Distance (GSD).
     * \param[in] gsd The GSD value to set.
     */
    virtual void setGsd(double gsd) = 0;

    /*!
     * \brief Sets the path to the Digital Terrain Model (DTM) file.
     * \param[in] dtmPath The path to the DTM file.
     */
    virtual void setDtmPath(const tl::Path &dtmPath) = 0;

    /*!
     * \brief Sets the path to the Digital Surface Model (DSM) file.
     * \param[in] dsmPath The path to the DSM file.
     */
    virtual void setDsmPath(const tl::Path &dsmPath) = 0;
};

} // namespace graphos

#endif // GRAPHOS_DTM_MODEL_INTERFACE_H
