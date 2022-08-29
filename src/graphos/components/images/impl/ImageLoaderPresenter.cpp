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

#include "ImageLoaderPresenter.h"

#include "graphos/components/images/ImageLoaderModel.h"
#include "graphos/components/images/ImageLoaderView.h"
#include "graphos/components/images/impl/ImageLoaderTask.h"
#include "graphos/core/process/Progress.h"
#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

#include <tidop/core/messages.h>


namespace graphos
{

ImageLoaderPresenterImp::ImageLoaderPresenterImp(ImageLoaderView *view,
                                                 ImageLoaderModel *model)
  : ImageLoaderPresenter(),
    mView(view),
    mModel(model)
{
  ImageLoaderPresenterImp::init();
  ImageLoaderPresenterImp::initSignalAndSlots();
}

ImageLoaderPresenterImp::~ImageLoaderPresenterImp()
{
}

void ImageLoaderPresenterImp::init()
{
}

void ImageLoaderPresenterImp::initSignalAndSlots()
{
  connect(mView, &ImageLoaderView::accepted,      this, &ImageLoaderPresenterImp::run);
  connect(mView, &ImageLoaderView::rejected,      this, &ImageLoaderPresenterImp::cancel);
  connect(mView, &ImageLoaderView::filesSelected, this, &ImageLoaderPresenterImp::setImages);
}

void ImageLoaderPresenterImp::open()
{
  mView->setImagesDirectory(QString::fromStdWString(mModel->imagesDirectory().toWString()));
  mView->exec();
}

void ImageLoaderPresenterImp::addImage(int imageId, int cameraId)
{
  Image image = mImages[imageId];
  Camera camera = mCameras[cameraId];
  int camera_id = mModel->cameraID(camera);
  if (camera_id == 0)
    camera_id = mModel->addCamera(camera);

  image.setCameraId(camera_id);
  mModel->addImage(image);

  QString crs_proj = mModel->projectCRS();
  QString crs_image = image.cameraPose().crs();
  if (crs_proj.isEmpty() && !crs_image.isEmpty()) {
    mModel->setProjectCRS(crs_image);
  }

  emit image_loaded(image.id());
}

void ImageLoaderPresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if(progressHandler()) {
    progressHandler()->setDescription(tr("Load images error"));
  }
}

void ImageLoaderPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Images loaded"));
  }
}

std::unique_ptr<tl::Task> ImageLoaderPresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> image_loader_process;

  if (mImageFiles.empty()) return false;

  mImages.clear();
  for (auto &image : mImageFiles) {
    Image img(image);
    if (!mModel->existImage(img.id()))
      mImages.push_back(img);
  }

  mCameras.clear();
  for (const auto &camera : mModel->cameras()) {
    mCameras.push_back(camera.second);
  }

  image_loader_process = std::make_unique<LoadImagesTask>(&mImages, &mCameras, "OpenCV 1", mModel->projectCRS());

  connect(dynamic_cast<LoadImagesTask *>(image_loader_process.get()),
          &LoadImagesTask::imageAdded, 
          this, &ImageLoaderPresenterImp::addImage);

  if (progressHandler()) {
    progressHandler()->setRange(0, mImages.size());
    progressHandler()->setTitle("Load images...");
    progressHandler()->setDescription("Load images...");
  }
  
  return image_loader_process;
}

void ImageLoaderPresenterImp::setImages(const QStringList &files)
{
  mImageFiles = files;
}

void ImageLoaderPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

} // namespace graphos
