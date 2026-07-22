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

#include "graphos/core/task/Progress.h"
#include "graphos/components/orthophoto/impl/OrthophotoModel.h"
#include "graphos/components/orthophoto/impl/OrthophotoView.h"
#include "graphos/components/orthophoto/impl/OrthophotoTask.h"

//#include <tidop/rastertools/io/Reader.h>

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

auto OrthophotoPresenterImp::undistortedCameras(const std::map<int, Camera> &cameras) const -> std::map<int, Camera>
{
    std::map<int, Camera> undistorted_cameras;

    for (const auto &camera_pair : cameras) {

        int camera_id = camera_pair.first;
        const auto &camera = camera_pair.second;

        Undistort undistort(camera);
        Camera undistort_camera = undistort.undistortCamera();

        //if (camera.hasCalibratedHMatrix()) {

        //    auto calibrated_h_matrix = camera.calibratedHMatrix();
        //    cv::Mat H = cv::Mat::eye(3, 3, CV_32F);
        //    H.at<float>(0, 0) = calibrated_h_matrix(0, 0);
        //    H.at<float>(0, 1) = calibrated_h_matrix(0, 1);
        //    H.at<float>(0, 2) = calibrated_h_matrix(0, 2);
        //    H.at<float>(1, 0) = calibrated_h_matrix(1, 0);
        //    H.at<float>(1, 1) = calibrated_h_matrix(1, 1);
        //    H.at<float>(1, 2) = calibrated_h_matrix(1, 2);
        //    H.at<float>(2, 0) = calibrated_h_matrix(2, 0);
        //    H.at<float>(2, 1) = calibrated_h_matrix(2, 1);
        //    H.at<float>(2, 2) = calibrated_h_matrix(2, 2);

        //    auto &calibration = camera.calibration() ? camera.calibration() : camera.priorCalibration();
        //    float cx = static_cast<float>(calibration->parameter(Calibration::Parameters::cx));
        //    float cy = static_cast<float>(calibration->parameter(Calibration::Parameters::cy));

        //    std::vector<cv::Point2f> srcPoints = {{cx, cy}};
        //    std::vector<cv::Point2f> dstPoints;

        //    cv::perspectiveTransform(srcPoints, dstPoints, H);

        //    auto &calibration2 = undistort_camera.calibration() ? undistort_camera.calibration() : undistort_camera.priorCalibration();
        //    calibration2->setParameter(Calibration::Parameters::cx, static_cast<double>(dstPoints[0].x));
        //    calibration2->setParameter(Calibration::Parameters::cy, static_cast<double>(dstPoints[0].y));

        //}

        undistorted_cameras[camera_id] = undistort_camera;

    }

    return undistorted_cameras;
}

void OrthophotoPresenterImp::open()
{
    mModel->loadSettings();

    mView->setCrs(mModel->crs());

    mView->exec();
}

void OrthophotoPresenterImp::init()
{

}

void OrthophotoPresenterImp::initSignalAndSlots()
{
    connect(mView, &TaskView::run, this, &TaskPresenter::run);
    connect(mView, &DialogView::help, [&]() {
        emit help("orthophoto.html");
    });
}

void OrthophotoPresenterImp::onError(tl::TaskErrorEvent *event)
{
    TaskPresenter::onError(event);

    if(progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("OrthophotoComponent", "Orthophoto task error"));
    }
}

void OrthophotoPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
    TaskPresenter::onFinished(event);

    if(progressHandler()) {
        progressHandler()->setDescription(QApplication::translate("OrthophotoComponent", "Orthophoto finished"));
    }
}

