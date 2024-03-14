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

#ifndef GRAPHOS_TASK_VIEW_H
#define GRAPHOS_TASK_VIEW_H

#include "graphos/core/mvp.h"


namespace graphos
{

/*!
 * \brief View for Tasks
 *
 * The `TaskView` class represents the view for tasks in the application. It inherits from the `DialogView` class.
 */
class TaskView
  : public DialogView
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor
     *
     * Constructs a `TaskView` object with the specified parent widget and window flags.
     *
     * \param[in] parent Pointer to the parent widget.
     * \param[in] f Window flags specifying the behavior of the window.
     */
    TaskView(QWidget *parent = nullptr,
             Qt::WindowFlags f = Qt::WindowFlags());

    /*!
     * \brief Destructor
     *
     * Destroys the `TaskView` object.
     */
    ~TaskView() override;

signals:

    /*!
     * \brief Signal emitted when starting a task
     *
     * This signal is emitted when the user initiates the execution of a task.
     */
    void run();

};


} // namespace graphos


#endif // GRAPHOS_TASK_VIEW_H
