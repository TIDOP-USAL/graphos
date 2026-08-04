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


namespace graphos
{

/*!
 * \brief Ground Points
 *
 * The `GroundPoint` class represents a point on the ground in a three-dimensional space.
 * It inherits from `tl::Point3d`, providing x, y, and z coordinates.
 */
class GroundPoint
  : public tl::Point3d
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs a `GroundPoint` object with default values.
     */
    GroundPoint();

    /*!
     * \brief Constructor with a 3D point.
     *
     * Constructs a `GroundPoint` object with the given 3D point.
     *
     * \param[in] point3d A point in three-dimensional space.
     */
    explicit GroundPoint(const tl::Point3d &point3d);
    ~GroundPoint();

    /*!
     * \brief Set the point coordinates.
     *
     * Sets the coordinates of the ground point.
     *
     * \param[in] point The point to set.
     */
    void setPoint(const tl::Point3d &point);

    /*!
     * \brief Get the color of the ground point.
     *
     * \return The color of the ground point.
     * \see Color
     */
    auto color() const -> tl::Color;

    /*!
     * \brief Set the color of the ground point.
     *
     * Sets the color of the ground point.
     *
     * \param[in] color The color to set.
     * \see Color
     */
    void setColor(const tl::Color &color);

    /*!
     * \brief Add a pair of identifiers to the track of the ground point.
     *
     * Adds a pair of identifiers representing an image ID and a point ID to the track of the ground point.
     *
     * \param[in] imageId The ID of the image.
     * \param[in] pointId The ID of the point.
     */
    void addPairToTrack(size_t imageId,
                        size_t pointId);

    /*!
     * \brief Set the track of the ground point.
     *
     * Sets the track of the ground point.
     *
     * \param[in] track The track to set.
     * \see Track
     */
    void setTrack(const Track &track);

    /*!
     * \brief Remove a pair from the track of the ground point.
     *
     * Removes a pair of identifiers representing an image ID and a point ID from the track of the ground point.
     *
     * \param[in] imageId The ID of the image.
     */
    void removeTrackPair(size_t imageId);

    /*!
     * \brief Get the track of the ground point.
     *
     * \return The track of the ground point.
     * \see Track
     */
    auto track() const -> const Track&;

private:

    tl::Color mColor;
    Track mTrack;

};




/*!
 * \brief Ground points reader
 *
 * The `GroundPointsReader` class represents a reader for ground points. It provides an abstract interface for reading
 * ground point data from various file formats. Subclasses of `GroundPointsReader` must implement the `read` and `format`
 * methods to define how point data is read and to specify the format of the input file, respectively.
 */
class GroundPointsReader
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs a `GroundPointsReader` object.
     */
    GroundPointsReader();
    virtual ~GroundPointsReader() = default;

    /*!
     * \brief Read ground points from a file.
     *
     * This method must be implemented by subclasses to read ground points from the specified file.
     *
     * \param[in] path The path to the file containing ground point data.
     */
    virtual void read(const tl::Path &path) = 0;

    /*!
     * \brief Get the format of the input file.
     *
     * This method must be implemented by subclasses to specify the format of the input file.
     *
     * \return A string representing the format of the input file.
     */
    virtual auto format() const -> std::string = 0;

    /*!
     * \brief Get the vector of ground points.
     *
     * \return The vector of ground points.
     */
    auto points() const -> std::vector<GroundPoint>;

    /*!
     * \brief Get the EPSG code associated with the ground points.
     *
     * \return The EPSG code associated with the ground points.
     */
    auto epsgCode() const -> std::string;

protected:

    void addGroundPoint(const GroundPoint &groundPoint);
    void setGroundPoints(const std::vector<GroundPoint> &groundPoint);
    void setEPSGCode(const std::string &epsgCode);

private:

    std::vector<GroundPoint> mGroundPoints;
    std::string mEpsgCode;
};




/*!
 * \brief Ground points reader factory
 *
 * The `GroundPointsReaderFactory` class provides a factory method for creating instances of `GroundPointsReader` objects.
 * It allows creating a reader for ground points based on the specified format.
 */
class GroundPointsReaderFactory
{

private:

    GroundPointsReaderFactory() = default;

public:

    /*!
     * \brief Create a reader for ground points based on the specified format.
     *
     * This static method creates and returns a unique pointer to a `GroundPointsReader` object based on the specified format.
     *
     * \param[in] format The format of the input file.
     * \return A unique pointer to the created `GroundPointsReader` object.
     */
    static auto create(const std::string& format) -> std::unique_ptr<GroundPointsReader>;

};




/*!
 * \brief Ground points writer
 *
 * The `GroundPointsWriter` class represents a writer for ground points. It provides an abstract interface for writing
 * ground point data to various file formats. Subclasses of `GroundPointsWriter` must implement the `write` and `format`
 * methods to define how point data is written and to specify the format of the output file, respectively.
 */
class GroundPointsWriter
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs a `GroundPointsWriter` object.
     */
    GroundPointsWriter();

    /*!
     * \brief Virtual destructor.
     *
     * Destroys the `GroundPointsWriter` object. Ensures proper cleanup of derived class resources.
     */
    virtual ~GroundPointsWriter() = default;

    /*!
     * \brief Write ground points to a file.
     *
     * This method must be implemented by subclasses to write ground points to the specified file.
     *
     * \param[in] path The path to the file where ground point data will be written.
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
     * \brief Set the ground points to be written.
     *
     * Sets the ground points that will be written to the output file.
     *
     * \param[in] groundPoint The vector of ground points.
     */
    void setGroundPoints(const std::vector<GroundPoint> &groundPoint);

    /*!
     * \brief Set the EPSG code associated with the ground points.
     *
     * Sets the EPSG code associated with the ground points.
     *
     * \param[in] epsgCode The EPSG code to set.
     */
    void setEPSGCode(const std::string &epsgCode);

protected:

    auto groundPoints() const -> std::vector<GroundPoint>;
    auto epsgCode() const -> std::string;

private:

    std::vector<GroundPoint> mGroundPoints;
    std::string mEpsgCode;
};



/*!
 * \brief Ground points writer factory
 *
 * The `GroundPointsWriterFactory` class provides a factory method for creating instances of `GroundPointsWriter` objects.
 * It allows creating a writer for ground points based on the specified format.
 */
class GroundPointsWriterFactory
{

private:

    GroundPointsWriterFactory() = default;

public:

    /*!
     * \brief Create a writer for ground points based on the specified format.
     *
     * This static method creates and returns a unique pointer to a `GroundPointsWriter` object based on the specified format.
     *
     * \param[in] format The format of the output file.
     * \return A unique pointer to the created `GroundPointsWriter` object.
     */
    static auto create(const std::string &format) -> std::unique_ptr<GroundPointsWriter>;

};



} // namespace graphos