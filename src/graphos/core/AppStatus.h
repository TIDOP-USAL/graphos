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

#ifndef GRAPHOS_APP_STATUS_H
#define GRAPHOS_APP_STATUS_H

#include "graphos/graphos_global.h"

#include <tidop/core/flags.h>

#include <QObject>

#include <memory>


namespace graphos
{


/*!
 * \brief Application Status
 *
 * The `AppStatus` class represents the status of the application, indicating various flags and states.
 * It inherits from `QObject`.
 */
class AppStatus
  : public QObject
{

    Q_OBJECT

public:

    enum class Flag : uint64_t
    {
        none = (0 << 0),                   /*!< No flag */
        project_exists = (1 << 0),         /*!< Project exists */
        project_modified = (1 << 1),       /*!< Project has been modified */
        images_added = (1 << 2),           /*!< Images have been added */
        image_open = (1 << 3),             /*!< An image is open */
        feature_extraction = (1 << 4),     /*!< Feature extraction is in progress */
        feature_matching = (1 << 5),       /*!< Feature matching is in progress */
        oriented = (1 << 6),               /*!< Orientation is completed */
        absolute_oriented = (1 << 7),      /*!< Absolute orientation is completed */
        dense_model = (1 << 8),            /*!< Dense model reconstruction is completed */
        dtm = (1 << 9),                    /*!< DTM (Digital Terrain Model) generation is completed */
        dsm = (1 << 10),                   /*!< DSM (Digital Surface Model) generation is completed */
        ortho = (1 << 11),                 /*!< Orthoimage generation is completed */
        mesh = (1 << 12),                  /*!< Mesh reconstruction is completed */
        processing = (1 << 20),            /*!< Processing is in progress */
        loading_images = (1 << 21),        /*!< Images are being loaded */
        tab_image_active = (1 << 22),      /*!< Image tab is active */
        tab_3d_viewer_active = (1 << 23),  /*!< 3D viewer tab is active */
        //sparse_model_open     = (1 << 24),
        //dense_model_open      = (1 << 25),
        //mesh_model_open       = (1 << 26),
        //loading_thumbs        = (1 << 27),
        command_mode = (1 << 30)           /*!< Command mode is active */
    };

public:

    /*!
     * \brief Constructor
     *
     * Constructs an `AppStatus` object.
     */
    AppStatus();

    /*!
     * \brief Destructor
     *
     * Destroys the `AppStatus` object.
     */
    ~AppStatus() override;

    TL_DISABLE_COPY(AppStatus)
    TL_DISABLE_MOVE(AppStatus)

    /* Graphos Flags */

    /*!
     * \brief Activate or deactivate a specific flag
     *
     * Sets the specified flag to active or inactive.
     *
     * \param[in] flag The flag to modify
     * \param[in] active Whether to activate or deactivate the flag
     */
    void activeFlag(Flag flag, bool active);

    /*!
     * \brief Check if a specific flag is enabled
     *
     * Checks if the specified flag is enabled.
     *
     * \param[in] flag The flag to check
     * \return True if the flag is enabled, false otherwise
     */
    auto isEnabled(Flag flag) const -> bool;

    /*!
     * \brief Enable a specific flag
     *
     * Enables the specified flag.
     *
     * \param[in] flag The flag to enable
     */
    void enable(Flag flag);

    /*!
     * \brief Disable a specific flag
     *
     * Disables the specified flag.
     *
     * \param[in] flag The flag to disable
     */
    void disable(Flag flag);

    /*!
     * \brief Switch the state of a specific flag
     *
     * Switches the state of the specified flag.
     *
     * \param[in] flag The flag to switch
     */
    void switchFlag(Flag flag);

    /* User flags */

    /*!
     * \brief Activate or deactivate a specific user flag
     *
     * Sets the specified user flag to active or inactive.
     *
     * \param[in] flag The user flag to modify
     * \param[in] active Whether to activate or deactivate the user flag
     */
    void activeFlag(uint32_t flag, bool active);

    /*!
     * \brief Check if a specific user flag is enabled
     *
     * Checks if the specified user flag is enabled.
     *
     * \param[in] flag The user flag to check
     * \return True if the user flag is enabled, false otherwise
     */
    bool isEnabled(uint32_t flag) const;

    /*!
     * \brief Enable a specific user flag
     *
     * Enables the specified user flag.
     *
     * \param[in] flag The user flag to enable
     */
    void enable(uint32_t flag);

    /*!
     * \brief Disable a specific user flag
     *
     * Disables the specified user flag.
     *
     * \param[in] flag The user flag to disable
     */
    void disable(uint32_t flag);

    /*!
     * \brief Switch the state of a specific user flag
     *
     * Switches the state of the specified user flag.
     *
     * \param[in] flag The user flag to switch
     */
    void switchFlag(uint32_t flag);

    /*!
     * \brief Clear all flags
     *
     * Clears all application and user flags.
     */
    void clear();

signals:

    void update();

private:

    tl::EnumFlags<Flag> mFlags;
    tl::Flags<uint32_t> mUserFlags;

};
ALLOW_BITWISE_FLAG_OPERATIONS(AppStatus::Flag)

} // namespace graphos


#endif // GRAPHOS_APP_STATUS_H
