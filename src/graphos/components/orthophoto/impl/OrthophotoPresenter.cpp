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

#include "OrthophotoPresenter.h"

#include "graphos/framework/task/ProgressHandler.h"
#include "graphos/components/orthophoto/impl/OrthophotoModel.h"
#include "graphos/components/orthophoto/impl/OrthophotoView.h"
#include "graphos/components/orthophoto/impl/OrthophotoTask.h"
#include "graphos/widgets/SeamBasedBlendingMosaicWidget.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/camera/CameraRepository.h"
#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/ortho/OrthophotoProperties.h"
#include "graphos/core/ortho/OrthomosaicProperties.h"
#include "graphos/core/ortho/SbbProperties.h"

#include <tidop/core/task/TaskList.h>

#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>

namespace graphos
{

OrthophotoPresenterImp::OrthophotoPresenterImp(OrthophotoView *view,
                                               OrthophotoModel *model)
  : OrthophotoPresenter(),
    mView(view),
    mModel(model)
{
    OrthophotoPresenterImp::init();
    OrthophotoPresenterImp::initSignalAndSlots();
}

OrthophotoPresenterImp::~OrthophotoPresenterImp()
{

}

auto OrthophotoPresenterImp::undistortedCameras(const CameraRepository &cameraRepo) const -> std::map<int, Camera>
{
    std::map<int, Camera> undistorted_cameras;

    for (const auto &[camera_id, camera] : cameraRepo) {

        Undistort undistort(camera);
        Camera undistort_camera = undistort.undistortCamera();
        undistorted_cameras[camera_id] = undistort_camera;

    }

    return undistorted_cameras;
}

void OrthophotoPresenterImp::setSeamBasedBlendingMosaicWidget(std::shared_ptr<SeamBasedBlendingMosaicWidget> sbb)
{
    mSbb = sbb;

    mView->addOrthomosaicMethodWidget(mSbb.get());
    mView->setOrthomosaicMethod(mSbb->objectName());
}

void OrthophotoPresenterImp::open()
{
    mModel->loadSettings();

    mView->setCrs(QString::fromStdString(mModel->crs()));

    QString orthomosaic_method = "RLB";
    if (auto config = mModel->properties()) {

        mView->setGsd(config->gsd());
        mView->setMaxTiltAngle(config->maxTiltAngle());
        mView->setInterpolation(QString::fromStdString(config->interpolation()));
        if (auto orthomosaic_properties = config->orthomosaicProperties()) {

            orthomosaic_method = QString::fromStdString(orthomosaic_properties->name());

            if (orthomosaic_method == "SBB") {
                auto sbb_properties = dynamic_cast<SbbProperties *>(orthomosaic_properties.get());
                mSbb->setExposureCompensator(QString::fromStdString(sbb_properties->exposureCompensator()));
                mSbb->setSeamFinder(QString::fromStdString(sbb_properties->seamFinder()));
                mSbb->setBlenderType(QString::fromStdString(sbb_properties->blenderType()));
            } else if (orthomosaic_method == "RLB") {

            }

        }
    } 

    mView->setOrthomosaicMethod(orthomosaic_method);

    mView->exec();
}

void OrthophotoPresenterImp::init()
{

}

void OrthophotoPresenterImp::initSignalAndSlots()
{
    connect(mView, &OrthophotoView::orthomosaicMethodChanged, [&](QString method) {
        mView->setOrthomosaicMethod(method);
        });

    connect(mView, &TaskView::run, this, &TaskPresenter::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("orthophoto.html");
    });
}

void OrthophotoPresenterImp::onError(tl::TaskErrorEvent &event)
{
    TaskPresenter::onError(event);

    if(progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("OrthophotoComponent", "Orthophoto task error"));
    }
}

void OrthophotoPresenterImp::onFinished(tl::TaskFinalizedEvent &event)
{
    if (auto *task = dynamic_cast<const OrthophotoTask *>(event.task())) {
        try {

            mModel->setOrthoReport(task->report());
            mModel->setOrthophoto(task->orthoPath());

        } catch (const std::exception &e) {
            tl::printException(e);
        }
    }

    TaskPresenter::onFinished(event);

    if(progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("OrthophotoComponent", "Orthophoto finished"));
    }
}

