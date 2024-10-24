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


#include "LoadFromVideoPresenter.h"

#include "graphos/core/task/Progress.h"
#include "graphos/components/loadfromvideo/impl/LoadFromVideoModel.h"
#include "graphos/components/loadfromvideo/impl/LoadFromVideoView.h"
#include "graphos/components/loadfromvideo/impl/LoadFromVideoTask.h"
#include "graphos/core/utils.h"

#include <tidop/core/defs.h>

namespace graphos
{

LoadFromVideoPresenterImp::LoadFromVideoPresenterImp(LoadFromVideoView *view,
                                                     LoadFromVideoModel *model)
  : LoadFromVideoPresenter(),
    mView(view),
    mModel(model)
{
    LoadFromVideoPresenterImp::init();
    LoadFromVideoPresenterImp::initSignalAndSlots();
}

LoadFromVideoPresenterImp::~LoadFromVideoPresenterImp() = default;

void LoadFromVideoPresenterImp::addImage(QString imagePath, int cameraId)
{
    Image image(imagePath);
    Camera camera = mCameras[cameraId];
    int camera_id = mModel->cameraID(camera);
    if (camera_id == 0)
        camera_id = mModel->addCamera(camera);

    image.setCameraId(camera_id);
    mModel->addImage(image);

    emit frame_loaded(image.id());
}

void LoadFromVideoPresenterImp::open()
{
    mView->exec();
}

void LoadFromVideoPresenterImp::init()
{

}

void LoadFromVideoPresenterImp::initSignalAndSlots()
{
    connect(mView, &TaskView::run, this, &TaskPresenter::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("load_from_video.html");
            });
}

void LoadFromVideoPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Process error"));
    }
}

void LoadFromVideoPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Process finished"));
    }
}

auto LoadFromVideoPresenterImp::createTask() -> std::unique_ptr<tl::Task>
{
    std::unique_ptr<tl::Task> task;

    tl::Path images_path = mModel->imagesPath();

    mCameras.clear();
    for (const auto &camera : mModel->cameras()) {
        mCameras.push_back(camera.second);
    }

    int skip_frames = mView->skipFrames();
    int begin = mView->videoIni();
    int end = mView->videoEnd();


    task = std::make_unique<LoadFromVideoTask>(mView->video().toStdWString(),
                                                  skip_frames,
                                                  begin,
                                                  end,
                                                  images_path,
                                                  &mCameras,
                                                  "OpenCV 1");

    connect(dynamic_cast<LoadFromVideoTask *>(task.get()),
            &LoadFromVideoTask::image_added,
            this, &LoadFromVideoPresenterImp::addImage);

    if (progressHandler()) {
        progressHandler()->setRange(0, (static_cast<size_t>(end) - begin) / skip_frames);
        progressHandler()->setTitle("Frame extraction...");
        progressHandler()->setDescription("Frame extraction...");
    }

    mView->hide();

    return task;
}

void LoadFromVideoPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

} // namespace graphos
