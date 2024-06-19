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

#include "CloseProjectPresenter.h"

#include "graphos/components/closeproject/CloseProjectModel.h"
#include "graphos/components/closeproject/CloseProjectView.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

CloseProjectPresenterImp::CloseProjectPresenterImp(CloseProjectView *view,
                                                   CloseProjectModel *model,
                                                   AppStatus *status)
  : CloseProjectPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
    CloseProjectPresenterImp::init();
    CloseProjectPresenterImp::initSignalAndSlots();
}

CloseProjectPresenterImp::~CloseProjectPresenterImp() = default;

void CloseProjectPresenterImp::open()
{
    if (mAppStatus->isEnabled(AppStatus::Flag::project_modified)) {
        int i_ret = QMessageBox(QMessageBox::Information,
                                tr("Save Changes"),
                                tr("There are unsaved changes. Do you want to save the changes before closing the project?"),
                                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel).exec();
        if (i_ret == QMessageBox::Yes) {
            mModel->saveProject();
        } else if (i_ret == QMessageBox::Cancel) {
            return;
        }
    }

    mModel->clear();

    mAppStatus->clear();

    emit projectClosed();
}

void CloseProjectPresenterImp::init()
{
}

void CloseProjectPresenterImp::initSignalAndSlots()
{

}

} // namespace graphos