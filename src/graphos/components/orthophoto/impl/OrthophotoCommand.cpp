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

#include "OrthophotoCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project/Project.h"
#include "graphos/core/project/io/ProjectReader.h"
#include "graphos/core/project/io/ProjectWriter.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/components/orthophoto/impl/OrthophotoTask.h"
#include "graphos/core/orientation/io/CameraPosesReader.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/app/Logger.h>
#include <tidop/core/base/Split.h>
#include <tidop/core/base/TypeConversions.h>
#include <tidop/core/console/ValuesValidator.h>
#include <tidop/core/base/TypeConversions.h>
//#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>


using namespace tl;

namespace graphos
{

OrthophotoCommand::OrthophotoCommand()
  : Command("ortho", "Create orthophoto"),
    //mProject(nullptr),
    mDisableCuda(false)
{
    this->addArgument<Path>("prj", 'p', "Project file");
    this->addArgument<double>("gsd", 'g', "Ground sample distance", 0.1);
    //this->addArgument<Path>("dsm", "Digital Surface Model");
    this->addArgument<std::string>("crs", "Coordinate Reference System", "");
    auto arg_interpolation = Argument::make<std::string>("interpolation", 'i', "Interpolation", "BILINEAR");
    std::vector<std::string> interpolation_methods{"NEAREST", "BILINEAR", "BICUBIC"};
    arg_interpolation->setValidator(std::make_shared<ValuesValidator<std::string>>(interpolation_methods));
    this->addArgument(arg_interpolation);

#ifdef HAVE_CUDA
    tl::Message::pauseMessages();
    bool cuda_enabled = cudaEnabled(10.0, 3.0);
    tl::Message::resumeMessages();
    if (cuda_enabled)
        this->addArgument<bool>("disable_cuda", "If true disable CUDA (default = false)", mDisableCuda);
    else mDisableCuda = true;
#else
    mDisableCuda = true;
#endif //HAVE_CUDA

    this->addExample("ortho -p 253/253.xml --gsd 0.1");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

bool OrthophotoCommand::run()
{
    bool r = false;

    auto &log = tl::Logger::instance();

    try {

        tl::Path project_path = this->value<Path>("prj");
        auto gsd =  this->value<double>("gsd");
        auto crs =  this->value<std::string>("crs");
        auto interpolation =  this->value<std::string>("interpolation");
        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        Project project;
        ProjectReader reader;
        reader.read(project_path, project);

        auto &project_info = project.info();

        auto enu_crs = project_info.enuCrs();

        if (crs.empty()) {

            //// Esto no tiene que hacerse ya que vamos a tener las coordenadas geograficas directamente
            //auto epsg_geographic = std::make_shared<tl::Crs>("EPSG:4326");
            //auto epsg_geocentric = std::make_shared<tl::Crs>("EPSG:4978");
            //tl::CrsTransform crs_transfom_geocentric_to_geographic(epsg_geocentric, epsg_geographic);
            //auto lla = crs_transfom_geocentric_to_geographic.transform(offset);

            ////auto zone = tl::utmZoneFromLonLat(lla.x, lla.y);
            //int zone = tl::utmZoneFromLongitude(lla.x);
            //crs = "EPSG:326";
            //crs.append(std::to_string(zone));
            auto v = tl::split<std::string>(enu_crs, ';');
            auto zone = tl::utmZoneFromLonLat(tl::convertStringTo<double>(v.at(1)), tl::convertStringTo<double>(v.at(2)));
            crs = "EPSG:326";
            crs.append(std::to_string(zone.first));
        }

        const auto &images = project.images();
        const auto &cameras = project.cameras();

        tl::Path undistort_path(project_info.projectFolder());
        undistort_path.append("undistorted");

        auto undistorted_image_paths = undistort_path.list(".tif");

        if (!undistort_path.exists() || undistorted_image_paths.size() != images.size()) {

            undistort_path.createDirectories();

            UndistortImages undistort_task(images,
                                           cameras,
                                           undistort_path,
                                           UndistortImages::Format::tiff,
                                           !mDisableCuda,
                                           true);
            undistort_task.run();
        }

        
        auto undistorted_images = undistortedImages(images, project.cameraPoses(), undistort_path);
        auto undistorted_cameras = undistortedCameras(cameras);
        auto dsm_path = project.dsm();
        tl::Path ortho_path = project_info.projectFolder();
        ortho_path.append("ortho");
        ortho_path.append("orthomosaic.tif");

        OrthophotoTask orthophoto_task(undistorted_images,
                                       undistorted_cameras,
                                       ortho_path,
                                       dsm_path,
                                       enu_crs,
                                       crs,
                                       interpolation,
                                       gsd, 
                                       !mDisableCuda);
        orthophoto_task.run();

        //tl::Path orthophoto_file = ortho_path;
        //orthophoto_file.append("ortho.tif");

        if (ortho_path.exists()) {

            auto report = orthophoto_task.report();
            project.setOrthoReport(report);

            auto ortho_data = std::make_shared<OrthophotoProperties>();
            ortho_data->setCrs(crs);
            ortho_data->setGsd(gsd);
            ortho_data->setInterpolation(interpolation);
            project.setOrthoConfig(ortho_data);

            project.setOrthophoto(ortho_path);

            ProjectWriter writer;
            writer.write(project_path, project);
        }

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

auto OrthophotoCommand::undistortedCameras(const CameraRepository &cameraRepo) const -> std::map<int, Camera>
{
    std::map<int, Camera> undistorted_cameras;

    for (const auto &[camera_id, camera] : cameraRepo) {

        Undistort undistort(camera);
        Camera undistort_camera = undistort.undistortCamera();

        if (camera.calibratedHMatrix()) {

            auto &calibrated_h_matrix = camera.calibratedHMatrix().value();
            cv::Mat H = cv::Mat::eye(3, 3, CV_32F);
            H.at<float>(0, 0) = calibrated_h_matrix(0, 0);
            H.at<float>(0, 1) = calibrated_h_matrix(0, 1);
            H.at<float>(0, 2) = calibrated_h_matrix(0, 2);
            H.at<float>(1, 0) = calibrated_h_matrix(1, 0);
            H.at<float>(1, 1) = calibrated_h_matrix(1, 1);
            H.at<float>(1, 2) = calibrated_h_matrix(1, 2);
            H.at<float>(2, 0) = calibrated_h_matrix(2, 0);
            H.at<float>(2, 1) = calibrated_h_matrix(2, 1);
            H.at<float>(2, 2) = calibrated_h_matrix(2, 2);

            auto calibration = camera.calibration() ? camera.calibration() : camera.priorCalibration();
            float cx = static_cast<float>(calibration->parameter(Calibration::Parameters::cx));
            float cy = static_cast<float>(calibration->parameter(Calibration::Parameters::cy));

            std::vector<cv::Point2f> srcPoints = {{cx, cy}};
            std::vector<cv::Point2f> dstPoints;

            cv::perspectiveTransform(srcPoints, dstPoints, H);

            auto calibration2 = undistort_camera.calibration() ? undistort_camera.calibration() : undistort_camera.priorCalibration();
            calibration2->setParameter(Calibration::Parameters::cx, static_cast<double>(dstPoints[0].x));
            calibration2->setParameter(Calibration::Parameters::cy, static_cast<double>(dstPoints[0].y));

        }

        undistorted_cameras[camera_id] = undistort_camera;

    }

    return undistorted_cameras;
}

auto OrthophotoCommand::undistortedImages(const ImageRepository &imageRepo, 
                                          const CameraPosesRepository &cameraPosesRepo,
                                          const tl::Path &undistort_path) const -> std::unordered_map<size_t, Image>
{
    std::unordered_map<size_t, Image> undistorted_images;

    for (const auto &[image_id, pose] : cameraPosesRepo) {

        Image image = *imageRepo.find(image_id);

        // Se utiliza el path de la imagen corregida de distorsión.
        tl::Path image_path = undistort_path;
        std::string file_name = std::to_string(image_id).append(".tif");
        image_path.append(file_name);
        image.setPath(image_path);

        // Se sustituyen las poses importadas (EXIF) por las de la orientación
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

        //int camera_id = image.cameraId();

        undistorted_images[image_id] = image;

    }

    return undistorted_images;
}

} // namespace graphos