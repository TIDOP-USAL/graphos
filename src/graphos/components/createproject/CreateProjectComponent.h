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

#ifndef GRAPHOS_CREATE_PROJECT_COMPONENT_H
#define GRAPHOS_CREATE_PROJECT_COMPONENT_H

#include "graphos/core/Component.h"

namespace graphos
{

class Project;

/*!
 * \brief Componente para la creación de proyectos
 */
class CreateProjectComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  CreateProjectComponent(Project *project,
                         Application *application);
  ~CreateProjectComponent();

private slots:

  void onProjectCreated();

signals:

  /*!
   * \brief Señal emitida cuando se crea un nuevo proyecto
   */
  void projectCreated();

// ComponentBase

protected:

  void createModel() override;
  void createView() override;
  void createPresenter() override;
  void createCommand() override;
  void update() override;

protected:

  Project *mProject;

};


} // namespace graphos


#endif // GRAPHOS_CREATE_PROJECT_COMPONENT_H
