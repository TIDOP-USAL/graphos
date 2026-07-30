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

#include "graphos/core/sfm/poses.h"

namespace graphos
{

/*!
 * \brief Reader for camera poses
 *
 * The `CameraPosesReader` class provides an interface for reading camera poses from different sources. It defines
 * methods for reading camera poses from a specific path and format. Subclasses of `CameraPosesReader` implement
 * the actual reading logic for different file formats.
 */
class CameraPosesReader
{

public:

    /*!
     * \brief Default constructor.
     *
     * Constructs a `CameraPosesReader` object.
     */
    CameraPosesReader();
    virtual ~CameraPosesReader() = default;

    /*!
     * \brief Read camera poses from a specific path.
     *
     * This method should be implemented by subclasses to read camera poses from a specific file format.
     *
     * \param[in] path The path to the file containing camera poses.
     */
    virtual void read(const tl::Path &path) = 0;

    /*!
     * \brief Get the format of the camera poses.
     *
     * This method should be implemented by subclasses to return the format of the camera poses.
     *
     * \return A string representing the format of the camera poses.
     */
    virtual auto format() const -> std::string = 0;

    /*!
     * \brief Get the camera poses read by the reader.
     *
     * \return An unordered map containing image IDs and corresponding camera poses.
     */
    auto cameraPoses() const -> std::unordered_map<size_t, CameraPose>;

protected:

    void addCameraPose(size_t imageId,
                       const CameraPose &cameraPoses);

private:

    std::unordered_map<size_t, CameraPose> mCameraPoses;

};


/*!
 * \brief Factory for creating camera poses readers
 *
 * The `CameraPosesReaderFactory` class provides a static method to create instances of camera poses readers based on
 * a specified format. It encapsulates the creation logic and allows for dynamic selection of the appropriate reader
 * based on the format provided.
 */
class CameraPosesReaderFactory
{

private:

    CameraPosesReaderFactory() = default;

public:

    /*!
     * \brief Create a camera poses reader based on the specified format.
     *
     * This method creates an instance of a camera poses reader corresponding to the given format.
     *
     * \param[in] format The format of the camera poses to be read.
     * \return A unique pointer to the created camera poses reader.
     */
    static auto create(const std::string& format) -> std::unique_ptr<CameraPosesReader>;

};

} // namespace graphos