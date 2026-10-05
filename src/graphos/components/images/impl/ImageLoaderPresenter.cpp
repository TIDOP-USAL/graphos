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
#include "graphos/components/images/impl/LoadImagesTask.h"
#include "graphos/framework/task/ProgressHandler.h"
#include "graphos/core/image/Image.h"
#include "graphos/core/camera/Camera.h"

#include <tidop/core/app/Message.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/geotools/CRSsTools.h>

#include <QApplication>

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
    connect(mView, &ImageLoaderView::accepted, this, &ImageLoaderPresenterImp::run);
    connect(mView, &ImageLoaderView::rejected, this, &ImageLoaderPresenterImp::cancel);
    connect(mView, &ImageLoaderView::filesSelected, this, &ImageLoaderPresenterImp::setImages);
}

void ImageLoaderPresenterImp::open()
{
    mView->setImagesDirectory(QString::fromStdWString(mModel->imagesDirectory().toWString()));
    mView->exec();
}

void ImageLoaderPresenterImp::addImage(int imageIndex, int cameraId)
{
    Image image = mImages[imageIndex];
    image.setCameraId(cameraId);
    mModel->addImage(std::move(image));

    emit image_loaded(Image::id(image));
}

void ImageLoaderPresenterImp::onError(tl::TaskErrorEvent &event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("ImageLoaderComponent", "Task error"));
    }
}

void ImageLoaderPresenterImp::onFinished(tl::TaskFinalizedEvent &event)
{
    TaskPresenter::onFinished(event);

    if (auto *load_images_task = dynamic_cast<const LoadImagesTask *>(event.task())) {

        if (mModel->enuCrs().empty() && !load_images_task->enuCrs().empty()) {
            std::string enu_str = load_images_task->enuCrs();
            mModel->setEnuCrs(enu_str);
            tl::Message::info("Calculated Local ENU Reference System: {}", enu_str);
        }
    }

    if (progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("ImageLoaderComponent", "Task finished"));
    }
}

auto ImageLoaderPresenterImp::createTask() -> std::unique_ptr<tl::Task>
{
    std::unique_ptr<tl::Task> image_loader_task;

    if (mImageFiles.empty()) return image_loader_task;

    mImages.clear();
    for (auto &image_file : mImageFiles) {
        tl::Path image_path(image_file.toStdString());
        size_t hash_id = tl::Path::hash(image_path.toString());
        if (!mModel->existImage(hash_id)) {
            mImages.emplace_back(image_path);
        }
    }

    if (mImages.empty()) return image_loader_task;

    image_loader_task = std::make_unique<LoadImagesTask>(mImages, mModel->cameraRepository(), "OpenCV 1");
    //mTaskRef = load_task.get();

    connect(dynamic_cast<LoadImagesTask *>(image_loader_task.get()), &LoadImagesTask::imageAdded,
            this, &ImageLoaderPresenterImp::addImage);

    if (progressHandler()) {
        progressHandler()->setRange(0, mImages.size());
        progressHandler()->setTitle(QApplication::translate("ImageLoaderComponent", "Load images"));
        progressHandler()->setDescription(QApplication::translate("ImageLoaderComponent", "Loading images..."));
    }

    return image_loader_task;
}

void ImageLoaderPresenterImp::setImages(const QStringList &files)
{
    mImageFiles = files;
}

void ImageLoaderPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}

} // namespace graphos
