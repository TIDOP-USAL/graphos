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

#include "ExportCamerasPresenter.h"

#include "graphos/components/export/cameras/ExportCamerasModel.h"
#include "graphos/components/export/cameras/ExportCamerasView.h"
#include "graphos/components/export/cameras/impl/ExportCamerasTask.h"
#include "graphos/widgets/NvmFormatWidget.h"
#include "graphos/widgets/BundlerFormatWidget.h"
#include "graphos/widgets/MveFormatWidget.h"
#include "graphos/widgets/OriTxtFormatWidget.h"
#include "graphos/core/task/Progress.h"

#include <tidop/core/defs.h>
#include <tidop/geometry/entities/point.h>



namespace graphos
{

ExportCamerasPresenterImp::ExportCamerasPresenterImp(ExportCamerasView *view,
                                                     ExportCamerasModel *model)
  : ExportCamerasPresenter(),
    mView(view),
    mModel(model),
    //mNvmFormatWidget(new NvmFormatWidgetImp),
    //mBundlerFormatWidget(new BundlerFormatWidgetImp),
    //mMveFormatWidget(new MveFormatWidgetImp),
    mOriTxtFormatWidget(new OriTxtFormatWidgetImp)
{
    this->init();
    this->initSignalAndSlots();
}

ExportCamerasPresenterImp::~ExportCamerasPresenterImp()
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

void ExportCamerasPresenterImp::open()
{
    mView->setCurrentFormat(mOriTxtFormatWidget->windowTitle());
    mView->exec();
}

void ExportCamerasPresenterImp::init()
{
    //mView->addFormatWidget(mNvmFormatWidget);
    //mView->addFormatWidget(mBundlerFormatWidget);
    mView->addFormatWidget(mOriTxtFormatWidget);
    //mView->setCurrentFormat(mNvmFormatWidget->windowTitle());
}

void ExportCamerasPresenterImp::initSignalAndSlots()
{
    connect(mView, &ExportCamerasView::formatChange, this, &ExportCamerasPresenterImp::setCurrentFormat);
    connect(mView, &ExportCamerasView::run,
            this, &ExportCamerasPresenterImp::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("export_cameras.html");
    });
}

//void ExportCamerasPresenterImp::save()
//{
//    QString reconstructionPath = mModel->reconstruction();
//
//    colmap::Reconstruction reconstruction;
//    //#ifdef _DEBUG
//    //  //Lectura como texto
//    //  reconstruction.ReadText(reconstructionPath.toStdString());
//    //#else
//      //Lectura como binario
//    reconstruction.ReadBinary(reconstructionPath.toStdString());
//    //#endif
//
//    tl::Point3D offset = mModel->offset();
//    OrientationExport orientationExport(&reconstruction, offset);
//
//    QString oriFormat = mView->format();
//    if (oriFormat.compare("NVM") == 0) {
//        orientationExport.exportMVE(mNvmFormatWidget->file());
//    } else if (oriFormat.compare("Bundler") == 0) {
//        orientationExport.exportBundler(mBundlerFormatWidget->file());
//    } else if (oriFormat.compare("MVE") == 0) {
//        orientationExport.exportMVE(mMveFormatWidget->file());
//    } else if (oriFormat.compare("TXT") == 0) {
//        orientationExport.exportOrientation(mOriTxtFormatWidget->file(), mOriTxtFormatWidget->rotation().compare("Quaternions") == 0);
//    }
//
//}

void ExportCamerasPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Export cameras error"));
    }
}

void ExportCamerasPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Export cameras finished"));
    }
}

std::unique_ptr<tl::Task> ExportCamerasPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> export_task;

    if (progressHandler()) {
        progressHandler()->setRange(0, 1);
        progressHandler()->setCloseAuto(true);
        progressHandler()->setTitle("Export Cameras...");
        progressHandler()->setDescription("Export Cameras poses...");
    }

    QString format = mView->format();
    export_task = std::make_unique<ExportCamerasTask>(mOriTxtFormatWidget->file(),
                                                      mModel->images(),
                                                      mModel->poses(),
                                                      format);
    if (format.compare("TXT") == 0) {
        dynamic_cast<ExportCamerasTask *>(export_task.get())->setQuaternionRotation(mOriTxtFormatWidget->rotation() == "Quaternions");
    }

    mView->hide();

    return export_task;
}

void ExportCamerasPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

void ExportCamerasPresenterImp::setCurrentFormat(const QString &format)
{
    mView->setCurrentFormat(format);
}

} // namespace graphos
