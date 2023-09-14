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

#include "ExportPointCloudPresenter.h"

#include "graphos/components/export/pointcloud/ExportPointCloudModel.h"
#include "graphos/components/export/pointcloud/ExportPointCloudView.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

ExportPointCloudPresenterImp::ExportPointCloudPresenterImp(ExportPointCloudView *view,
                                                           ExportPointCloudModel *model,
                                                           AppStatus *status)
  : ExportPointCloudPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
    this->init();
    this->initSignalAndSlots();
}

ExportPointCloudPresenterImp::~ExportPointCloudPresenterImp()
{
}

void ExportPointCloudPresenterImp::open()
{
    mView->setGraphosProjectsPath(QString::fromStdWString(mModel->graphosProjectsDirectory().toWString()));
    mView->exec();
}

void ExportPointCloudPresenterImp::exportPointCloud(const QString &file)
{
    if (file.isEmpty() == false) {
        mModel->exportPointCloud(file.toStdWString());
    }
}

void ExportPointCloudPresenterImp::init()
{
}

void ExportPointCloudPresenterImp::initSignalAndSlots()
{
    connect(mView, &ExportPointCloudView::fileSelected, this, &ExportPointCloudPresenterImp::exportPointCloud);
}

} // namespace graphos