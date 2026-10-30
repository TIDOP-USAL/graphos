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

#include "ExportMeshPresenter.h"

#include "graphos/components/export/mesh/ExportMeshModel.h"
#include "graphos/components/export/mesh/ExportMeshView.h"
#include "graphos/framework/app/Application.h"
#include "graphos/core/AppStatus.h"

#include <tidop/core/base/Exception.h>

#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

ExportMeshPresenterImp::ExportMeshPresenterImp(ExportMeshView *view,
                                               ExportMeshModel *model,
                                               AppStatus *status)
  : ExportMeshPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
    ExportMeshPresenterImp::init();
    ExportMeshPresenterImp::initSignalAndSlots();
}

ExportMeshPresenterImp::~ExportMeshPresenterImp()
{
}

void ExportMeshPresenterImp::open()
{
    try {

        if (mModel->isProjectScaled()) {
            int i_ret = QMessageBox(QMessageBox::Information,
                QApplication::translate("ExportMeshPresenter", "Scaled Project"),
                QApplication::translate("ExportMeshPresenter",
                    "The project currently has an active scale. In order to perform the export correctly, "
                    "it is necessary to save the project first.\n\n"
                    "Would you like to save the project now and proceed with the export?"),
                QMessageBox::Yes | QMessageBox::No).exec();
            if (i_ret == QMessageBox::Yes) {
                mModel->saveProject();
                mAppStatus->activeFlag(AppStatus::Flag::project_modified, false);
            } else if (i_ret == QMessageBox::No) {
                return;
            }
        }

        mView->setGraphosProjectsPath(QString::fromStdWString(mModel->graphosProjectsDirectory().toWString()));
        mView->exec();

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void ExportMeshPresenterImp::exportMesh(const QString &file)
{
    if (file.isEmpty() == false) {
        mModel->exportMesh(file.toStdWString());
    }
}

void ExportMeshPresenterImp::init()
{
}

void ExportMeshPresenterImp::initSignalAndSlots()
{
    connect(mView, &ExportMeshView::fileSelected, this, &ExportMeshPresenterImp::exportMesh);
}

} // namespace graphos