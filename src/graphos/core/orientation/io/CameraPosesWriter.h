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

#include <string>
#include <unordered_map>

#include <tidop/core/base/Path.h>

#include "graphos/core/orientation/CameraPose.h"

namespace graphos
{

/*!
 * \brief Writer for camera poses
 *
 * The `CameraPosesWriter` class provides an interface for writing camera poses to different file formats. It defines
 * methods for writing camera poses to a specific path and format. Subclasses of `CameraPosesWriter` implement
 * the actual writing logic for different file formats.
 */
class CameraPosesWriter
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs a `CameraPosesWriter` object.
     */
    CameraPosesWriter();
    virtual ~CameraPosesWriter() = default;

    /*!
     * \brief Write camera poses to a specific path.
     *
     * This method should be implemented by subclasses to write camera poses to a specific file format.
     *
     * \param path The path to write the camera poses to.
     */
    virtual void write(const tl::Path &path) = 0;

    /*!
     * \brief Get the format of the camera poses.
     *
     * This method should be implemented by subclasses to return the format of the camera poses.
     *
     * \return A string representing the format of the camera poses.
     */
    virtual auto format() const -> std::string = 0;

    /*!
     * \brief Set the camera poses to be written.
     *
     * \param[in] cameraPoses The camera poses to be written, stored as an unordered map of image IDs and camera poses.
     */
    void setCameraPoses(const std::unordered_map<size_t, CameraPose> &cameraPoses);

protected:

    std::unordered_map<size_t, CameraPose> cameraPoses() const;

private:

    std::unordered_map<size_t, CameraPose> mCameraPoses;
};




/*!
 * \brief Factory for creating camera poses writers
 *
 * The `CameraPosesWriterFactory` class provides a static method to create instances of camera poses writers based on
 * a specified format. It encapsulates the creation logic and allows for dynamic selection of the appropriate writer
 * based on the format provided.
 */
class CameraPosesWriterFactory
{

private:

    CameraPosesWriterFactory() = default;

public:

    /*!
     * \brief Create a camera poses writer based on the specified format.
     *
     * This method creates an instance of a camera poses writer corresponding to the given format.
     *
     * \param[in] format The format of the camera poses to be written.
     * \return A unique pointer to the created camera poses writer.
     */
    static std::unique_ptr<CameraPosesWriter> create(const std::string &format);

};

} // namespace graphos
