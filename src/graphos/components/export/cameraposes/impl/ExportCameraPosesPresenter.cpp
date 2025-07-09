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

#include "ExportCameraPosesPresenter.h"

#include "graphos/components/export/cameraposes/ExportCameraPosesModel.h"
#include "graphos/components/export/cameraposes/ExportCameraPosesView.h"
#include "graphos/components/export/cameraposes/impl/ExportCameraPosesTask.h"
#include "graphos/widgets/NvmFormatWidget.h"
#include "graphos/widgets/BundlerFormatWidget.h"
#include "graphos/widgets/MveFormatWidget.h"
#include "graphos/widgets/OriTxtFormatWidget.h"
#include "graphos/core/task/Progress.h"

#include <tidop/core/defs.h>
#include <tidop/geometry/entities/point.h>

#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>

namespace graphos
{

ExportCameraPosesPresenterImp::ExportCameraPosesPresenterImp(ExportCameraPosesView *view,
                                                             ExportCameraPosesModel *model)
  : ExportCameraPosesPresenter(),
    mView(view),
    mModel(model),
    //mNvmFormatWidget(new NvmFormatWidget),
    //mBundlerFormatWidget(new BundlerFormatWidgetImp),
    //mMveFormatWidget(new MveFormatWidget),
    mOriTxtFormatWidget(new OriTxtFormatWidget)
{
    ExportCameraPosesPresenterImp::init();
    ExportCameraPosesPresenterImp::initSignalAndSlots();
}

ExportCameraPosesPresenterImp::~ExportCameraPosesPresenterImp()
{
    //if (mNvmFormatWidget) {
    //    delete mNvmFormatWidget;
    //    mNvmFormatWidget = nullptr;
    //}

    //if (mBundlerFormatWidget) {
    //    delete mBundlerFormatWidget;
    //    mBundlerFormatWidget = nullptr;
    //}

    //if (mMveFormatWidget) {
    //    delete mMveFormatWidget;
    //    mMveFormatWidget = nullptr;
    //}

    if (mOriTxtFormatWidget) {
        delete mOriTxtFormatWidget;
        mOriTxtFormatWidget = nullptr;
    }
}

void ExportCameraPosesPresenterImp::open()
{
    mExportFormat.clear();

    QString filters("TXT (*.txt);;ODM (*.json)");

    QString selected_filter;
    mExportFile = QFileDialog::getSaveFileName(nullptr,
                                               QApplication::translate("ExportCameraPosesPresenter", "Camera Poses Export"),
                                               QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                               filters,
                                               &selected_filter);

    if (!mExportFile.isEmpty()) {

        if (selected_filter.compare("TXT (*.txt)") == 0) {
            mView->setCurrentFormat(mOriTxtFormatWidget->windowTitle());
        } else if (selected_filter.compare("ODM (*.json)") == 0) {
            mExportFormat = "ODM";
            run();
            return;
        } else {
            tl::Message::error("Unsupported format");
            return;
        }

        mView->exec();
    }
}

void ExportCameraPosesPresenterImp::init()
{
    //mView->addFormatWidget(mNvmFormatWidget);
    //mView->addFormatWidget(mBundlerFormatWidget);
    mView->addFormatWidget(mOriTxtFormatWidget);
    //mView->setCurrentFormat(mNvmFormatWidget->windowTitle());
}

void ExportCameraPosesPresenterImp::initSignalAndSlots()
{
    connect(mView, &ExportCameraPosesView::formatChange, this, &ExportCameraPosesPresenterImp::setCurrentFormat);
    connect(mView, &ExportCameraPosesView::run, this, &ExportCameraPosesPresenterImp::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("export_cameras.html");
    });
}

void ExportCameraPosesPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("ExportCameraPosesPresenter", "Task error"));
    }
}

void ExportCameraPosesPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("ExportCameraPosesPresenter", "Task finished"));
    }
}

auto ExportCameraPosesPresenterImp::createTask() -> std::unique_ptr<tl::Task>
{
    std::unique_ptr<tl::Task> export_task;

    if (progressHandler()) {
        progressHandler()->setRange(0, 1);
        progressHandler()->setCloseAuto(true);
        progressHandler()->setTitle(QApplication::translate("ExportCameraPosesPresenter", "Export Camera Poses"));
        progressHandler()->setDescription(QApplication::translate("ExportCameraPosesPresenter", "Exporting camera poses..."));
    }

    //tl::Path export_file_path(mOriTxtFormatWidget->file().toStdString());

    if (mExportFormat.isEmpty()) mExportFormat = mView->format();

    export_task = std::make_unique<ExportCameraPosesTask>(tl::Path(mExportFile.toStdWString()),
                                                          mModel->images(),
                                                          mModel->poses(),
                                                          mModel->cameras(),
                                                          mModel->enuCrs(),
                                                          mExportFormat);
    if (mExportFormat == "TXT") {
        dynamic_cast<ExportCameraPosesTask *>(export_task.get())->setQuaternionRotation(mOriTxtFormatWidget->rotation() == "Quaternions");
    }

    mView->hide();

    return export_task;
}

void ExportCameraPosesPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

void ExportCameraPosesPresenterImp::setCurrentFormat(const QString &format)
{
    mView->setCurrentFormat(format);
}

} // namespace graphos
