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

#ifndef GRAPHOS_CAMERAS_PRESENTER_INTERFACE_H
#define GRAPHOS_CAMERAS_PRESENTER_INTERFACE_H

#include "graphos/core/mvp.h"

namespace graphos
{


/*!
 * \brief Cameras presenter interface
 */
class CamerasPresenter
  : public Presenter
{

    Q_OBJECT

public:

    CamerasPresenter() = default;
    ~CamerasPresenter() override = default;

public slots:

    /*!
     * \brief Slot to set the active camera.
     * \param[in] id The ID of the active camera.
     */
    virtual void activeCamera(int id) = 0;

protected slots:

    /*!
     * \brief save Slot to save data.
     */
    virtual void save() = 0;

    /*!
     * \brief discart Slot to discard changes.
     */
    virtual void discard() = 0;

signals:

    /*!
     * \brief Signal emitted to indicate the need for updating cameras.
     */
    void updateCameras();

};

} // namespace graphos

#endif // GRAPHOS_CAMERAS_PRESENTER_INTERFACE_H
