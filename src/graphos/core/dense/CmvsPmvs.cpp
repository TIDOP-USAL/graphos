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

#include "graphos/core/dense/CmvsPmvs.h"

#include "graphos/core/camera/Calibration.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/sfm/orientationexport.h"
#include "graphos/core/ply.h"

/* TidopLib */ 

#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/core/app.h>
#include <tidop/img/imgreader.h>
#include <tidop/core/progress.h>

/* COLMAP */

#include <colmap/base/database.h>
#include <colmap/util/string.h>

#include <fstream>
#include <set>

using namespace tl;

namespace graphos
{

/* Default parameters */

constexpr auto pmvs_default_use_visibility_information = true;
constexpr auto pmvs_defaultm_images_per_cluster = 100;
constexpr auto pmvs_default_level = 1;
constexpr auto pmvs_default_cell_size = 2;
constexpr auto pmvs_default_threshold = 0.7;
constexpr auto pmvs_default_window_size = 7;
constexpr auto pmvs_default_minimun_image_number = 3;



CmvsPmvs::CmvsPmvs()
  : Densification(Method::cmvs_pmvs),
    mUseVisibilityInformation(pmvs_default_use_visibility_information),
    mImagesPerCluster(pmvs_defaultm_images_per_cluster),
    mLevel(pmvs_default_level),
    mCellSize(pmvs_default_cell_size),
    mThreshold(pmvs_default_threshold),
    mWindowSize(pmvs_default_window_size),
    mMinimunImageNumber(pmvs_default_minimun_image_number)
{
}

CmvsPmvs::CmvsPmvs(bool useVisibilityInformation,
                                       int imagesPerCluster,
                                       int level,
                                       int cellSize,
                                       double threshold,
                                       int windowSize,
                                       int minimunImageNumber)
    : Densification(Method::cmvs_pmvs),
      mUseVisibilityInformation(useVisibilityInformation),
      mImagesPerCluster(imagesPerCluster),
      mLevel(level),
      mCellSize(cellSize),
      mThreshold(threshold),
      mWindowSize(windowSize),
      mMinimunImageNumber(minimunImageNumber)
{
}

CmvsPmvs::CmvsPmvs(const CmvsPmvs &cmvsPmvs)
  : Densification(cmvsPmvs),
    mUseVisibilityInformation(cmvsPmvs.mUseVisibilityInformation),
    mImagesPerCluster(cmvsPmvs.mImagesPerCluster),
    mLevel(cmvsPmvs.mLevel),
    mCellSize(cmvsPmvs.mCellSize),
    mThreshold(cmvsPmvs.mThreshold),
    mWindowSize(cmvsPmvs.mWindowSize),
    mMinimunImageNumber(cmvsPmvs.mMinimunImageNumber)
{
}

CmvsPmvs::CmvsPmvs(CmvsPmvs &&cmvsPmvs) noexcept
  : Densification(std::forward<Densification>(cmvsPmvs)),
    mUseVisibilityInformation(cmvsPmvs.mUseVisibilityInformation),
    mImagesPerCluster(cmvsPmvs.mImagesPerCluster),
    mLevel(cmvsPmvs.mLevel),
    mCellSize(cmvsPmvs.mCellSize),
    mThreshold(cmvsPmvs.mThreshold),
    mWindowSize(cmvsPmvs.mWindowSize),
    mMinimunImageNumber(cmvsPmvs.mMinimunImageNumber)
{
}

auto CmvsPmvs::operator =(const CmvsPmvs& cmvsPmvs) -> CmvsPmvs&
{
    if (this != &cmvsPmvs) {
        this->mUseVisibilityInformation = cmvsPmvs.mUseVisibilityInformation;
        this->mImagesPerCluster = cmvsPmvs.mImagesPerCluster;
        this->mLevel = cmvsPmvs.mLevel;
        this->mCellSize = cmvsPmvs.mCellSize;
        this->mThreshold = cmvsPmvs.mThreshold;
        this->mWindowSize = cmvsPmvs.mWindowSize;
        this->mMinimunImageNumber = cmvsPmvs.mMinimunImageNumber;
    }
    return *this;
}

auto CmvsPmvs::operator =(CmvsPmvs&& cmvsPmvs) noexcept -> CmvsPmvs&
{
    if (this != &cmvsPmvs) {
        this->mUseVisibilityInformation = cmvsPmvs.mUseVisibilityInformation;
        this->mImagesPerCluster = cmvsPmvs.mImagesPerCluster;
        this->mLevel = cmvsPmvs.mLevel;
        this->mCellSize = cmvsPmvs.mCellSize;
        this->mThreshold = cmvsPmvs.mThreshold;
        this->mWindowSize = cmvsPmvs.mWindowSize;
        this->mMinimunImageNumber = cmvsPmvs.mMinimunImageNumber;
    }
    return *this;
}

auto CmvsPmvs::useVisibilityInformation() const -> bool
{
    return mUseVisibilityInformation;
}

auto CmvsPmvs::imagesPerCluster() const -> int
{
    return mImagesPerCluster;
}

auto CmvsPmvs::level() const -> int
{
    return mLevel;
}

auto CmvsPmvs::cellSize() const -> int
{
    return mCellSize;
}

auto CmvsPmvs::threshold() const -> double
{
    return mThreshold;
}

auto CmvsPmvs::windowSize() const -> int
{
    return mWindowSize;
}

auto CmvsPmvs::minimunImageNumber() const -> int
{
    return mMinimunImageNumber;
}

void CmvsPmvs::setUseVisibilityInformation(bool useVisibilityInformation)
{
    mUseVisibilityInformation = useVisibilityInformation;
}

void CmvsPmvs::setImagesPerCluster(int imagesPerCluster)
{
    mImagesPerCluster = imagesPerCluster;
}

void CmvsPmvs::setLevel(int level)
{
    mLevel = level;
}

void CmvsPmvs::setCellSize(int cellSize)
{
    mCellSize = cellSize;
}

void CmvsPmvs::setThreshold(double threshold)
{
    mThreshold = threshold;
}

void CmvsPmvs::setWindowSize(int windowSize)
{
    mWindowSize = windowSize;
}

void CmvsPmvs::setMinimunImageNumber(int minimunImageNumber)
{
    mMinimunImageNumber = minimunImageNumber;
}

void CmvsPmvs::reset()
{
    mUseVisibilityInformation = pmvs_default_use_visibility_information;
    mImagesPerCluster = pmvs_defaultm_images_per_cluster;
    mLevel = pmvs_default_level;
    mCellSize = pmvs_default_cell_size;
    mThreshold = pmvs_default_threshold;
    mWindowSize = pmvs_default_window_size;
    mMinimunImageNumber = pmvs_default_minimun_image_number;
}


auto CmvsPmvs::name() const -> QString
{
    return {"CMVS/PMVS"};
}





CmvsPmvsDensifier::CmvsPmvsDensifier(const std::unordered_map<size_t, Image> &images,
                                     const std::map<int, Camera> &cameras,
                                     const std::unordered_map<size_t, CameraPose> &poses,
                                     const std::vector<GroundPoint> &groundPoints,
                                     const tl::Path &outputPath,
                                     tl::Path database,
                                     bool cuda,
                                     bool autoSegmentation)
  : DensifierBase(images, cameras, poses, groundPoints, outputPath),
    mDatabase(std::move(database)),
    mAutoSegmentation(autoSegmentation)
{
    DensifierBase::enableCuda(cuda);
    setUndistortImagesFormat(UndistortImages::Format::jpeg);
}

CmvsPmvsDensifier::~CmvsPmvsDensifier() = default;

void CmvsPmvsDensifier::clearPreviousModel() const
{
    outputPath().removeDirectory();
}

void CmvsPmvsDensifier::writeBundleFile()
{
    try {

        colmap::Database database;
        database.Open(mDatabase.toString());
        const auto &colmap_images = database.ReadAllImages();

        std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;

        for (const auto &image : images()) {

            Path image_path(image.second.path().toStdString());

            for (const auto &colmap_image : colmap_images) {
                Path colmap_image_path(colmap_image.Name());

                if (image_path.equivalent(colmap_image_path)) {
                    graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
                    break;
                }
            }

        }


        Path bundler_path(outputPath());
        bundler_path.append("bundle.rd.out");
        Path bundler_path_list(outputPath());
        bundler_path_list.append("bundle.rd.out.list.txt");

        std::map<int, Undistort> undistort;

        for (auto &camera : cameras()) {
            undistort[camera.first] = Undistort(camera.second);
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

                const auto &image = images().at(image_id);
                int camera_id = image.cameraId();
                //auto &camera = cameras().at(camera_id);

                Camera undistort_camera = undistort.at(camera_id).undistortCamera();
                double new_focal = undistort_camera.focal();

                auto projection_center = pose.second.position();
                auto rotation_matrix = pose.second.rotationMatrix();

                auto xyx = rotation_matrix * -projection_center.vector();

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

                stream << points_3d.x << " "
                       << points_3d.y << " "
                       << points_3d.z << "\n";

                stream << points_3d.color().red() << " "
                       << points_3d.color().green() << " "
                       << points_3d.color().blue() << "\n";

                auto &track = points_3d.track();

                stream << track.size();

                for (auto &map : track.pairs()) {

                    size_t image_id = map.first;
                    size_t point_id = map.second;

                    auto keypoints = database.ReadKeypoints(graphos_to_colmap_image_ids.at(image_id));
                    auto _undistort = undistort.at(images().at(image_id).cameraId());

                    Camera undistort_camera = _undistort.undistortCamera();
                    float ppx = static_cast<float>(undistort_camera.calibration()->parameter(Calibration::Parameters::cx));
                    float ppy = static_cast<float>(undistort_camera.calibration()->parameter(Calibration::Parameters::cy));

                    Point<float> undistort_point = _undistort.undistortPoint(Point<float>(keypoints[point_id].x, keypoints[point_id].y));
                    stream << " " << static_cast<int>(mGraphosToBundlerIds.at(image_id))
                        << " " << point_id
                        << " " << undistort_point.x - ppx
                        << " " << ppy - undistort_point.y << " ";

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

void CmvsPmvsDensifier::writeVisibility()
{

    try {

        ///////////////////////////////////////////////////////////////////////////////
        /// TODO: Repetido
        colmap::Database database;
        database.Open(mDatabase.toString());
        const auto &colmap_images = database.ReadAllImages();

        std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;

        for (const auto &image : images()) {

            Path image_path(image.second.path().toStdString());

            for (const auto &colmap_image : colmap_images) {
                Path colmap_image_path(colmap_image.Name());

                if (image_path.equivalent(colmap_image_path)) {
                    graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
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

void CmvsPmvsDensifier::writeOptions() const
{
    try {

        ///// options

        Path options_path(outputPath());
        options_path.append("option-all");
        std::ofstream file_options(options_path.toString(), std::ios::trunc);
        TL_ASSERT(file_options.is_open(), "Can't open file")

            TL_TODO("Si hay muchas imagenes separar en clusters pero con solape ya que los generados por cmvs dejan huecos al fusionar");

        file_options << "# Generated by Graphos - all images, no clustering.\n";

        file_options << "level " << CmvsPmvs::level() << "\n";
        file_options << "csize " << CmvsPmvs::cellSize() << "\n";
        file_options << "threshold " << CmvsPmvs::threshold() << "\n";
        file_options << "wsize " << CmvsPmvs::windowSize() << "\n";
        file_options << "minImageNum " << CmvsPmvs::minimunImageNumber() << "\n";
        file_options << "CPU " << std::thread::hardware_concurrency() << "\n";
        file_options << "setEdge 0" << "\n";
        file_options << "useBound 0" << "\n";
        file_options << "useVisData " << (CmvsPmvs::useVisibilityInformation() ? 1 : 0) << "\n";
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

void CmvsPmvsDensifier::densify()
{

    try {

        Path app_path = tl::App::instance().path().parentPath();
        app_path.append("pmvs2");

        std::string cmd_cmvs("\"");
        cmd_cmvs.append(app_path.toString());
        cmd_cmvs.append("\" \"");
        cmd_cmvs.append(outputPath().toString());
        cmd_cmvs.append("/\" option-all");

        Message::info("Process: {}", cmd_cmvs);
        Process process(cmd_cmvs);
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


void CmvsPmvsDensifier::execute(Progress *progressBar)
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

        Path undistort_path(outputPath().parentPath());
        undistort_path.append("undistort");
        undistort_path.createDirectories();
        this->undistort(QString::fromStdWString(undistort_path.toWString()));

        /// Copiar imagenes corregidas

        for (const auto &pose : poses()) {

            size_t image_id = pose.first;
            const auto &image = images().at(image_id);

            Path undistort_image_path = undistort_path;
            undistort_image_path.append(image.name().toStdWString());
            undistort_image_path.replaceExtension(".jpg");

            Path undistort_pmvs_path = outputPath();
            undistort_pmvs_path.append("visualize");
            undistort_pmvs_path.append(colmap::StringPrintf("%08d.jpg", mGraphosToBundlerIds.at(image_id)));

            Path::copy(undistort_image_path, undistort_pmvs_path);
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

        Message::success("Densification finished in {:.2} minutes", mReport.time / 60.);

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("MVS error");
    }
}


} // namespace graphos
