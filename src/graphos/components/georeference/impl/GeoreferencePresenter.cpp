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

#include "GeoreferencePresenter.h"

#include "graphos/components/georeference/impl/GeoreferenceModel.h"
#include "graphos/components/georeference/impl/GeoreferenceView.h"
#include "graphos/components/georeference/impl/GeoreferenceTask.h"
#include "graphos/core/task/Progress.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

#include <tidop/core/defs.h>
#include <tidop/core/msg/message.h>

#include <QFileInfo>
#include <QAbstractItemModel>
#include <QStandardItemModel>

namespace graphos
{

GeoreferencePresenterImp::GeoreferencePresenterImp(GeoreferenceView *view,
                                                   GeoreferenceModel *model)
  : mView(view),
    mModel(model)
{
    GeoreferencePresenterImp::init();
    GeoreferencePresenterImp::initSignalAndSlots();
}

GeoreferencePresenterImp::~GeoreferencePresenterImp()
{
}

void GeoreferencePresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Georeference process error"));
    }
}

void GeoreferencePresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if (progressHandler()) {
        progressHandler()->setDescription(tr("Georeference finished"));
    }

    try {

        auto transform = dynamic_cast<GeoreferenceTask const *>(event->task())->transform();

        tl::Path path = mModel->reconstructionPath();

        if (transform != tl::Matrix<double, 4, 4>::identity()) {
            mModel->setTransform(transform);
        } 

        path.append("offset.txt");
        mModel->setOffset(path);

        path.replaceFileName("poses.bin");

        auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
        poses_reader->read(path);
        auto poses = poses_reader->cameraPoses();

        for (const auto &camera_pose : poses) {
            mModel->addPhotoOrientation(camera_pose.first, camera_pose.second);
        }

        path.replaceFileName("sparse.ply");
        mModel->setSparseModel(path);

        path.replaceFileName("ground_points.bin");
        mModel->setGroundPoints(path);

        mModel->setCrs(mView->crs());

        auto cameras = dynamic_cast<GeoreferenceTask const *>(event->task())->cameras();

        for (const auto &camera : cameras) {
            mModel->updateCamera(camera.first, camera.second);
        }

    } catch (const std::exception &e) {
        tl::printException(e);
    }
}

std::unique_ptr<tl::Task> GeoreferencePresenterImp::createTask()
{
    std::unique_ptr<tl::Task> georeference_process;

    georeference_process = std::make_unique<GeoreferenceTask>(mModel->images(),
                                                              mModel->cameras(),
                                                              mModel->poses(),
                                                              mModel->groundPoints(),
                                                              mModel->groundControlPoints(),
                                                              mModel->reconstructionPath(),
                                                              mModel->database());

    if (progressHandler()) {
        progressHandler()->setRange(0, 0);
        progressHandler()->setTitle("Computing Georeference...");
        progressHandler()->setDescription("Computing Georeference...");
    }

    return georeference_process;
}

void GeoreferencePresenterImp::cancel()
{
    TaskPresenter::cancel();

   tl::Message::warning("Processing has been canceled by the user");
}

void GeoreferencePresenterImp::open()
{
    mView->clear();

    mModel->loadGroundControlPoints();
    mView->setCrs(mModel->crs());
    mView->showMaximized();

    bool active_image = false;
    std::vector<std::pair<size_t, QString>> images;
    for (const auto &image : mModel->images()) {
        images.emplace_back(image.first, image.second.path());
    }

    if (!images.empty()) {
        mView->setImageList(images);
        setImageActive(images[0].first);
    }

}

void GeoreferencePresenterImp::init()
{
    mView->setItemModelGroundControlPoints(mModel->itemModelGroundControlPoints());
    mView->setItemModelImagePoints(mModel->itemModelImagePoints());
}

void GeoreferencePresenterImp::initSignalAndSlots()
{
    connect(mView, &GeoreferenceView::image_changed, this, &GeoreferencePresenterImp::setImageActive);
    connect(mView, &GeoreferenceView::addGroundControlPoint, mModel, &GeoreferenceModel::addGroundControlPoint);
    connect(mView, &GeoreferenceView::removeGroundControlPoint, mModel, &GeoreferenceModel::removeGroundControlPoint);
    connect(mView, &GeoreferenceView::add_image_point, mModel, &GeoreferenceModel::addImagePoint);
    connect(mView, &GeoreferenceView::remove_image_point, mModel, &GeoreferenceModel::removeImagePoint);
    connect(mView, &GeoreferenceView::crsChange, mModel, &GeoreferenceModel::setCrs);
    connect(mView, &GeoreferenceView::georeference, this, &TaskPresenter::run);
    connect(mView, &GeoreferenceView::accepted, mModel, &GeoreferenceModel::save);
    connect(mView, &GeoreferenceView::import_gcp, 
        [&](const QString &file, const QString &format) {
            mModel->importGroundControlPoints(file, format);
            mView->setCrs(mModel->crs());
        });
    connect(mView, &GeoreferenceView::export_gcp, mModel, &GeoreferenceModel::exportGroundControlPoints);

    connect(mView, &DialogView::help, [&]() {
        emit help("cameras.html");
    });

}

void GeoreferencePresenterImp::setImageActive(size_t imageId)
{
    auto image = mModel->image(imageId);
    mView->setCurrentImage(image.path());
    std::list<std::pair<QString, QPointF>> points = mModel->points(imageId);
    mView->setPoints(points);
}

} // namespace graphos
