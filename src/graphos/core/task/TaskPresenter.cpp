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

#include "graphos/core/task/TaskPresenter.h"
#include "graphos/core/task/Progress.h"

#include <tidop/core/progress.h>

#include <QStandardPaths>
#include <QDir>

namespace graphos
{

TaskPresenter::TaskPresenter()
  : mProcess(nullptr),
    mProgressHandler(nullptr)
{
    TaskPresenter::init();
    TaskPresenter::initSignalAndSlots();
}

TaskPresenter::~TaskPresenter() = default;

void TaskPresenter::onError(tl::TaskErrorEvent *event)
{
    if (mProgressHandler) {
        mProgressHandler->finish();
        mProgressHandler->reset();
        mProgressHandler->setDescription(QString::fromStdString(event->errorMessage()));
        mProgressHandler->setRange(0, 100);

        disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
    }

    emit failed();
}

void TaskPresenter::onFinished(tl::TaskFinalizedEvent *event)
{
    tl::unusedParameter(event);

    if (mProgressHandler) {
        mProgressHandler->finish();
        mProgressHandler->reset();
        mProgressHandler->setDescription(tr("Process Finished"));

        disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
    }

    emit finished();
}

void TaskPresenter::onStopped(tl::TaskStoppedEvent *event)
{
    tl::unusedParameter(event);

    if (mProgressHandler) {
        mProgressHandler->finish();
        mProgressHandler->reset();
        mProgressHandler->setDescription(tr("Process canceled"));

        disconnect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
    }

    emit canceled();
}

auto TaskPresenter::progressHandler() const -> ProgressHandler*
{
    return mProgressHandler;
}

void TaskPresenter::setProgressHandler(ProgressHandler *progressHandler)
{
    mProgressHandler = progressHandler;
}

void TaskPresenter::run()
{
    try {

        tl::Task *p = mProcess.release();
        delete p;
        p = nullptr;

        mProcess = createTask();

        TL_ASSERT(mProcess, "Empty process");

        mProcess->subscribe([this](tl::TaskErrorEvent *event)
        {
            onError(event);
        });

        mProcess->subscribe([this](tl::TaskFinalizedEvent *event)
        {
            onFinished(event);
        });

        mProcess->subscribe([this](tl::TaskStoppedEvent *event)
        {
            onStopped(event);
        });

        if (mProgressHandler) {
            connect(mProgressHandler, SIGNAL(cancel()), this, SLOT(cancel()));
            mProgressHandler->init();
        }

        emit running();

        mProcess->runAsync(mProgressHandler);

    } catch (const std::exception &e) {
        tl::printException(e);
    }
}

void TaskPresenter::cancel()
{
    if (mProcess) {
        mProcess->stop();
        if (mProgressHandler) {
            mProgressHandler->setDescription(tr("Stopping process"));
        }
    }
}

void TaskPresenter::init()
{
}

void TaskPresenter::initSignalAndSlots()
{
}


} // namespace graphos
