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
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

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
    mView->setGraphosProjectsPath(QString::fromStdWString(mModel->graphosProjectsDirectory().toWString()));
    mView->exec();
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