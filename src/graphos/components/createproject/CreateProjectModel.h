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

#ifndef GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H
#define GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"

#include <tidop/core/path.h>

namespace graphos
{

/*!
 * \brief CreateProjectModel interface
 */
class CreateProjectModel
    : public Model
{

    Q_OBJECT

public:

    CreateProjectModel(QObject* parent = nullptr) : Model(parent) {}
    ~CreateProjectModel() override = default;

    virtual tl::Path projectsDefaultPath() const = 0;

public slots:

    /*!
     * \brief Set project name
     * \param[in] name Project's name
     */
    virtual void setProjectName(const QString& name) = 0;

    /*!
     * \brief Set project description
     * \param[in] description Project description
     */
    virtual void setProjectDescription(const QString& projectDescription) = 0;

    /*!
     * \brief Set project directory
     * \param[in] folder Project Directory
     */
    virtual void setProjectFolder(const tl::Path& folder) = 0;

    /*!
     * \brief Save the project
     */
    virtual void save() = 0;

};


} // namespace graphos

#endif // GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H

