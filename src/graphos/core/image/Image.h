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

#include <memory>
#include <map>

#include <QString>

#include <tidop/core/base/Path.h>

#include "graphos/core/orientation/CameraPose.h"

namespace graphos
{


class Image
{
public:

    using Metadata = std::map<std::string, std::string, std::less<>>;

protected:

    tl::Path mFilePath;
    int mCameraId{0};
    CameraPose mCameraPose{};
    Metadata mMetadata;

public:

    /*!
     * \brief Default constructor for Image
     */
    Image() = default;

    /*!
     * \brief Constructor Image
     * \param[in] file Image file path
     */
    explicit Image(tl::Path file);

    /*!
     * \brief Copy constructor for Image
     * \param[in] image Image object to copy
     */
    Image(const Image &image) = default;

    /*!
     * \brief Move constructor for Image
     * \param[in] image Image object to move
     */
    Image(Image &&image) noexcept = default;

    /*!
     * \brief Destructor for Image
     */
    ~Image() = default;

    /*!
     * \brief Assignment operator for Image
     * \param[in] image Image object to assign
     * \return Reference to the Image object
     */
    auto operator =(const Image &image) -> Image & = default;

    /*!
     * \brief Move assignment operator for Image
     * \param[in] image Image object to move
     * \return Reference to the Image object
     */
    auto operator =(Image &&image) noexcept -> Image & = default;

    /*!
     * \brief Returns the path of the image file
     * \return Image file path
     */
    [[nodiscard]]
    auto path() const -> const tl::Path &;
    
    /*!
     * \brief Sets the path of the image file
     * \param[in] file Image file path
     */
    void setPath(tl::Path file);

    /*!
     * \brief Returns the name of the image file
     * \return Image file name
     */
    [[nodiscard]]
    auto name() const -> QString;

    /*!
     * \brief Returns the identifier of the camera
     * \return Camera identifier
     */
    [[nodiscard]]
    auto cameraId() const -> int;

    /*!
     * \brief Sets the identifier of the camera
     * \param[in] cameraId Camera identifier
     */
    void setCameraId(int cameraId);

    /*!
     * \brief Returns the position and orientation of the image
     * \return Camera pose
     */
    auto cameraPose() -> CameraPose &;

    [[nodiscard]]
    auto cameraPose() const -> const CameraPose &;

    /*!
     * \brief Adds metadata to the image
     * \param[in] key Metadata key
     * \param[in] value Metadata value
     */
    void addMetadata(std::string key, std::string value);

    /*!
     * \brief Checks if the image has metadata with the given key
     * \param[in] key Metadata key
     * \return True if the metadata exists, false otherwise
     */
    [[nodiscard]]
    auto hasMetadata(std::string_view key) const -> bool;

    /*!
     * \brief Returns the metadata value for the given key
     * \param[in] key Metadata key
     * \return Metadata value, or empty string if the key does not exist
     */
    [[nodiscard]]
    auto metadata(std::string_view key) const -> std::string;

    auto metadata() const -> const Metadata &;

    /*!
     * \brief Returns the identifier of the Image
     * \param[in] image Image
     * return Image identifier
     */
    static auto id(const Image &image) -> size_t;

};

} // namespace graphos