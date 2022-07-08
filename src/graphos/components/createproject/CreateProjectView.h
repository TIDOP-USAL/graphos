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

#ifndef GRAPHOS_CREATE_PROJECT_VIEW_INTERFACE_H
#define GRAPHOS_CREATE_PROJECT_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"


namespace graphos
{

/*!
 * \brief CreateProjectView interface
 */
class CreateProjectView
  : public DialogView
{

  Q_OBJECT

public:

  CreateProjectView(QWidget *parent) : DialogView(parent) {}
  virtual ~CreateProjectView() override = default;

  /*!
   * \brief Return the project name
   * \return Project name
   */
  virtual QString projectName() const = 0;

  /*!
   * \brief Returns the project path
   * \return Project path
   */
  virtual QString projectPath() const = 0;

  /*!
   * \brief Set the project path
   * \param path Project path
   */
  virtual void setProjectPath(const QString &path) = 0;

  /*!
   * \brief Returns the description of the project
   * \return Project description
   */
  virtual QString projectDescription() const = 0;

  /*!
   * \brief Check if a folder with the name of the project will be created
   */
  virtual bool createProjectFolderEnable() const = 0;

  /*!
   * \brief Sets if the project exists or not
   * If it already exists, the save button is disabled and the path 
   * is changed to red.
   * \param[in] prjExist
   */
  virtual void setExistingProject(bool prjExist) = 0;

protected slots:

  virtual void onClickButtonSelectPath() = 0;

signals:

  void project_name_changed();
};

} // namespace graphos


#endif // GRAPHOS_CREATE_PROJECT_VIEW_INTERFACE_H
