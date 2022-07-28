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

#include "graphos/core/process/Progress.h"
#include "graphos/components/loadfromvideo/impl/LoadFromVideoModel.h"
#include "graphos/components/loadfromvideo/impl/LoadFromVideoView.h"
//#include "graphos/components/loadfromvideo/impl/LoadFromVideoProcess.h"
#include "graphos/core/utils.h"

#include <tidop/core/defs.h>

namespace graphos
{

LoadFromVideoPresenterImp::LoadFromVideoPresenterImp(LoadFromVideoView *view,
                                                     LoadFromVideoModel *model)
  : LoadFromVideoPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

LoadFromVideoPresenterImp::~LoadFromVideoPresenterImp()
{

}

void LoadFromVideoPresenterImp::setImages(const QStringList &files)
{
  //mImageFiles = files;
}

void LoadFromVideoPresenterImp::addImage(int imageId, int cameraId)
{
  //Image image = mImages[imageId];
  //Camera camera = mCameras[cameraId];
  //int camera_id = mModel->cameraID(camera);
  //if(camera_id == 0)
  //  camera_id = mModel->addCamera(camera);

  //image.setCameraId(camera_id);
  //mModel->addImage(image);

  //QString crs_proj = mModel->projectCRS();
  //QString crs_image = image.cameraPose().crs();
  //if(crs_proj.isEmpty() && !crs_image.isEmpty()) {
  //  mModel->setProjectCRS(crs_image);
  //}

  //emit frame_loaded(image.id());
}

void LoadFromVideoPresenterImp::open()
{
  //mModel->loadSettings();
  //LoadFromVideoParameters *parameters = mModel->parameters();

  /* Configure View here */
  
  mView->exec();
}

void LoadFromVideoPresenterImp::init()
{

}

void LoadFromVideoPresenterImp::initSignalAndSlots()
{
  connect(mView, &ProcessView::run,     this,   &ProcessPresenter::run);
  connect(mView, &DialogView::help, [&]() {
    emit help("video.html");
  });
}

void LoadFromVideoPresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Process error"));
  }
}

void LoadFromVideoPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Process finished"));
  }
}

std::unique_ptr<tl::Task> LoadFromVideoPresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> process;
  
  //std::shared_ptr<LoadFromVideoAlgorithm> algorithm = std::make_shared<LoadFromVideoAlgorithm>();

  //process = std::make_unique<LoadFromVideoProcess>(algorithm);
  //
  //if (progressHandler()){
  //  progressHandler()->setRange(0, 0);
  //  progressHandler()->setTitle("Computing LoadFromVideo...");
  //  progressHandler()->setDescription("Computing LoadFromVideo...");
  //}
  //
  //mView->hide();
  //
  return process;
}

void LoadFromVideoPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

} // namespace graphos
