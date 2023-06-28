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

#ifndef GRAPHOS_RECENT_PROJECTS_PRESENTER_INTERFACE_H
#define GRAPHOS_RECENT_PROJECTS_PRESENTER_INTERFACE_H

#include "graphos/core/mvp.h"

namespace graphos
{

/*!
 * \brief RecentProjectsPresenter interface
 */
class RecentProjectsPresenter
  : public Presenter
{

  Q_OBJECT

public:

  RecentProjectsPresenter(){}
  virtual ~RecentProjectsPresenter() override = default;

protected slots:

  virtual void clearHistory() = 0;

signals:

  /*!
   * \brief Signal emitted when the project is loaded
   */
  void project_loaded();
  void open_project(QString);
};

} // namespace graphos

#endif // GRAPHOS_RECENT_PROJECTS_PRESENTER_INTERFACE_H
