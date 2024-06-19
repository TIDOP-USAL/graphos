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

#ifndef GRAPHOS_TASK_PRESENTER_H
#define GRAPHOS_TASK_PRESENTER_H

#include <QObject>

#include <tidop/core/task.h>

#include "graphos/core/mvp.h"

namespace tl
{
class TaskErrorEvent;
class TaskFinalizedEvent;
}


namespace graphos
{

class ProgressHandler;

/*!
 * \brief Task presenter
 *
 * The `TaskPresenter` class is responsible for presenting and managing a task's execution.
 * It inherits from the Presenter class and provides methods for handling task events, setting up progress handling,
 * creating and running tasks, and emitting signals to indicate task status.
 */
class TaskPresenter
  : public Presenter
{

    Q_OBJECT

public:

    /*!
     * \brief Default constructor
     *
     * Constructs a `TaskPresenter` object.
     */
    TaskPresenter();
    ~TaskPresenter() override;

protected:

    /*!
     * \brief Error handler for task events
     *
     * This method is called when an error event occurs during task execution.
     *
     * \param[in] event The error event.
     */
    virtual void onError(tl::TaskErrorEvent *event);

    /*!
     * \brief Finished handler for task events
     *
     * This method is called when the task finishes successfully.
     *
     * \param[in] event The finished event.
     */
    virtual void onFinished(tl::TaskFinalizedEvent *event);

    /*!
     * \brief Stopped handler for task events
     *
     * This method is called when the task is stopped.
     *
     * \param[in] event The stopped event.
     */
    virtual void onStopped(tl::TaskStoppedEvent *event);

    /*!
     * \brief Get the progress handler
     *
     * This method returns a pointer to the progress handler used by the task presenter.
     *
     * \return A pointer to the progress handler.
     */
    auto progressHandler() const -> ProgressHandler*;

    /*!
     * \brief Create task
     *
     * This method creates a new task.
     *
     * \return A unique pointer to the created task.
     */
    virtual auto createTask() -> std::unique_ptr<tl::Task> = 0;

public slots:

    /*!
     * \brief Set the progress handler
     *
     * This slot sets the progress handler for the task presenter.
     *
     * \param[in] progressHandler Pointer to the progress handler.
     */
    virtual void setProgressHandler(ProgressHandler *progressHandler);

    /*!
     * \brief Run the task
     *
     * This slot starts the execution of the task.
     */
    virtual void run();

    /*!
     * \brief Cancel the task
     *
     * This slot cancels the execution of the task.
     */
    virtual void cancel();

signals:

    /*!
     * \brief Signal emitted when the task is running
     */
    void running();

    /*!
     * \brief Signal emitted when the task finishes successfully
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

    void init() override;
    void initSignalAndSlots() override;

private:

    std::unique_ptr<tl::Task> mProcess;
    ProgressHandler *mProgressHandler;
};


} // namespace graphos


#endif // GRAPHOS_TASK_PRESENTER_H
