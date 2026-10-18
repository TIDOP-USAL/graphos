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

#include "graphos/framework/mvp/Model.h"

namespace graphos
{

/*!
 * \brief Model interface for help component
 *
 * This class represents the model responsible for providing help information.
 */
class HelpModel
  : public Model
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor for HelpModel
     * \param[in] parent The parent object
     */
    HelpModel(QObject *parent = nullptr) : Model(parent) {}

    /*!
     * \brief Destructor for HelpModel
     */
    ~HelpModel() override = default;

    /*!
     * \brief Gets the base URL for help
     * \return The base URL
     */
    virtual auto urlBase() const->QString = 0;

    /*!
     * \brief Gets the URL of the home page for help
     * \return The URL of the home page
     */
    virtual auto homePage() const->QString = 0;

public slots:

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

} // namespace graphos