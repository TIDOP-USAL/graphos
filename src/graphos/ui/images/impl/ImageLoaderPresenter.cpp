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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "ImageLoaderPresenter.h"

#include "graphos/ui/images/ImageLoaderModel.h"
#include "graphos/ui/images/ImageLoaderView.h"
#include "graphos/ui/HelpDialog.h"
#include "graphos/core/process/Progress.h"
#include "graphos/process/MultiProcess.h"
#include "graphos/process/images/LoadImagesProcess.h"
#include "graphos/core/image.h"

#include <tidop/core/messages.h>
#include <tidop/geospatial/camera.h>

namespace graphos
{

ImageLoaderPresenterImp::ImageLoaderPresenterImp(ImageLoaderView *view,
                                                 ImageLoaderModel *model)
  : ImageLoaderPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
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

void ImageLoaderPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("Images.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void ImageLoaderPresenterImp::open()
{
  mView->setImagesDirectory(mModel->imagesDirectory());
  mView->exec();
}

void ImageLoaderPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void ImageLoaderPresenterImp::addImage(int imageId, int cameraId)
{
  Image image = mImages[imageId];
  tl::Camera camera = mCameras[cameraId];
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

  emit imageLoaded(image.path());
}

void ImageLoaderPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Images loaded"));
  }

  //msgInfo("Images loaded");
}

bool ImageLoaderPresenterImp::createProcess()
{
  if (mImageFiles.empty()) return false;

  mImages.clear();
  for (auto &image : mImageFiles) {
    Image img(image);
    mImages.push_back(img);
  }

  mCameras.clear();
  for (auto &it = mModel->cameraBegin(); it != mModel->cameraEnd(); it++) {
    mCameras.push_back(it->second);
  }

  std::shared_ptr<LoadImagesProcess> load_images(new LoadImagesProcess(&mImages, &mCameras, mModel->projectCRS()));

  connect(load_images.get(), &LoadImagesProcess::imageAdded, this, &ImageLoaderPresenterImp::addImage);

  mMultiProcess->appendProcess(load_images);

  if (mProgressHandler) {
    mProgressHandler->setRange(0, mImages.size());
    mProgressHandler->setTitle("Load images...");
    mProgressHandler->setDescription("Load images...");
  }
  
  return true;
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

void ImageLoaderPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Load images error"));
  }
}

} // namespace graphos
