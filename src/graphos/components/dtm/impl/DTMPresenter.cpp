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

#include "DTMPresenter.h"

#include "graphos/components/dtm/DTMView.h"
#include "graphos/components/dtm/DTMModel.h"
#include "graphos/components/dtm/impl/DTMTask.h"
#include "graphos/core/task/Progress.h"

#include <tidop/core/msg/message.h>

#include <QDir>
#include <QImageReader>
#include <QMessageBox>

namespace graphos
{

DtmPresenterImp::DtmPresenterImp(DtmView *view,
                                 DtmModel *model)
  : DtmPresenter(),
    mView(view),
    mModel(model)
{
    this->init();
    this->initSignalAndSlots();
}

DtmPresenterImp::~DtmPresenterImp()
{
}

void DtmPresenterImp::open()
{
    mView->setGSD(mModel->gsd());
    mView->enableMDS();

    mView->exec();
}

void DtmPresenterImp::init()
{
}

void DtmPresenterImp::initSignalAndSlots()
{
    connect(mView, &DtmView::run, this, &DtmPresenterImp::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("dtm.html");
    });
}

void DtmPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("DEM error"));
    }
}

void DtmPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("DEM finished"));
    }

    tl::Path dsm_file = mModel->projectPath();
    dsm_file.append("dtm").append("dsm.tif");
    if (mView->isMdsEnable() && dsm_file.exists()) {
        mModel->setDsmPath(dsm_file);
    }

    tl::Path dtm_file = dsm_file;
    dtm_file.replaceBaseName("dtm");
    if (mView->isMdtEnable() && dtm_file.exists()) {
        mModel->setDtmPath(dtm_file);
    }

    mModel->setGSD(mView->gsd());
}

std::unique_ptr<tl::Task> DtmPresenterImp::createTask()
{
    std::unique_ptr<tl::Task> dtm_task;

    tl::Path dtm_path = mModel->projectPath();
    dtm_path.append("dtm");

    dtm_task = std::make_unique<DtmTask>(mModel->denseModel(), 
                                         mModel->offset(),
                                         dtm_path,
                                         mView->gsd(),
                                         mModel->crs(),
                                         mView->isMdsEnable(),
                                         mView->isMdtEnable());

    if (progressHandler()) {
        progressHandler()->setRange(0, 100);
        progressHandler()->setTitle("DEM");
        progressHandler()->setDescription("DEM processing...");
    }

    mView->hide();

    return dtm_task;
}

void DtmPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

} // namespace graphos



