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

#pragma once

#include "graphos/graphos_global.h"

#include <memory>
#include <unordered_map>

#include <tidop/core/base/Path.h>
#include <tidop/geometry/primitives/Point.h>
#include <tidop/graphic/Color.h>

#include "graphos/core/orientation/Track.h"
#include "graphos/core/orientation/GroundControlPoint.h"
#include "graphos/core/image/Image.h"


namespace graphos
{


/*!
 * \brief Ground control points reader
 *
 * This class represents a reader for ground control points (GCPs). It provides
 * an interface for reading GCP data from a specified file format.
 */
class GCPsReader
{

public:

    /*!
     * \brief Default constructor.
     */
    GCPsReader();

    /*!
     * \brief Destructor.
     */
    virtual ~GCPsReader() = default;

    /*!
     * \brief Read ground control points from a file.
     * \param[in] path The path to the file containing GCP data.
     */
    virtual void read(const tl::Path &path) = 0;

    /*!
     * \brief Get the format of the GCP file.
     * \return The format of the GCP file.
     */
    virtual auto format() const -> std::string = 0;

    /*!
     * \brief Get the vector of ground control points.
     * \return The vector of ground control points.
     */
    auto gcps() const -> std::vector<GroundControlPoint>;

    /*!
     * \brief Get the EPSG code associated with the GCPs.
     * \return The EPSG code associated with the GCPs.
     */
    auto epsgCode() const -> std::string;

    /*!
     * \brief Set the images associated with the GCPs.
     * \param[in] images A map containing image IDs and corresponding Image objects.
     */
    void setImages(const std::unordered_map<size_t, Image> &images);

protected:

    void addGroundControlPoint(const GroundControlPoint &gcp);
    void setEPSGCode(const std::string &epsgCode);
    auto images() const -> const std::unordered_map<size_t, Image>&;

private:

    std::vector<GroundControlPoint> mGCPs;       ///< Vector of ground control points.
    std::string mEpsgCode;                       ///< EPSG code associated with the GCPs.
    std::unordered_map<size_t, Image> mImages;   ///< Map of image IDs and corresponding Image objects.
};





/*!
 * \brief Ground control points reader factory
 *
 * This class represents a factory for creating instances of GCPsReader objects.
 * It provides a static method to create a GCPsReader based on the specified format.
 */
class GCPsReaderFactory
{

private:

    GCPsReaderFactory() = default;

public:

    /*!
     * \brief Create a GCPsReader instance based on the specified format.
     * \param[in] format The format of the GCPs file.
     * \return A unique pointer to the created GCPsReader object.
     */
    static auto create(const std::string &format) -> std::unique_ptr<GCPsReader>;

};

} // namespace graphos
