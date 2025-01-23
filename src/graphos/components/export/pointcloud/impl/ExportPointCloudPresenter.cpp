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
#include "graphos/widgets/PlyFormatWidget.h"
#include "graphos/widgets/LasFormatWidget.h"
#include "graphos/core/task/Progress.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

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
        progressHandler()->setDescription(QApplication::translate("ExportPointCloudComponent", "Task error"));
    }
}

void ExportPointCloudPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("ExportPointCloudComponent", "Task finished"));
    }
}

auto ExportPointCloudPresenterImp::createTask() -> std::unique_ptr<tl::Task>
{
    std::unique_ptr<tl::Task> export_point_cloud_task;

    /// Provisional
    if (mPlyFormatWidget && mPlyFormatWidget->windowTitle() == mView->format()) {
        export_point_cloud_task = std::make_unique<ExportPointCloudTask>(mModel->pointCloud(),
                                                                         mExportFile.toStdString(),
                                                                         mModel->enuCrs().toStdString(),
                                                                         mModel->crs().toStdString(),
                                                                         mPlyFormatWidget->format() == PlyFormatWidget::Format::binary,
                                                                         mPlyFormatWidget->isExportColorsEnabled(),
                                                                         mPlyFormatWidget->isExportNormalsEnabled());
    } else if (mLasFormatWidget && mLasFormatWidget->windowTitle() == mView->format()) {
        export_point_cloud_task = std::make_unique<ExportPointCloudTask>(mModel->pointCloud(),
                                                                         mExportFile.toStdString(),
                                                                         mModel->enuCrs().toStdString(),
                                                                         mModel->crs().toStdString(),
                                                                         false,
                                                                         mLasFormatWidget->isExportColorsEnabled(),
                                                                         mLasFormatWidget->isExportNormalsEnabled());
    }


    if (progressHandler()) {
        double scale = mModel->pointCloudSize() / 90.;
        progressHandler()->setRange(0, 10. * scale + mModel->pointCloudSize());
        progressHandler()->setTitle(QApplication::translate("ExportPointCloudComponent", "Point Cloud Export"));
        progressHandler()->setDescription(QApplication::translate("ExportPointCloudComponent", "Point cloud exporting..."));
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
    QString filters;
    if (mPlyFormatWidget) filters.append("PLY (*.ply)");
    if (mLasFormatWidget) {
        if (!filters.isEmpty()) filters.append(";;");
        filters.append("LAS (*.las *.laz)");
    }

    QString selected_filter;
    mExportFile = QFileDialog::getSaveFileName(nullptr,
                                               QApplication::translate("ExportPointCloudComponent", "Point Cloud Export"),
                                               QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                               filters,
                                               &selected_filter);

    if (!mExportFile.isEmpty()) {

        if (selected_filter.compare("PLY (*.ply)") == 0) {
            mView->setFormat(mPlyFormatWidget->windowTitle());
        } else if (selected_filter.compare("LAS (*.las *.laz)") == 0) {
            mView->setFormat(mLasFormatWidget->windowTitle());
        } else {
            tl::Message::error("Unsupported format");
            return;
        }

        mView->setCrs(mModel->crs());

        mView->exec();
    }

}

void ExportPointCloudPresenterImp::setPlyFormatWidget(const std::shared_ptr<PlyFormatWidget> &plyFormatWidget)
{
    mPlyFormatWidget = plyFormatWidget;

    mView->addFormat(mPlyFormatWidget.get());
    mView->setFormat(mPlyFormatWidget->windowTitle());
}

void ExportPointCloudPresenterImp::setLasFormatWidget(const std::shared_ptr<LasFormatWidget> &lasFormatWidget)
{
    mLasFormatWidget = lasFormatWidget;

    mView->addFormat(mLasFormatWidget.get());
    mView->setFormat(mLasFormatWidget->windowTitle());
}

void ExportPointCloudPresenterImp::setFormat(const QString &format)
{
    mView->setFormat(format);
}

void ExportPointCloudPresenterImp::init()
{
}

void ExportPointCloudPresenterImp::initSignalAndSlots()
{
    connect(mView, &ExportPointCloudView::run, this, &ExportPointCloudPresenterImp::run);

    connect(mView, &DialogView::help, [&]() {
        emit help("export_point_cloud.html");
        });
}

} // namespace graphos