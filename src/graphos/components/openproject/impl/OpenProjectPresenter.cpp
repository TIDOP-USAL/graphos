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

#include "OpenProjectPresenter.h"

#include "graphos/components/openproject/OpenProjectModel.h"
#include "graphos/components/openproject/OpenProjectView.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

OpenProjectPresenterImp::OpenProjectPresenterImp(OpenProjectView *view,
                                                 OpenProjectModel *model)
  : OpenProjectPresenter(),
    mView(view),
    mModel(model)
{
    OpenProjectPresenterImp::init();
    OpenProjectPresenterImp::initSignalAndSlots();
}

OpenProjectPresenterImp::~OpenProjectPresenterImp() = default;

void OpenProjectPresenterImp::setProjectFile(const QString &file)
{

    if (!file.isEmpty()) {
        auto status = Application::instance().status();

        if (status->isEnabled(AppStatus::Flag::project_modified)) {
            int i_ret = QMessageBox(QMessageBox::Information,
                tr("Save Changes"),
                tr("There are unsaved changes. Do you want to save them?"),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel).exec();
            if (i_ret == QMessageBox::Yes) {
                mModel->saveProject();
                status->clear();
            } else if (i_ret == QMessageBox::Cancel) {
                return;
            }
        }

        mModel->clear();
        mModel->loadProject(file.toStdWString());

        emit project_loaded();
    }
}

void OpenProjectPresenterImp::open()
{
    mView->setGraphosProjectsPath(QString::fromStdWString(mModel->graphosProjectsDirectory().toWString()));
    mView->exec();
}

void OpenProjectPresenterImp::init()
{
}

void OpenProjectPresenterImp::initSignalAndSlots()
{
    connect(mView, &OpenProjectView::fileSelected, this, &OpenProjectPresenterImp::setProjectFile);
}

} // namespace graphos