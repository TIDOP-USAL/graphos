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
#include "graphos/components/export/pointcloud/impl/ExportPointCloudTask.h"
#include "graphos/core/task/Progress.h"
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
    ExportPointCloudPresenterImp::init();
    ExportPointCloudPresenterImp::initSignalAndSlots();
}

void ExportPointCloudPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Export Point Cloud error"));
    }
}

void ExportPointCloudPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Export Point Cloud finished"));
    }
}

auto ExportPointCloudPresenterImp::createTask() -> std::unique_ptr<tl::Task>
{
    std::unique_ptr<tl::Task> export_point_cloud_task = std::make_unique<ExportPointCloudTask>(mModel->pointCloud(),
                                                                                               mModel->offset(),
                                                                                               mExportFile.toStdString(),
                                                                                               mModel->crs().toStdString());

    if (progressHandler()) {
        progressHandler()->setRange(0, mModel->pointCloudSize()+11);
        progressHandler()->setTitle("Export Point Cloud");
        progressHandler()->setDescription("Export Point Cloud processing...");
        progressHandler()->setCloseAuto(true);
    }

    mView->hide();

    return export_point_cloud_task;
}

void ExportPointCloudPresenterImp::cancel()
{
    TaskPresenter::cancel();
}

void ExportPointCloudPresenterImp::open()
{
    mView->setGraphosProjectsPath(QString::fromStdWString(mModel->graphosProjectsDirectory().toWString()));
    mView->exec();
}

void ExportPointCloudPresenterImp::exportPointCloud(const QString &file)
{
    if (file.isEmpty() == false) {
        mExportFile = file;
        emit run();
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