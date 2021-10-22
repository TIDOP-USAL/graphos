/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H
#define GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class CreateProjectModel
  : public Model
{

  Q_OBJECT

public:

  CreateProjectModel(QObject *parent = nullptr) : Model(parent) {}
  ~CreateProjectModel() override = default;

  /*!
   * \brief Returns project name
   * \return Project name
   */
  virtual QString projectName() const = 0;

  /*!
   * \brief Returns project description
   * \return Project description
   */
  virtual QString projectDescription() const = 0;

  /*!
   * \brief Returns project directory
   * \return Project directory
   */
  virtual QString projectFolder() const = 0;

  /*!
   * \brief Returns project file path
   * \return Project directory
   */
  virtual QString projectPath() const = 0;

public slots:

  /*!
   * \brief Set project name
   * \param[in] name Project's name
   */
  virtual void setProjectName(const QString &name) = 0;

  /*!
   * \brief Set project description
   * \param[in] description Project description
   */
  virtual void setProjectDescription(const QString &projectDescription) = 0;

  /*!
   * \brief Set project directory
   * \param[in] path Project Directory
   */
  virtual void setProjectFolder(const QString &dir) = 0;

  /*!
   * \brief Set project database
   * \param[in] path Database path
   */
  virtual void setDatabase(const QString &database) = 0;

  /*!
   * \brief Save a project as ...
   * \param[in] file Project file
   */
  virtual void saveAs(const QString &file) = 0;

};



} // namespace graphos

#endif // GRAPHOS_CREATE_PROJECT_MODEL_INTERFACE_H

