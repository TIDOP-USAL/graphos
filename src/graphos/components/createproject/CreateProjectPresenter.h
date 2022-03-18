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

#ifndef GRAPHOS_CREATE_PROJECT_PRESENTER_INTERFACE_H
#define GRAPHOS_CREATE_PROJECT_PRESENTER_INTERFACE_H

#include <memory>

#include <QObject>

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class HelpDialog;

/*!
 * \brief CreateProjectPresenter interface
 */
class CreateProjectPresenter
  : public Presenter
{

  Q_OBJECT

public:

  CreateProjectPresenter(){}
  virtual ~CreateProjectPresenter() override = default;

signals:

  /*!
   * \brief Signal emitted when the project is created
   */
  void projectCreate();

protected slots:

  /*!
   * \brief Save the project
   */
  virtual void saveProject() = 0;

  /*!
   * \brief Discard the changes in the project
   */
  virtual void discartProject() = 0;

  /*!
   * \brief Check if a project with the same name already exists
   */
  virtual void checkProjectName() const = 0;

};

} // namespace graphos


#endif // GRAPHOS_CREATE_PROJECT_PRESENTER_INTERFACE_H
