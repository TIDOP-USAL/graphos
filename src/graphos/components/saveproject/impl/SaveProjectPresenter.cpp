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

#include "SaveProjectPresenter.h"

#include "graphos/components/saveproject/SaveProjectModel.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

namespace graphos
{

SaveProjectPresenterImp::SaveProjectPresenterImp(SaveProjectView *view,
                                                 SaveProjectModel *model,
                                                 AppStatus *status)
  : SaveProjectPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
    SaveProjectPresenterImp::init();
    SaveProjectPresenterImp::initSignalAndSlots();
}

SaveProjectPresenterImp::~SaveProjectPresenterImp() = default;

void SaveProjectPresenterImp::open()
{
    try {

        mModel->save();

        mAppStatus->activeFlag(AppStatus::Flag::project_modified, false);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void SaveProjectPresenterImp::init()
{
}

void SaveProjectPresenterImp::initSignalAndSlots()
{
}

} // namespace graphos