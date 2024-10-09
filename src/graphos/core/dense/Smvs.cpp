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

#include "graphos/core/dense/Smvs.h"

#include "graphos/core/utils.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/sfm/orientationexport.h"
#include "graphos/core/ply.h"

/* TIDOP LIB */
#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/core/app.h>
#include <tidop/img/imgreader.h>
#include <tidop/core/progress.h>

/* COLMAP */
#include <colmap/util/string.h>

#include <iomanip>

namespace graphos
{

constexpr auto smvs_default_input_image_scale = 1;
constexpr auto smvs_default_output_depth_scale = 2;
constexpr auto smvs_default_shading_based_optimization = false;
constexpr auto smvs_default_semi_global_matching = false;
constexpr auto smvs_default_surface_smoothing_factor = 1.0;

Smvs::Smvs()
  : Densification(Method::smvs),
    mInputImageScale(smvs_default_input_image_scale),
    mOutputDepthScale(smvs_default_output_depth_scale),
    mShadingBasedOptimization(smvs_default_shading_based_optimization),
    mSemiGlobalMatching(smvs_default_semi_global_matching),
    mSurfaceSmoothingFactor(smvs_default_surface_smoothing_factor)
{
}

Smvs::Smvs(int inputImageScale,
                               int outputDepthScale,
                               bool shadingBasedOptimization,
                               bool semiGlobalMatching,
                               double surfaceSmoothingFactor)
  : Densification(Method::smvs),
    mInputImageScale(inputImageScale),
    mOutputDepthScale(outputDepthScale),
    mShadingBasedOptimization(shadingBasedOptimization),
    mSemiGlobalMatching(semiGlobalMatching),
    mSurfaceSmoothingFactor(surfaceSmoothingFactor)
{
}

Smvs::Smvs(const Smvs &smvs)
  : Densification(smvs),
    mInputImageScale(smvs.mInputImageScale),
    mOutputDepthScale(smvs.mOutputDepthScale),
    mShadingBasedOptimization(smvs.mShadingBasedOptimization),
    mSemiGlobalMatching(smvs.mSemiGlobalMatching),
    mSurfaceSmoothingFactor(smvs.mSurfaceSmoothingFactor)
{
}

Smvs::Smvs(Smvs &&smvs) noexcept
  : Densification(std::forward<Densification>(smvs)),
    mInputImageScale(smvs.mInputImageScale),
    mOutputDepthScale(smvs.mOutputDepthScale),
    mShadingBasedOptimization(smvs.mShadingBasedOptimization),
    mSemiGlobalMatching(smvs.mSemiGlobalMatching),
    mSurfaceSmoothingFactor(smvs.mSurfaceSmoothingFactor)
{
}

auto Smvs::operator =(const Smvs& smvs) -> Smvs&
{
    if (this != &smvs) {
        this->mInputImageScale = smvs.mInputImageScale;
        this->mOutputDepthScale = smvs.mOutputDepthScale;
        this->mShadingBasedOptimization = smvs.mShadingBasedOptimization;
        this->mSemiGlobalMatching = smvs.mSemiGlobalMatching;
        this->mSurfaceSmoothingFactor = smvs.mSurfaceSmoothingFactor;
    }
    return *this;
}

auto Smvs::operator =(Smvs&& smvs) noexcept -> Smvs&
{
    if (this != &smvs) {
        this->mInputImageScale = smvs.mInputImageScale;
        this->mOutputDepthScale = smvs.mOutputDepthScale;
        this->mShadingBasedOptimization = smvs.mShadingBasedOptimization;
        this->mSemiGlobalMatching = smvs.mSemiGlobalMatching;
        this->mSurfaceSmoothingFactor = smvs.mSurfaceSmoothingFactor;
    }
    return *this;
}

auto Smvs::inputImageScale() const -> int
{
    return mInputImageScale;
}

auto Smvs::outputDepthScale() const -> int
{
    return mOutputDepthScale;
}

auto Smvs::shadingBasedOptimization() const -> bool
{
    return mShadingBasedOptimization;
}

auto Smvs::semiGlobalMatching() const -> bool
{
    return mSemiGlobalMatching;
}

auto Smvs::surfaceSmoothingFactor() const -> double
{
    return mSurfaceSmoothingFactor;
}

void Smvs::setInputImageScale(int inputImageScale)
{
    mInputImageScale = inputImageScale;
}

void Smvs::setOutputDepthScale(int outputDepthScale)
{
    mOutputDepthScale = outputDepthScale;
}

void Smvs::setShadingBasedOptimization(bool shadingBasedOptimization)
{
    mShadingBasedOptimization = shadingBasedOptimization;
}

void Smvs::setSemiGlobalMatching(bool semiGlobalMatching)
{
    mSemiGlobalMatching = semiGlobalMatching;
}

void Smvs::setSurfaceSmoothingFactor(double surfaceSmoothingFactor)
{
    mSurfaceSmoothingFactor = surfaceSmoothingFactor;
}

void Smvs::reset()
{
    mInputImageScale = smvs_default_input_image_scale;
    mOutputDepthScale = smvs_default_output_depth_scale;
    mShadingBasedOptimization = smvs_default_shading_based_optimization;
    mSemiGlobalMatching = smvs_default_semi_global_matching;
    mSurfaceSmoothingFactor = smvs_default_surface_smoothing_factor;
}

auto Smvs::name() const -> QString
{
    return {"SMVS"};
}


/*----------------------------------------------------------------*/



SmvsDensifier::SmvsDensifier(const std::unordered_map<size_t, Image> &images,
                             const std::map<int, Camera> &cameras,
                             const std::unordered_map<size_t, CameraPose> &poses,
                             const std::vector<GroundPoint> &groundPoints,
                             const tl::Path &outputPath,
                             bool cuda,
                             bool autoSegmentation)
  : DensifierBase(images, cameras, poses, groundPoints, outputPath),
    mAutoSegmentation(autoSegmentation)
{
    DensifierBase::enableCuda(cuda);
    setUndistortImagesFormat(UndistortImages::Format::jpeg);
}

SmvsDensifier::~SmvsDensifier() = default;

void SmvsDensifier::clearPreviousModel() const
{
    outputPath().removeDirectory();
}

void SmvsDensifier::writeMveFile()
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

