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

#include "graphos/components/dense/impl/CmvsPmvsTask.h"

#include "graphos/core/utils.h"
#include "graphos/core/camera/Calibration.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/orientation/GroundPoint.h"
#include "graphos/core/orientation/io/OrientationExport.h"
#include "graphos/core/ply.h"

/* TidopLib */ 

#include <tidop/core/task/Task.h>
#include <tidop/core/base/Path.h>
#include <tidop/core/app/App.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/rastertools/io/Writer.h>
#include <tidop/core/task/Process.h>
#include <tidop/core/task/Progress.h>

/* COLMAP */

#include <colmap/scene/database.h>
#include <colmap/util/string.h>

#include <fstream>
#include <set>

using namespace tl;

namespace graphos
{


CmvsPmvsTask::CmvsPmvsTask(const std::shared_ptr<CmvsPmvsProperties> &properties,
                           const ImageRepository &imageRepo,
                           const CameraRepository &cameraRepo,
                           const CameraPosesRepository &poses,
                           const std::vector<GroundPoint> &groundPoints,
                           const tl::Path &outputPath,
                           tl::Path database,
                           bool cuda,
                           bool autoSegmentation)
  : DenseTaskBase(imageRepo, cameraRepo, poses, groundPoints, outputPath),
    mProperties(properties),
    mDatabase(std::move(database)),
    mAutoSegmentation(autoSegmentation)
{
    DenseTaskBase::enableCuda(cuda);
    setUndistortImagesFormat(UndistortImages::Format::jpeg);
}

CmvsPmvsTask::~CmvsPmvsTask() = default;

void CmvsPmvsTask::clearPreviousModel() const
{
    outputPath().removeDirectory();
}

void CmvsPmvsTask::writeBundleFile()
{
    try {

        auto database = colmap::Database::Open(mDatabase.toUtf8());
        const auto &colmap_images = database->ReadAllImages();

        std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;

        for (const auto &pose : poses()) {

            size_t graphos_image_id = pose.first;
            if (!images().contains(graphos_image_id)) {
                continue;
            }
            Path image_path(images().find(graphos_image_id)->path());

            for (const auto &colmap_image : colmap_images) {
                Path colmap_image_path(colmap_image.Name());

                if (image_path.equivalent(colmap_image_path)) {
                    graphos_to_colmap_image_ids[graphos_image_id] = colmap_image.ImageId();
                    break;
                }
            }

        }


        Path bundler_path(outputPath());
        bundler_path.append("bundle.rd.out");
        Path bundler_path_list(outputPath());
        bundler_path_list.append("bundle.rd.out.list.txt");

        std::map<int, Undistort> undistort;

        for (auto &[camera_id, camera] : cameras()) {
            undistort[camera_id] = Undistort(camera);
        }

        std::ofstream stream(bundler_path.toString(), std::ios::trunc);
        std::ofstream stream_image_list(bundler_path_list.toString(), std::ios::trunc);

        if (stream.is_open() && stream_image_list.is_open()) {

            size_t camera_count = poses().size();
            size_t ground_points_count = groundPoints().size();

            stream << "# Bundle file v0.3\n";
            stream << camera_count << " " << ground_points_count << "\n";

            for (const auto &pose : poses()) {

                size_t image_id = pose.first;
                size_t bundler_image_id = mGraphosToBundlerIds.size();
                mGraphosToBundlerIds[image_id] = bundler_image_id;

                const auto image = images().find(image_id);
                if (!image) {
                    tl::Message::warning("Image with ID %zu not found in the repository. Skipping.", image_id);
                    continue;
                }
                int camera_id = image->cameraId();

                Camera undistort_camera = undistort.at(camera_id).undistortCamera();
                double new_focal = undistort_camera.focal();

                auto projection_center = pose.second.position();
                auto rotation_matrix = pose.second.rotationMatrix();

                tl::Vector3d xyx = rotation_matrix * -projection_center.vector();

                // En el formato bundler r10, r11, r12, r20, r21, r22, T1 y T2 se invierte el signo
                stream << new_focal << " 0 0 \n";
                stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << "\n";
                stream << -rotation_matrix(1, 0) << " " << -rotation_matrix(1, 1) << " " << -rotation_matrix(1, 2) << "\n";
                stream << -rotation_matrix(2, 0) << " " << -rotation_matrix(2, 1) << " " << -rotation_matrix(2, 2) << "\n";
                stream << xyx[0] << " " << -xyx[1] << " " << -xyx[2] << std::endl;

                stream_image_list << colmap::StringPrintf("%08d.jpg", bundler_image_id) << std::endl;


                /// Write Projection Matrix
                {

                    tl::Path proj_matrix_path = outputPath();
                    proj_matrix_path.append(colmap::StringPrintf("/txt/%08d.txt", bundler_image_id));

                    std::ofstream file(proj_matrix_path.toString(), std::ios::trunc);
                    TL_ASSERT(file.is_open(), "Write Projection Matrix Error");

                    Matrix<double, 3, 3> calib_matrix = Matrix<double, 3, 3>::identity();
                    calib_matrix[0][0] = undistort_camera.calibration()->parameter(Calibration::Parameters::focalx);
                    calib_matrix[1][1] = undistort_camera.calibration()->parameter(Calibration::Parameters::focaly);
                    calib_matrix[0][2] = undistort_camera.calibration()->parameter(Calibration::Parameters::cx);
                    calib_matrix[1][2] = undistort_camera.calibration()->parameter(Calibration::Parameters::cy);

                    Matrix<double, 3, 4> proj_matrix;
                    proj_matrix[0][0] = rotation_matrix[0][0];
                    proj_matrix[0][1] = rotation_matrix[0][1];
                    proj_matrix[0][2] = rotation_matrix[0][2];
                    proj_matrix[1][0] = rotation_matrix[1][0];
                    proj_matrix[1][1] = rotation_matrix[1][1];
                    proj_matrix[1][2] = rotation_matrix[1][2];
                    proj_matrix[2][0] = rotation_matrix[2][0];
                    proj_matrix[2][1] = rotation_matrix[2][1];
                    proj_matrix[2][2] = rotation_matrix[2][2];
                    proj_matrix[0][3] = xyx[0];
                    proj_matrix[1][3] = xyx[1];
                    proj_matrix[2][3] = xyx[2];

                    proj_matrix = calib_matrix * proj_matrix;

                    file << "CONTOUR" << '\n';
                    file << proj_matrix << std::endl;

                }


            }

            for (auto &points_3d : groundPoints()) {

                stream << points_3d.x() << " "
                       << points_3d.y() << " "
                       << points_3d.z() << "\n";

                stream << points_3d.color().red() << " "
                       << points_3d.color().green() << " "
                       << points_3d.color().blue() << "\n";

                auto &track = points_3d.track();

                stream << track.size();

                for (auto &map : track.pairs()) {

                    size_t image_id = map.first;
                    size_t point_id = map.second;

                    auto keypoints = database->ReadKeypoints(graphos_to_colmap_image_ids.at(image_id));
                    const auto image = images().find(image_id);
                    if (!image) {
                        tl::Message::warning("Image with ID %zu not found in the repository. Skipping.", image_id);
                        continue;
                    }
                    auto &_undistort = undistort.at(image->cameraId());

                    Camera undistort_camera = _undistort.undistortCamera();
                    float ppx = static_cast<float>(undistort_camera.calibration()->parameter(Calibration::Parameters::cx));
                    float ppy = static_cast<float>(undistort_camera.calibration()->parameter(Calibration::Parameters::cy));

                    Point<float> undistort_point = _undistort.undistortPoint(Point<float>(keypoints[point_id].x, keypoints[point_id].y));
                    stream << " " << static_cast<int>(mGraphosToBundlerIds.at(image_id))
                        << " " << point_id
                        << " " << undistort_point.x() - ppx
                        << " " << ppy - undistort_point.y() << " ";

                }

                stream << '\n';

            }

            stream.close();
            stream_image_list.close();

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Write Bundle file error");
    }
}

void CmvsPmvsTask::writeVisibility()
{

    try {

        ///////////////////////////////////////////////////////////////////////////////
        /// TODO: Repetido
        auto database = colmap::Database::Open(mDatabase.toUtf8());
        const auto &colmap_images = database->ReadAllImages();

        std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;

        for (const auto &pose : poses()) {

            size_t graphos_image_id = pose.first;
            const auto image = images().find(graphos_image_id);
            if (!image) {
                continue;
            }
            tl::Path image_path(image->path());
            for (const auto &colmap_image : colmap_images) {
                Path colmap_image_path(colmap_image.Name());

                if (image_path.equivalent(colmap_image_path)) {
                    graphos_to_colmap_image_ids[graphos_image_id] = colmap_image.ImageId();
                    break;
                }
            }

        }

        //////////////////////////////////////////////////////////////////////////////


        Path visibility_path(outputPath());
        visibility_path.append("vis.dat");
        std::ofstream stream(visibility_path.toString(), std::ios::trunc);
        if (stream.is_open()) {

            stream << "VISDATA" << '\n';
            stream << poses().size() << '\n';

            size_t max_size = poses().size() - 1;

            for (const auto &pose : poses()) {

                size_t image_id = pose.first;

                std::set<size_t> visible_image_ids;

                for (auto &points_3d : groundPoints()) {

                    if (max_size == visible_image_ids.size()) break;

                    auto &track = points_3d.track();

                    if (track.existPair(image_id)) {

                        for (auto &map : track.pairs()) {

                            size_t second_image_id = map.first;
                            if (second_image_id != image_id) {
                                visible_image_ids.insert(second_image_id);
                            }

                        }

                    }

                }

                stream << mGraphosToBundlerIds[image_id] << " " << visible_image_ids.size();

                for (auto id : visible_image_ids) {
                    stream << " " << mGraphosToBundlerIds[id];
                }

                stream << '\n';
            }

            stream.close();

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Write Visibility file error");
    }
}

void CmvsPmvsTask::writeOptions() const
{
    try {

        ///// options

        Path options_path(outputPath());
        options_path.append("option-all");
        std::ofstream file_options(options_path.toString(), std::ios::trunc);
        TL_ASSERT(file_options.is_open(), "Can't open file")

        TL_TODO("Si hay muchas imagenes separar en clusters pero con solape ya que los generados por cmvs dejan huecos al fusionar");

        file_options << "# Generated by Graphos - all images, no clustering.\n";

        file_options << "level " << mProperties->level() << "\n";
        file_options << "csize " << mProperties->cellSize() << "\n";
        file_options << "threshold " << mProperties->threshold() << "\n";
        file_options << "wsize " << mProperties->windowSize() << "\n";
        file_options << "minImageNum " << mProperties->minimunImageNumber() << "\n";
        file_options << "CPU " << std::thread::hardware_concurrency() << "\n";
        file_options << "setEdge 0" << "\n";
        file_options << "useBound 0" << "\n";
        file_options << "useVisData " << (mProperties->useVisibilityInformation() ? 1 : 0) << "\n";
        file_options << "sequence -1" << "\n";
        file_options << "maxAngle 10" << "\n";
        file_options << "quad 2.0" << "\n";

        file_options << "timages " << poses().size();

        for (auto &pose : poses()) {

            file_options << " " << mGraphosToBundlerIds.at(pose.first);
        }

        file_options << "\n";

        file_options << "oimages 0" << std::endl;

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Write Options file error");
    }
}

void CmvsPmvsTask::densify()
{

    try {

        Path app_path = tl::App::instance().path().parentPath();
        app_path.append("pmvs2");

        std::string cmd_cmvs("\"");
        cmd_cmvs.append(app_path.toUtf8());
        cmd_cmvs.append("\" \"");
        cmd_cmvs.append(outputPath().toUtf8());
        cmd_cmvs.append("/\" option-all");

        Message::info("Process: {}", cmd_cmvs);
        tl::Process process(cmd_cmvs);
        process.run();

        TL_ASSERT(process.status() == Process::Status::finalized, "Densify Point Cloud error");

        Path dense_model = outputPath();
        dense_model.append("models");
        dense_model.append("option-all.ply");

        TL_ASSERT(dense_model.exists(), "Densify Point Cloud error");

        setDenseModel(dense_model);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void CmvsPmvsTask::copyUndistortedImages() const
{
    tl::Path undistort_path(outputPath().parentPath().parentPath());
    undistort_path.append("undistorted");
    Path output_path = outputPath();
    output_path.append("visualize");

    for (const auto &[image_id, pose] : poses()) {

        Path undistort_image_path = undistort_path;
        std::string file_name = std::to_string(image_id).append(".tif");
        undistort_image_path.append(file_name);

        RasterReader image_reader(undistort_image_path);
        if (image_reader.isOpen()) {

            Path image_out_path = output_path;
            image_out_path.append(colmap::StringPrintf("%08d.jpg", mGraphosToBundlerIds.at(image_id)));

            cv::Mat mat = image_reader.read();

            double nodata_value = tl::NoData<float>;
            if (mat.type() == CV_32F)
                nodata_value = tl::NoData<float>;
            else if (mat.type() == CV_64F)
                nodata_value = tl::NoData<double>;
            else
                nodata_value = 0;

            cv::Mat mask;
            cv::inRange(mat, cv::Scalar::all(nodata_value), cv::Scalar::all(nodata_value), mask);
            cv::bitwise_not(mask, mask);

            normalizeImage(mat, mat, this->isCudaEnabled(), mask);
            cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);

            RasterWriter image_writer(image_out_path);
            if (image_writer.isOpen()) {
                image_writer.create(mat.rows, mat.cols, 3, tl::DataType::TL_8U);
                image_writer.write(mat);
                image_writer.close();
            }
        }
    }
}


void CmvsPmvsTask::execute(Progress *progressBar, std::stop_token stopToken)
{

    try {
        this->clearPreviousModel();

        outputPath().createDirectories();

        Path txt_path(outputPath());
        txt_path.append("txt");
        txt_path.createDirectory();

        Path visualize_path(outputPath());
        visualize_path.append("visualize");
        visualize_path.createDirectory();

        Path models_path(outputPath());
        models_path.append("models");
        models_path.createDirectory();

        writeBundleFile();

        if (status() == Status::stopping) return;

        writeVisibility();

        if (status() == Status::stopping) return;

        writeOptions();

        if (status() == Status::stopping) return;

        Path undistort_path(outputPath().parentPath().parentPath());
        undistort_path.append("undistorted");
        undistort_path.createDirectories();
        this->undistort(QString::fromStdWString(undistort_path.toWString()));
        this->copyUndistortedImages();

        if (status() == Status::stopping) return;

        this->densify();
        if (mAutoSegmentation) this->autoSegmentation();

        Ply ply(denseModel());
        mReport.points = static_cast<int>(ply.size());
        ply.close();
        mReport.cuda = isCudaEnabled();
        mReport.method = this->mProperties->name();
        mReport.time = this->time();

        Message::success("Densification finished in {:.2} minutes", mReport.time / 60.);

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("MVS error");
    }
}


} // namespace graphos
