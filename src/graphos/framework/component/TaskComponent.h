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

#include "graphos/framework/component/ComponentBase.h"

namespace graphos
{

class ProgressHandler;
class Application;

/*!
 * \brief Component that executes a task
 */
class TaskComponent
  : public ComponentBase
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor
     * \param application Pointer to the application instance
     */
    TaskComponent(Application *application);

    /*!
     * \brief Destructor
     */
    ~TaskComponent() override = default;

public slots:

    /*!
     * \brief Set the progress handler
     * \param[in] progressHandler Progress handler
     */
    virtual void setProgressHandler(ProgressHandler *progressHandler);

private slots:

    /*!
     * \brief Slot invoked when the component is created
     */
    void onComponentCreated();

protected slots:

    /*!
     * \brief Slot invoked when the task is running
     */
    virtual void onRunning();

    /*!
     * \brief Slot invoked when the task is finished
     */
    virtual void onFinished();

    /*!
     * \brief Slot invoked when the task fails
     */
    virtual void onFailed();

    /*!
     * \brief Slot invoked when the task is canceled
     */
    virtual void onCanceled();

signals:

    /*!
     * \brief Signal emitted when the task is running
     */
    void running();

    /*!
     * \brief Signal emitted when the task is finished
     */
    void finished();

    /*!
     * \brief Signal emitted when the task fails
     */
    void failed();

    /*!
     * \brief Signal emitted when the task is canceled
     */
    void canceled();

private:

    ProgressHandler *mProgressHandler;
};

} // namespace graphos