            for (const auto &pose : poses()) {

                size_t image_id = pose.first;
                size_t mve_id = mGraphosToMveIds.size();
                mGraphosToMveIds[image_id] = mve_id;

                const auto &image = images().at(image_id);
                int camera_id = image.cameraId();
                auto &camera = cameras().at(camera_id);

                Camera undistort_camera = undistort.at(camera_id).undistortCamera();
                double new_focal = undistort_camera.focal() / std::max(camera.width(), camera.height());
                double new_ppx = undistort_camera.calibration()->parameter(Calibration::Parameters::cx) / camera.width();
                double new_ppy = undistort_camera.calibration()->parameter(Calibration::Parameters::cy) / camera.height();

                auto projection_center = pose.second.position();
                auto rotation_matrix = pose.second.rotationMatrix();

                auto xyx = rotation_matrix * -projection_center.vector();

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
                    stream_ini << "name = " << image.name().toStdString() << std::endl;

                    stream_ini.close();
                }

                stream << new_focal << " " << "0" << " " << "0" << "\n";
                stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << "\n";
                stream << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << "\n";
                stream << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << "\n";
                stream << xyx[0] << " " << xyx[1] << " " << xyx[2] << std::endl;

            }

            for (auto &points_3d : groundPoints()) {

                stream << points_3d.x << " "
                    << points_3d.y << " "
                    << points_3d.z << "\n";

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

void SmvsDensifier::densify()
{
    try {

        tl::Path app_path = tl::App::instance().path().parentPath();
        app_path.append("smvsrecon_SSE41");

        std::string cmd("\"");
        cmd.append(app_path.toString());
        cmd.append("\" ");
        cmd.append("--scale=").append(std::to_string(Smvs::inputImageScale()));
        cmd.append(" --output-scale=").append(std::to_string(Smvs::outputDepthScale()));
        cmd.append(" --alpha=").append(std::to_string(Smvs::surfaceSmoothingFactor()));
        cmd.append(" --force ");
        if (!Smvs::semiGlobalMatching())
            cmd.append(" --no-sgm ");
        if (Smvs::shadingBasedOptimization())
            cmd.append(" --shading ");
        cmd.append("\"").append(outputPath().toString());

        tl::Process process(cmd);

        process.run();

        TL_ASSERT(process.status() == tl::Process::Status::finalized, "Densify Point Cloud error");

        std::string model_name("smvs-");
        if (Smvs::shadingBasedOptimization())
            model_name.append("S");
        else
            model_name.append("B");
        model_name.append(std::to_string(Smvs::inputImageScale())).append(".ply");

        tl::Path dense_model = outputPath();
        dense_model.append(model_name);

        TL_ASSERT(dense_model.exists(), "Densify Point Cloud error");

        setDenseModel(dense_model);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void SmvsDensifier::execute(tl::Progress *progressBar)
{

    try {

        this->clearPreviousModel();

        outputPath().createDirectories();

        tl::Path undistort_path(outputPath().parentPath());
        undistort_path.append("undistort");
        undistort_path.createDirectories();

        this->writeMveFile();

        if (status() == tl::Task::Status::stopping) return;

        this->undistort(QString::fromStdWString(undistort_path.toWString()));

        /// Copiar imagenes corregidas

        for (const auto &pose : poses()) {

            size_t image_id = pose.first;
            const auto &image = images().at(image_id);

            tl::Path undistort_image_path = undistort_path;
            undistort_image_path.append(image.name().toStdWString());
            undistort_image_path.replaceExtension(".jpg");

            tl::Path undistort_smvs_path = outputPath();
            undistort_smvs_path.append("views");
            undistort_smvs_path.append(colmap::StringPrintf("view_%04d.mve", mGraphosToMveIds.at(image_id)));
            undistort_smvs_path.append("undistorted.jpg");

            tl::Path::copy(undistort_image_path, undistort_smvs_path);
        }

        if (status() == Status::stopping) return;

        this->densify();
        if (mAutoSegmentation) this->autoSegmentation();

        Ply ply(denseModel());
        mReport.points = static_cast<int>(ply.size());
        ply.close();
        mReport.cuda = isCudaEnabled();
        mReport.method = this->name().toStdString();
        mReport.time = this->time();

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("MVS error");
    }

}

} // namespace graphos
