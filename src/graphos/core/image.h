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

#ifndef GRAPHOS_CORE_IMAGE_H
#define GRAPHOS_CORE_IMAGE_H

#include <memory>
#include <map>

#include <QString>

#include <tidop/core/path.h>

#include "graphos/core/sfm/poses.h"

namespace graphos
{


class Image
{

public:

    /*!
     * \brief Default constructor for Image
     */
    Image();

    /*!
     * \brief Constructor Image
     * \param[in] file Image file
     */
    TL_DEPRECATED("Image(const tl::Path &file)", 2.0.0)
    Image(const QString &file);

    /*!
     * \brief Constructor Image
     * \param[in] file Image file path
     */
    Image(tl::Path file);

    /*!
     * \brief Copy constructor for Image
     * \param[in] image Image object to copy
     */
    Image(const Image &image);

    /*!
     * \brief Move constructor for Image
     * \param[in] image Image object to move
     */
    Image(Image &&image) noexcept;

    /*!
     * \brief Destructor for Image
     */
    ~Image() = default;

    /*!
     * \brief Returns the path of the image file
     * \return Image file path
     */
    auto path() const -> QString;
    // reemplazar QString path() por:
    //tl::Path path() const;
    
    /*!
     * \brief Sets the path of the image file
     * \param[in] file Image file path
     */
    TL_DEPRECATED("Image::setPath(const tl::Path &file)", 2.0.0)
    void setPath(const QString &file);
    
    /*!
     * \brief Sets the path of the image file
     * \param[in] file Image file path
     */
    void setPath(const tl::Path &file);

    /*!
     * \brief Returns the name of the image file
     * \return Image file name
     */
    auto name() const -> QString;

    /*!
     * \brief Returns the identifier of the camera
     * \return Camera identifier
     */
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
    auto cameraPose() const -> CameraPose;

    /*!
     * \brief Sets the position and orientation of the image
     * \param[in] cameraPose Camera pose
     */
    void setCameraPose(const CameraPose &cameraPose);

    /*!
     * \brief Adds metadata to the image
     * \param[in] key Metadata key
     * \param[in] value Metadata value
     */
    void addMetadata(const std::string &key, const std::string &value);

    /*!
     * \brief Checks if the image has metadata with the given key
     * \param[in] key Metadata key
     * \return True if the metadata exists, false otherwise
     */
    auto hasMetadata(const std::string &key) const -> bool;

    /*!
     * \brief Returns the metadata value for the given key
     * \param[in] key Metadata key
     * \return Metadata value, or empty string if the key does not exist
     */
    auto metadata(const std::string &key) const->std::string;

    auto metadata() const -> const std::map<std::string, std::string> &;

    /*!
     * \brief Assignment operator for Image
     * \param[in] image Image object to assign
     * \return Reference to the Image object
     */
    auto operator =(const Image& image) -> Image&;

    /*!
     * \brief Move assignment operator for Image
     * \param[in] image Image object to move
     * \return Reference to the Image object
     */
    auto operator =(Image&& image) noexcept -> Image&;

    /*!
     * \brief Returns the identifier of the Image
     * \param[in] image Image
     * return Image identifier
     */
    static auto id(const Image &image) -> size_t;

protected:

    tl::Path mFilePath;
    int mCameraId;
    CameraPose mCameraPose;
    std::map<std::string, std::string> mMetadata;
};

} // namespace graphos

#endif // GRAPHOS_CORE_IMAGE_H