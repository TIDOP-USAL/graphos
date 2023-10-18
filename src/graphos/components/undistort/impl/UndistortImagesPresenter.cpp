/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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


#include "UndistortImagesPresenter.h"

#include "graphos/core/utils.h"
#include "graphos/core/task/Progress.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/components/undistort/impl/UndistortImagesModel.h"
#include "graphos/components/undistort/impl/UndistortImagesView.h"

#include <tidop/core/defs.h>

namespace graphos
{

UndistortImagesPresenterImp::UndistortImagesPresenterImp(UndistortImagesView *view,
                                                         UndistortImagesModel *model)
  : UndistortImagesPresenter(),
    mView(view),
    mModel(model)
{
    this->init();
    this->initSignalAndSlots();
}

UndistortImagesPresenterImp::~UndistortImagesPresenterImp()
{

}

void UndistortImagesPresenterImp::open()
{
    mModel->loadSettings();

    mView->setDirectory(QString::fromStdString(mModel->projectFolder().toString()));

    mView->open();
}

void UndistortImagesPresenterImp::init()
{

}

void UndistortImagesPresenterImp::initSignalAndSlots()
{
    connect(mView, &UndistortImagesView::accepted, this, &UndistortImagesPresenterImp::run);
    connect(mView, &UndistortImagesView::rejected, this, &UndistortImagesPresenterImp::cancel);
}

void UndistortImagesPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Process error"));
    }
}

void UndistortImagesPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Process finished"));
    }
}

std::unique_ptr<tl::Task> UndistortImagesPresenterImp::createProcess()
{
    std::unique_ptr<tl::Task> process = std::make_unique<UndistortImages>(mModel->images(),
                                                                          mModel->cameras(),
                                                                          mView->directory().absolutePath(),
                                                                          UndistortImages::Format::tiff,
                                                                          mModel->useCuda());

    if (progressHandler()) {
        progressHandler()->setRange(0, mModel->images().size());
        progressHandler()->setTitle("Computing UndistortImages...");
        progressHandler()->setDescription("Computing UndistortImages...");
    }

    return process;
}

void UndistortImagesPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

} // namespace graphos