std::unique_ptr<tl::Task> OrthophotoPresenterImp::createTask()
{
    // Que sea una lista de tareas complica la gestión de los eventos de finalización y error, ya que no se puede saber a qué tarea pertenece el evento.
    // Como las imagenes tienen que venir corregidas de distorsión desde la densificación se omite esa tarea.
    //auto task_list = std::make_unique<tl::TaskList>();

    //tl::Path ortho_path = mModel->orthoPath();
    //if(!ortho_path.exists()) {
    //    int i_ret = QMessageBox(QMessageBox::Warning,
    //                            tr("Previous results"),
    //                            tr("The previous results will be overwritten. Do you wish to continue?"),
    //                            QMessageBox::Yes | QMessageBox::No).exec();
    //    if(i_ret == QMessageBox::No) {
    //        return ortho_process;
    //    }
    //}

    mModel->clearProject();
    emit ortho_deleted();

    const auto &images = mModel->images();
    const auto &cameras = mModel->cameras();
    bool use_cuda = mModel->useCuda();

    tl::Path undistort_path(mModel->projectFolder());
    undistort_path.append("undistorted");

    std::list<tl::Path> undistorted_image_paths;
    if (undistort_path.exists()) {
        undistorted_image_paths = undistort_path.list(".tif");
    }

    TL_TODO("Mover a la tarea. Con muchas imágenes tarda mucho y parece que se ha colgado el programa")
    auto undistorted_cameras = undistortedCameras(cameras);
    auto undistorted_images = undistortedImages(images, undistort_path);


    // TODO: Tiene que incluir el tipo de producto (RGB, Thermal, Red, NIR)
    tl::Path ortho_path = mModel->projectFolder();
    ortho_path.append("ortho");
    ortho_path.append("orthomosaic.tif");

    auto config = std::make_shared<OrthophotoProperties>();
    config->setCrs(mView->crs().toStdString());
    config->setGsd(mView->gsd());
    config->setMaxTiltAngle(mView->maxTiltAngle());
    config->setInterpolation(mView->interpolation().toStdString());

    auto orthomosaic_method = mView->orthomosaicMethod().toStdString();
    if (orthomosaic_method == "RLB") {

    } else if (orthomosaic_method == "SBB") {
        if (mSbb) {
            auto orthomosaic_properties = OrthomosaicPropertiesFactory::create(orthomosaic_method);
            orthomosaic_properties->setProperty("ExposureCompensator", mSbb->exposureCompensator().toStdString());
            orthomosaic_properties->setProperty("SeamFinder", mSbb->seamFinder().toStdString());
            orthomosaic_properties->setProperty("BlenderType", mSbb->blenderType().toStdString());
            config->setOrthomosaicProperties(orthomosaic_properties);
        } /*else if (mRlb) {

        }*/
    }

    mModel->setProperties(config);

    const auto orthoimage_count = undistorted_images.size();
    auto ortho_task = std::make_unique<OrthophotoTask>(config,
                                                       std::move(undistorted_images),
                                                       std::move(undistorted_cameras),
                                                       ortho_path,
                                                       mModel->dtmPath(),
                                                       mModel->enuCrs(),
                                                       mModel->useCuda());

    if(progressHandler()) {
        // The orthophoto and mosaic phases each account for half of the progress range.
        progressHandler()->setRange(0, orthoimage_count * 2);
        progressHandler()->setTitle(QApplication::translate("OrthophotoComponent", "Orthophoto"));
        progressHandler()->setDescription(QApplication::translate("OrthophotoComponent", "Orthophoto generation..."));
    }

    mView->hide();

    return ortho_task;
}

auto OrthophotoPresenterImp::undistortedImages(const ImageRepository &images,
                                               tl::Path &undistort_path) const -> std::unordered_map<size_t, Image>
{
    std::unordered_map<size_t, Image> undistorted_images;

    const auto &poses = mModel->poses();

    for (const auto &[image_id, pose] : poses) {

        Image image = *images.find(image_id);

        // Se utiliza el path de la imagen corregida de distorsión.
        tl::Path image_path = undistort_path;
        std::string file_name = std::to_string(image_id).append(".tif");
        image_path.append(file_name);
        image.setPath(image_path);

        // Se sustituye las poses importadas (EXIF) por las de la orientación
        CameraPose camera_pose = pose;
        auto rotation_matrix = camera_pose.rotationMatrix();
        rotation_matrix.at(1, 0) = -rotation_matrix.at(1, 0);
        rotation_matrix.at(1, 1) = -rotation_matrix.at(1, 1);
        rotation_matrix.at(1, 2) = -rotation_matrix.at(1, 2);
        rotation_matrix.at(2, 0) = -rotation_matrix.at(2, 0);
        rotation_matrix.at(2, 1) = -rotation_matrix.at(2, 1);
        rotation_matrix.at(2, 2) = -rotation_matrix.at(2, 2);
        camera_pose.setRotationMatrix(rotation_matrix);
        image.cameraPose() = camera_pose;

        int camera_id = image.cameraId();

        undistorted_images[image_id] = image;

    }

    return undistorted_images;
}

void OrthophotoPresenterImp::cancel()
{
    TaskPresenter::cancel();

    tl::Message::warning("Processing has been canceled by the user");
}


} // namespace graphos