std::unique_ptr<tl::Task> OrthophotoPresenterImp::createTask()
{
    auto task_list = std::make_unique<tl::TaskList>();

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
    //const auto &poses = mModel->poses();
    bool use_cuda = mModel->useCuda();

    tl::Path undistort_path(mModel->projectFolder());
    undistort_path.append("undistorted");

    std::list<tl::Path> undistorted_image_paths;
    if (undistort_path.exists()) {
        undistorted_image_paths = undistort_path.list(".tif");
    }

    if (undistorted_image_paths.size() != images.size()) {

        undistort_path.createDirectories();

        auto undistort = std::make_shared<UndistortImages>(images,
                                                           cameras,
                                                           undistort_path,
                                                           UndistortImages::Format::tiff,
                                                           use_cuda,
                                                           true);
        task_list->push_back(undistort);
    }



    auto undistorted_cameras = undistortedCameras(cameras);
    auto undistorted_images = undistortedImages(images, undistort_path);


    // TODO: Tiene que incluir el tipo de producto (RGB, Thermal, Red, NIR)
    tl::Path ortho_path = mModel->projectFolder();
    ortho_path.append("ortho");
    ortho_path.append("orthomosaic.tif");

    //for (auto &image : images) {

    //    auto image_id = Image::id(image);
    //    tl::Path undistorted_image = undistort_path;
    //    std::string file_name = std::to_string(image_id).append(".tif");
    //    undistorted_image.append(file_name);

    //    image.setPath(file_name);

    //}

    auto ortho_task = std::make_shared<OrthophotoTask>(undistorted_images,
                                                       undistorted_cameras,
                                                       ortho_path,
                                                       mModel->dtmPath(),
                                                       mModel->enuCrs().toStdString(),
                                                       mView->crs().toStdString(),
                                                       mView->interpolation().toStdString(),
                                                       mView->gsd(),
                                                       mModel->useCuda());

    ortho_task->subscribe([&, ortho_path](const tl::TaskFinalizedEvent *event) {

        try {

            auto task = dynamic_cast<OrthophotoTask const *>(event->task());
            if (task) {
                auto report = task->report();
                OrthophotoData ortho_data;
                ortho_data.path = ortho_path;
                ortho_data.epsgCode = mView->crs();
                ortho_data.gsd = mView->gsd();
                ortho_data.interpolation = mView->interpolation();
                ortho_data.report = report;
                mModel->addOrthophoto(ortho_data);
            }

        } catch (std::exception &e) {
            tl::printException(e);
        }

    });

    task_list->push_back(ortho_task);

    if(progressHandler()) {
        progressHandler()->setRange(0, mModel->images().size()*4);
        progressHandler()->setTitle(QApplication::translate("OrthophotoComponent", "Orthophoto"));
        progressHandler()->setDescription(QApplication::translate("OrthophotoComponent", "Orthophoto generation..."));
    }

    mView->hide();

    return task_list;
}

auto OrthophotoPresenterImp::undistortedImages(const std::unordered_map<size_t, Image> &images,
                                               tl::Path &undistort_path) const -> std::unordered_map<size_t, Image>
{
    std::unordered_map<size_t, Image> undistorted_images;

    const auto &poses = mModel->poses();

    for (const auto &pose : poses) {

        size_t image_id = pose.first;
        Image image = images.at(image_id);

        // Se utiliza el path de la imagen corregida de distorsión.
        tl::Path image_path = undistort_path;
        std::string file_name = std::to_string(image_id).append(".tif");
        image_path.append(file_name);
        image.setPath(image_path);

        // Se sustituye las poses importadas (EXIF) por las de la orientación
        CameraPose camera_pose = pose.second;
        auto rotation_matrix = camera_pose.rotationMatrix();
        rotation_matrix.at(1, 0) = -rotation_matrix.at(1, 0);
        rotation_matrix.at(1, 1) = -rotation_matrix.at(1, 1);
        rotation_matrix.at(1, 2) = -rotation_matrix.at(1, 2);
        rotation_matrix.at(2, 0) = -rotation_matrix.at(2, 0);
        rotation_matrix.at(2, 1) = -rotation_matrix.at(2, 1);
        rotation_matrix.at(2, 2) = -rotation_matrix.at(2, 2);
        camera_pose.setRotationMatrix(rotation_matrix);
        image.setCameraPose(camera_pose);

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
