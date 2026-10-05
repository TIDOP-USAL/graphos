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
 * \brief Ground Control points writer
 *
 * The `GCPsWriter` class provides an abstract interface for writing ground control points (GCPs) to various file formats.
 * Subclasses of `GCPsWriter` must implement the `write` and `format` methods to define how GCP data is written and
 * to specify the format of the output file, respectively.
 */
class GCPsWriter
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs a `GCPsWriter` object.
     */
    GCPsWriter();

    /*!
     * \brief Virtual destructor.
     *
     * Destroys the `GCPsWriter` object. Ensures proper cleanup of derived class resources.
     */
    virtual ~GCPsWriter() = default;

    /*!
     * \brief Write ground control points to a file.
     *
     * This method must be implemented by subclasses to write the ground control points to the specified file.
     *
     * \param[in] path The path to the file where GCP data will be written.
     */
    virtual void write(const tl::Path &path) = 0;

    /*!
     * \brief Get the format of the output file.
     *
     * This method must be implemented by subclasses to specify the format of the output file.
     *
     * \return A string representing the format of the output file.
     */
    virtual auto format() const -> std::string = 0;

    /*!
     * \brief Set the ground control points to be written.
     *
     * Sets the ground control points that will be written to the output file.
     *
     * \param[in] GCPs The vector of ground control points.
     */
    void setGCPs(const std::vector<GroundControlPoint> &GCPs);

    /*!
     * \brief Set the EPSG code associated with the ground control points.
     *
     * Sets the EPSG code associated with the ground control points.
     *
     * \param[in] epsgCode The EPSG code to set.
     */
    void setEPSGCode(const std::string &epsgCode);

    /*!
     * \brief Set the images associated with the ground control points.
     *
     * Sets the images associated with the ground control points.
     *
     * \param[in] images A map containing image IDs and corresponding Image objects.
     */
    void setImages(const std::unordered_map<size_t, Image> &images);

protected:

    auto gcps() const -> const std::vector<GroundControlPoint>&;
    auto epsgCode() const -> const std::string&;
    auto images() const -> const std::unordered_map<size_t, Image>&;

private:

    std::vector<GroundControlPoint> mGCPs;         ///< Vector of ground control points to be written.
    std::string mEpsgCode;                         ///< EPSG code associated with the ground control points.
    std::unordered_map<size_t, Image> mImages;     ///< Map of image IDs and corresponding Image objects associated with the ground control points.
};




/*!
 * \brief Ground Control points writer factory
 *
 * The `GCPsWriterFactory` class provides a factory method for creating instances of `GCPsWriter` objects.
 * It allows creating a writer for ground control points (GCPs) based on the specified format.
 */
class GCPsWriterFactory
{

private:

    GCPsWriterFactory() = default;

public:

    /*!
     * \brief Create a writer for ground control points based on the specified format.
     *
     * This static method creates and returns a unique pointer to a `GCPsWriter` object based on the specified format.
     *
     * \param format The format of the output file.
     * \return A unique pointer to the created `GCPsWriter` object.
     */
    static auto create(const std::string &format) -> std::unique_ptr<GCPsWriter>;

};

} // namespace graphos
