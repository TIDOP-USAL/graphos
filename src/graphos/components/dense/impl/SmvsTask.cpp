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

#include "graphos/components/dense/impl/SmvsTask.h"

#include "graphos/core/utils.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/image/Image.h"
#include "graphos/core/utils.h"
#include "graphos/core/orientation/GroundPoint.h"
#include "graphos/core/orientation/io/OrientationExport.h"
#include "graphos/core/ply.h"

/* TIDOP LIB */
#include <tidop/core/task/Task.h>
#include <tidop/core/base/Path.h>
#include <tidop/core/app/App.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/rastertools/io/Writer.h>
#include <tidop/core/task/Progress.h>
#include <tidop/core/task/Process.h>

/* COLMAP */
#include <colmap/util/string.h>

#include <iomanip>

namespace graphos
{

SmvsTask::SmvsTask(const std::shared_ptr<SmvsProperties> &properties,
                   const ImageRepository &imageRepo,
                   const CameraRepository &cameraRepo,
                   const CameraPosesRepository &poses,
                   const std::vector<GroundPoint> &groundPoints,
                   const tl::Path &outputPath,
                   bool cuda,
                   bool autoSegmentation)
  : DenseTaskBase(imageRepo, cameraRepo, poses, groundPoints, outputPath),
    mProperties(properties),
    mAutoSegmentation(autoSegmentation)
{
    DenseTaskBase::enableCuda(cuda);
    setUndistortImagesFormat(UndistortImages::Format::jpeg);
}

SmvsTask::~SmvsTask() = default;

void SmvsTask::clearPreviousModel() const
{
    outputPath().removeDirectory();
}

void SmvsTask::writeMveFile()
{

    try {

        tl::Path mve_path(outputPath());
        mve_path.append("synth_0.out");

        std::map<int, Undistort> undistort;

        for (auto &camera : cameras()) {
            undistort[camera.first] = Undistort(camera.second);
        }

        std::ofstream stream(mve_path.toString(), std::ios::trunc);

        if (stream.is_open()) {

            stream << "drews 1.0\n";
            stream << poses().size() << " " << groundPoints().size() << "\n";
            stream << std::fixed << std::setprecision(12);

            for (const auto &[image_id, pose] : poses()) {

                size_t mve_id = mGraphosToMveIds.size();
                mGraphosToMveIds[image_id] = mve_id;

                const auto image = images().find(image_id);
                if (!image) {
                    tl::Message::warning("Image with ID %zu not found in the repository. Skipping.", image_id);
                    continue;
                }

                int camera_id = image->cameraId();
                const auto camera = cameras().find(camera_id);
                if (!camera) {
                    tl::Message::warning("Camera with ID %d not found in the repository. Skipping image ID %zu.", camera_id, image_id);
                    continue;
                }
                Camera undistort_camera = undistort.at(camera_id).undistortCamera();
                double new_focal = undistort_camera.focal() / std::max(camera->width(), camera->height());
                double new_ppx = undistort_camera.calibration()->parameter(Calibration::Parameters::cx) / camera->width();
                double new_ppy = undistort_camera.calibration()->parameter(Calibration::Parameters::cy) / camera->height();
                auto projection_center = pose.position();
                auto rotation_matrix = pose.rotationMatrix();

                tl::Vector3d xyx = rotation_matrix * -projection_center.vector();

                tl::Path ini_file = outputPath();
                ini_file.append(colmap::StringPrintf("\\views\\view_%04d.mve", mve_id));
                ini_file.createDirectories();

                ini_file.append("meta.ini");
                std::ofstream stream_ini(ini_file.toString(), std::ios::trunc);

                if (stream_ini.is_open()) {

                    stream_ini << std::fixed << std::setprecision(12);

                    stream_ini << "# MVE view meta data is stored in INI-file syntax.\n";
                    stream_ini << "# This file is generated, formatting will get lost.\n\n";
                    stream_ini << "[camera]\n";
                    stream_ini << "focal_length = " << new_focal << "\n";
                    stream_ini << "pixel_aspect = " << 1. << "\n";
                    stream_ini << "principal_point = " << new_ppx << " " << new_ppy << "\n";
                    stream_ini << "rotation = " << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << " "
                               << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << " "
                               << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << "\n";
                    stream_ini << "translation = " << xyx[0] << " " << xyx[1] << " " << xyx[2] << "\n\n";
                    stream_ini << "[view]\n";
                    stream_ini << "id = " << mve_id << "\n";
                    stream_ini << "name = " << std::to_string(image_id) << std::endl;

                    stream_ini.close();
                }

                stream << new_focal << " " << "0" << " " << "0" << "\n";
                stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << "\n";
                stream << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << "\n";
                stream << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << "\n";
                stream << xyx[0] << " " << xyx[1] << " " << xyx[2] << std::endl;

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
                    stream << " " << mGraphosToMveIds.at(map.first) << " " << map.second << " 0";
                }

                stream << std::endl;

            }

            stream.close();

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void SmvsTask::densify()
{
    try {

        tl::Path app_path = tl::App::instance().path().parentPath();
        app_path.append("smvsrecon_SSE41");

        std::string cmd("\"");
        cmd.append(app_path.toUtf8());
        cmd.append("\" ");
        cmd.append("--scale=").append(std::to_string(mProperties->inputImageScale()));
        cmd.append(" --output-scale=").append(std::to_string(mProperties->outputDepthScale()));
        cmd.append(" --alpha=").append(std::to_string(mProperties->surfaceSmoothingFactor()));
        cmd.append(" --force ");
        if (!mProperties->semiGlobalMatching())
            cmd.append(" --no-sgm ");
        if (mProperties->shadingBasedOptimization())
            cmd.append(" --shading ");
        cmd.append("\"").append(outputPath().toUtf8());

        tl::Process process(cmd);

        process.run();

        TL_ASSERT(process.status() == tl::Process::Status::finalized, "Densify Point Cloud error");

        std::string model_name("smvs-");
        if (mProperties->shadingBasedOptimization())
            model_name.append("S");
        else
            model_name.append("B");
        model_name.append(std::to_string(mProperties->inputImageScale())).append(".ply");

        tl::Path dense_model = outputPath();
        dense_model.append(model_name);

        TL_ASSERT(dense_model.exists(), "Densify Point Cloud error");

        setDenseModel(dense_model);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void SmvsTask::copyUndistortedImages() const
{
    tl::Path undistort_path(outputPath().parentPath().parentPath());
    undistort_path.append("undistorted");
    tl::Path output_path(outputPath());
    output_path.append("views");

    for (const auto &[image_id, pose] : poses()) {

        //const auto image = images().find(image_id);
        //if (!image) continue;

        tl::Path undistort_image_path = undistort_path;
        std::string file_name = std::to_string(image_id).append(".tif");
        undistort_image_path.append(file_name);

        tl::RasterReader image_reader(undistort_image_path);
        if (image_reader.isOpen()) {

            tl::Path image_out_path = output_path;
            image_out_path.append(colmap::StringPrintf("view_%04d.mve", mGraphosToMveIds.at(image_id)));
            image_out_path.append("undistorted.jpg");

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

            tl::RasterWriter image_writer(image_out_path);
            if (image_writer.isOpen()) {
                image_writer.create(mat.rows, mat.cols, mat.channels(), tl::DataType::TL_8U);
                image_writer.write(mat);
                image_writer.close();
            }
        }
    }
}

void SmvsTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{

    try {

        this->clearPreviousModel();

        outputPath().createDirectories();

        tl::Path undistort_path(outputPath().parentPath().parentPath());
        undistort_path.append("undistorted");
        undistort_path.createDirectories();

        this->writeMveFile();

        if (status() == tl::Task::Status::stopping) return;

        this->undistort(QString::fromStdWString(undistort_path.toWString()));
        this->copyUndistortedImages();

        if (status() == Status::stopping) return;

        this->densify();
        if (mAutoSegmentation) this->autoSegmentation();

        Ply ply(denseModel());
        mReport.points = static_cast<int>(ply.size());
        ply.close();
        mReport.cuda = isCudaEnabled();
        mReport.method = mProperties->name();
        mReport.time = this->time();

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("MVS error");
    }

}

} // namespace graphos
