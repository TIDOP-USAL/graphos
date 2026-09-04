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

#include "graphos/framework/component/TaskComponent.h"
#include "graphos/framework/app/Application.h"
#include "graphos/framework/task/TaskPresenter.h"
#include "graphos/framework/command/Command.h"
#include "graphos/core/AppStatus.h"

//#include <tidop/core/base/Meta.h>

namespace graphos
{


/* Task Component */

TaskComponent::TaskComponent(Application *application)
  : ComponentBase(application),
    mProgressHandler(nullptr)
{
    connect(this, &Component::created,
            this, &TaskComponent::onComponentCreated);
}

void TaskComponent::setProgressHandler(ProgressHandler *progressHandler)
{
    mProgressHandler = progressHandler;
}

void TaskComponent::onComponentCreated()
{
    Presenter *presenter = this->presenter();
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::running,
            this, &TaskComponent::onRunning);
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::finished,
            this, &TaskComponent::onFinished);
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::failed,
            this, &TaskComponent::onFailed);
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::canceled,
            this, &TaskComponent::onCanceled);

    dynamic_cast<TaskPresenter *>(presenter)->setProgressHandler(mProgressHandler);
}

void TaskComponent::onRunning()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, true);
}

void TaskComponent::onFinished()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, false);

    emit finished();
}

void TaskComponent::onFailed()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, false);
}

void TaskComponent::onCanceled()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, false);
}

} // namespace graphos
