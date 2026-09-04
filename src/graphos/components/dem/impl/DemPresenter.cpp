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

#include "DemPresenter.h"

#include "graphos/components/dem/DemView.h"
#include "graphos/components/dem/DemModel.h"
#include "graphos/components/dem/impl/DemTask.h"
#include "graphos/framework/task/ProgressHandler.h"

#include <tidop/core/app/Message.h>

#include <QDir>
#include <QApplication>
#include <QMessageBox>

namespace graphos
{

DemPresenterImp::DemPresenterImp(DemView *view,
                                 DemModel *model)
  : DemPresenter(),
    mView(view),
    mModel(model)
{
    DemPresenterImp::init();
    DemPresenterImp::initSignalAndSlots();
}

void DemPresenterImp::open()
{
    if (auto config = mModel->config()) {
        mView->setGsd(config->gsd());
    }
    mView->setCrs(QString::fromStdString(mModel->crs()));
    mView->enableDsm();

    mView->exec();
}

void DemPresenterImp::init()
{
}

void DemPresenterImp::initSignalAndSlots()
{
    connect(mView, &DemView::run, this, &DemPresenterImp::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("dtm.html");
    });
}

void DemPresenterImp::onError(tl::TaskErrorEvent &event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("DemComponent", "Task error"));
    }
}

void DemPresenterImp::onFinished(tl::TaskFinalizedEvent &event)
{
    //dtm_task->subscribe([&](const tl::TaskFinalizedEvent *event) {

    //    auto task = dynamic_cast<DemTask const *>(event->task());

    //    auto report = task->report();
    //    mModel->setReport(report);

    //    });
    if (auto *task = dynamic_cast<const DemTask *>(event.task())) {
        try {

            mModel->setReport(task->report());
            mModel->setDsmPath(task->dsmPath());
            mModel->setDtmPath(task->dtmPath());
            mModel->setDsmEnuPath(task->dsmEnuPath());

        } catch (const std::exception &e) {
            tl::printException(e);
        }
    }

    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("DemComponent", "Task finished"));
    }

    //tl::Path dsm_file = mModel->projectPath();
    //dsm_file.append("dem").append("dsm.tif");
    //if (mView->isDsmEnable() && dsm_file.exists()) {
    //    mModel->setDsmPath(dsm_file);
    //}

    //tl::Path dtm_file = dsm_file;
    //dtm_file.replaceBaseName("dtm");
    //if (mView->isDsmEnable() && dtm_file.exists()) {
    //    mModel->setDtmPath(dtm_file);
    //}

    //mModel->setGsd(mView->gsd());
    //mModel->setCrs(mView->crs());
}

auto DemPresenterImp::createTask() -> std::unique_ptr<tl::Task>
{
    //TODO: Limpiar resultados previos si existen y preguntar al usuario si desea continuar
    //mModel->cleanProject();

    tl::Path dem_path = mModel->projectPath();
    dem_path.append("dem");

    auto config = std::make_shared<DemProperties>();
    config->setCrs(mView->crs().toStdString());
    config->setGsd(mView->gsd());
    config->setDsm(mView->isDsmEnable());
    config->setDtm(mView->isDtmEnable());
    mModel->setConfig(config);

    auto dtm_task = std::make_unique<DemTask>(mModel->denseModel(),
                                              dem_path,
                                              mModel->enuCrs(),
                                              config
                                              /*mView->crs().toStdString(),
                                              mView->gsd(),
                                              mView->isDsmEnable(),
                                              mView->isDtmEnable()*/);


    //dtm_task->subscribe([&](const tl::TaskFinalizedEvent *event) {

    //    auto task = dynamic_cast<DemTask const *>(event->task());

    //    auto report = task->report();
    //    mModel->setReport(report);

    //});

    if (progressHandler()) {
        progressHandler()->setRange(0, 100);
        progressHandler()->setTitle(QApplication::translate("DemComponent", "DEM"));
        progressHandler()->setDescription(QApplication::translate("DemComponent", "DEM processing..."));
    }

    mView->hide();

    return dtm_task;
}

void DemPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Task canceled by the user");
}

} // namespace graphos



