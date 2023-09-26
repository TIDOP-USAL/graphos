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

#include "graphos/core/dense/mvs.h"

#include "graphos/core/camera/Calibration.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/groundpoint.h"

/* TidopLib */
#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/core/app.h>
#include <tidop/img/imgreader.h>
#include <tidop/core/progress.h>
#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/core/chrono.h>

/* Colmap */
#include <colmap/base/database.h>

#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace graphos
{

constexpr auto mvsDefaultResolutionLevel = 1;
constexpr auto mvsDefaultMinResolution = 640;
constexpr auto mvsDefaultMaxResolution = 3200;
constexpr auto mvsDefaultNumberViews = 5;
constexpr auto mvsDefaultNumberViewsFuse = 3;

MvsProperties::MvsProperties()
  : mResolutionLevel(mvsDefaultResolutionLevel),
    mMinResolution(mvsDefaultMinResolution),
    mMaxResolution(mvsDefaultMaxResolution),
    mNumberViews(mvsDefaultNumberViews),
    mNumberViewsFuse(mvsDefaultNumberViewsFuse)
{
}

MvsProperties::MvsProperties(int resolutionLevel,
                             int minResolution,
                             int maxResolution,
                             int numberViews,
                             int numberViewsFuse)
  : mResolutionLevel(resolutionLevel),
    mMinResolution(minResolution),
    mMaxResolution(maxResolution),
    mNumberViews(numberViews),
    mNumberViewsFuse(numberViewsFuse)
{
}

MvsProperties::MvsProperties(const MvsProperties &mvs)
  : Mvs(mvs),
    mResolutionLevel(mvs.mResolutionLevel),
    mMinResolution(mvs.mMinResolution),
    mMaxResolution(mvs.mMaxResolution),
    mNumberViews(mvs.mNumberViews),
    mNumberViewsFuse(mvs.mNumberViewsFuse)
{
}

MvsProperties::MvsProperties(MvsProperties &&mvs) noexcept
  : Mvs(std::forward<Mvs>(mvs)),
    mResolutionLevel(mvs.mResolutionLevel),
    mMinResolution(mvs.mMinResolution),
    mMaxResolution(mvs.mMaxResolution),
    mNumberViews(mvs.mNumberViews),
    mNumberViewsFuse(mvs.mNumberViewsFuse)
{
}

MvsProperties &MvsProperties::operator =(const MvsProperties &mvs)
{
    if (this != &mvs) {
        mResolutionLevel = mvs.mResolutionLevel;
        mMinResolution = mvs.mMinResolution;
        mMaxResolution = mvs.mMaxResolution;
        mNumberViews = mvs.mNumberViews;
        mNumberViewsFuse = mvs.mNumberViewsFuse;
    }

    return *this;
}

MvsProperties &MvsProperties::operator =(MvsProperties &&mvs) noexcept
{
    if (this != &mvs) {
        mResolutionLevel = mvs.mResolutionLevel;
        mMinResolution = mvs.mMinResolution;
        mMaxResolution = mvs.mMaxResolution;
        mNumberViews = mvs.mNumberViews;
        mNumberViewsFuse = mvs.mNumberViewsFuse;
    }

    return *this;
}

int MvsProperties::resolutionLevel() const
{
    return mResolutionLevel;
}

int MvsProperties::minResolution() const
{
    return mMinResolution;
}

int MvsProperties::maxResolution() const
{
    return mMaxResolution;
}

int MvsProperties::numberViews() const
{
    return mNumberViews;
}

int MvsProperties::numberViewsFuse() const
{
    return mNumberViewsFuse;
}

void MvsProperties::setResolutionLevel(int resolutionLevel)
{
    mResolutionLevel = resolutionLevel;
}

void MvsProperties::setMinResolution(int minResolution)
{
    mMinResolution = minResolution;
}

void MvsProperties::setMaxResolution(int maxResolution)
{
    mMaxResolution = maxResolution;
}

void MvsProperties::setNumberViews(int numberViews)
{
    mNumberViews = numberViews;
}

void MvsProperties::setNumberViewsFuse(int numberViewsFuse)
{
    mNumberViewsFuse = numberViewsFuse;
}

void MvsProperties::reset()
{
    mResolutionLevel = mvsDefaultResolutionLevel;
    mMinResolution = mvsDefaultMinResolution;
    mMaxResolution = mvsDefaultMaxResolution;
    mNumberViews = mvsDefaultNumberViews;
    mNumberViewsFuse = mvsDefaultNumberViewsFuse;
}


QString MvsProperties::name() const
{
    return QString("MVS");
}




/*----------------------------------------------------------------*/



MvsDensifier::MvsDensifier(const std::unordered_map<size_t, Image> &images,
                           const std::map<int, Camera> &cameras,
                           const std::unordered_map<size_t, CameraPose> &poses,
                           const std::vector<GroundPoint> &groundPoints,
                           const tl::Path &outputPath,
                           const tl::Path &database,
                           bool cuda,
                           bool autoSegmentation)
  : DensifierBase(images, cameras, poses, groundPoints, outputPath),
    mDatabase(database),
    mAutoSegmentation(autoSegmentation)
{
    enableCuda(cuda);
    setUndistortImagesFormat(UndistortImages::Format::tiff);
}

MvsDensifier::~MvsDensifier()
{
}

void MvsDensifier::clearTemporalFiles()
{
    outputPath().append("temp").removeDirectory();
}

void MvsDensifier::exportToColmap()
{
    try {

        colmap::Database database;
        database.Open(mDatabase.toString());
        const auto &colmap_images = database.ReadAllImages();

        std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;
        std::unordered_map<size_t, colmap::FeatureKeypoints> keypoints;

        for (const auto &image : images()) {

            tl::Path image_path(image.second.path().toStdString());

            for (const auto &colmap_image : colmap_images) {
                tl::Path colmap_image_path(colmap_image.Name());

                if (image_path.equivalent(colmap_image_path)) {
                    graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
                    keypoints[colmap_image.ImageId()] = database.ReadKeypoints(colmap_image.ImageId());
                    break;
                }
            }

        }
        database.Close();

        std::map<int, Undistort> undistort_map;

        for (auto &camera : cameras()) {
            undistort_map[camera.first] = Undistort(camera.second);
        }

        tl::Path colmap_sparse_path = outputPath();
        colmap_sparse_path.append("temp");
        colmap_sparse_path.append("export");
        colmap_sparse_path.append("sparse");
        colmap_sparse_path.createDirectories();

        //  TL_TODO("Extraer la exportación a colmap")
        // cameras.txt
        {
            tl::Path colmap_cameras(colmap_sparse_path);
            colmap_cameras.append("cameras.txt");

            std::ofstream ofs;
            ofs.open(colmap_cameras.toString(), std::ofstream::out | std::ofstream::trunc);

            TL_ASSERT(ofs.is_open(), "Open fail: cameras.txt");

            ofs << "# Camera list with one line of data per camera: \n";
            ofs << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n";
            ofs << "# Number of cameras: " << cameras().size() << "\n";

            ofs << std::fixed << std::setprecision(12);

            for (auto &undistort_pair : undistort_map) {

                int camera_id = undistort_pair.first;
                Undistort undistort = undistort_pair.second;
                Camera undistort_camera = undistort.undistortCamera();
                auto calibration = undistort_camera.calibration();

                TL_ASSERT(calibration, "Camera calibration not found");

                /// La cámara tiene que ser PINHOLE para InterfaceCOLMAP
                ofs << camera_id << " PINHOLE " << undistort_camera.width() << " " << undistort_camera.height() << " ";

                double focal_x = 0.0;
                double focal_y = 0.0;

                if (calibration->existParameter(Calibration::Parameters::focal)) {
                    focal_x = focal_y = calibration->parameter(Calibration::Parameters::focal);
                } else {
                    focal_x = calibration->parameter(Calibration::Parameters::focalx);
                    focal_y = calibration->parameter(Calibration::Parameters::focaly);
                }

                ofs << focal_x << " " << focal_y << " ";

                double cx = calibration->existParameter(Calibration::Parameters::cx) ?
                            calibration->parameter(Calibration::Parameters::cx) :
                            undistort_camera.width() / 2.;
                double cy = calibration->existParameter(Calibration::Parameters::cy) ?
                            calibration->parameter(Calibration::Parameters::cy) :
                            undistort_camera.height() / 2.;

                ofs << cx << " " << cy << std::endl;

            }

        }

        // images.txt
        {
            tl::Path colmap_images(colmap_sparse_path);
            colmap_images.append("images.txt");

            std::ofstream ofs;
            ofs.open(colmap_images.toString(), std::ofstream::out | std::ofstream::trunc);

            TL_ASSERT(ofs.is_open(), "Open fail: images.txt");

            ofs << std::fixed << std::setprecision(12);

            for (const auto &pose : poses()) {

                size_t image_id = pose.first;
                auto &image_pose = pose.second;

                const auto &image = images().at(image_id);

                auto projection_center = image_pose.position();
                auto rotation_matrix = image_pose.rotationMatrix();

                tl::RotationMatrix<double> transform_to_colmap = tl::RotationMatrix<double>::identity();
                transform_to_colmap.at(1, 1) = -1;
                transform_to_colmap.at(2, 2) = -1;

                tl::Quaternion<double> quaternion = pose.second.quaternion();;

                auto xyx = rotation_matrix * -projection_center.vector();
                tl::Path image_path = image.path().toStdString();
                image_path.replaceExtension(".tif");

                auto colmap_image_id = graphos_to_colmap_image_ids[image_id];

                ofs << colmap_image_id << " " << quaternion.w << " " << quaternion.x << " " << quaternion.y << " " << quaternion.z << " "
                    << xyx[0] << " " << xyx[1] << " " << xyx[2] << " " << image.cameraId() << " " << image_path.fileName().toString() << std::endl;

                for (size_t i = 0; i < groundPoints().size(); i++) {

                    auto &track = groundPoints()[i].track();

                    if (track.existPair(image_id)) {

                        for (auto &map : track.pairs()) {

                            if (map.first == image_id) {

                                size_t point_id = map.second;
                                ofs << keypoints[colmap_image_id][point_id].x << " " << keypoints[colmap_image_id][point_id].y << " " << i + 1 << " ";

                            }

                        }
                    }
                }

                ofs << std::endl;

            }

        }

        // points3D.txt
        {
            tl::Path colmap_points_3d(colmap_sparse_path);
            colmap_points_3d.append("points3D.txt");

            std::ofstream ofs;
            ofs.open(colmap_points_3d.toString(), std::ofstream::out | std::ofstream::trunc);

            TL_ASSERT(ofs.is_open(), "Open fail: points3D.txt");

            ofs << std::fixed << std::setprecision(12);

            ///#   POINT3D_ID, X, Y, Z, R, G, B, ERROR, TRACK[] as (IMAGE_ID, POINT2D_IDX) 
            size_t point_id = 0;
            for (auto &points_3d : groundPoints()) {

                ofs << ++point_id << " "
                    << points_3d.x << " "
                    << points_3d.y << " "
                    << points_3d.z << " "
                    << points_3d.color().red() << " "
                    << points_3d.color().green() << " "
                    << points_3d.color().blue() << " 0";

                auto &track = points_3d.track();

                for (auto &map : track.pairs()) {
                    ofs << " " << graphos_to_colmap_image_ids.at(map.first) << " " << map.second;
                }

                ofs << std::endl;

            }

            ofs.close();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Densification error");
    }
}

void MvsDensifier::writeNVMFile()
{
    try {

        colmap::Database database;
        database.Open(mDatabase.toString());
        const auto &colmap_images = database.ReadAllImages();

        std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;
        std::unordered_map<size_t, colmap::FeatureKeypoints> keypoints;

        for (const auto &image : images()) {

            tl::Path image_path(image.second.path().toStdString());

            for (const auto &colmap_image : colmap_images) {
                tl::Path colmap_image_path(colmap_image.Name());

                if (image_path.equivalent(colmap_image_path)) {
                    graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
                    keypoints[colmap_image.ImageId()] = database.ReadKeypoints(colmap_image.ImageId());
                    break;
                }
            }

        }

        database.Close();

        tl::Path nvm_path(outputPath());
        nvm_path.append("model.nvm");

        std::map<int, Undistort> undistort;

        for (auto &camera : cameras()) {
            undistort[camera.first] = Undistort(camera.second);
        }

        std::ofstream stream(nvm_path.toString(), std::ios::trunc);

        if (stream.is_open()) {

            stream << "NVM_V3 \n\n" << poses().size() << "\n";
            stream << std::fixed << std::setprecision(12);

            std::unordered_map<size_t, size_t> graphos_to_mvs_ids;

            for (const auto &pose : poses()) {

                size_t image_id = pose.first;
                size_t mvs_id = graphos_to_mvs_ids.size();
                graphos_to_mvs_ids[image_id] = mvs_id;

                const auto &image = images().at(image_id);
                int camera_id = image.cameraId();
                double new_focal = undistort.at(camera_id).undistortCamera().focal();

                auto projection_center = pose.second.position();
                auto quaternion = pose.second.quaternion();
                auto rotation_matrix = pose.second.rotationMatrix();
                auto xyx = rotation_matrix * -projection_center.vector();

                tl::Path undistort_image(image.path().toStdString());
                undistort_image.replaceExtension(".tif");

                stream << "undistort/" << undistort_image.fileName().toString() << " ";
                stream << new_focal << " ";
                stream << quaternion.w << " ";
                stream << quaternion.x << " ";
                stream << quaternion.y << " ";
                stream << quaternion.z << " ";
                stream << xyx[0] << " ";
                stream << xyx[1] << " ";
                stream << xyx[2] << " ";
                stream << 0 << " ";
                stream << 0 << std::endl;

            }

            stream << "\n" << groundPoints().size() << std::endl;

            for (auto &points_3d : groundPoints()) {

                stream << points_3d.x << " "
                       << points_3d.y << " "
                       << points_3d.z << " "
                       << points_3d.color().red() << " "
                       << points_3d.color().green() << " "
                       << points_3d.color().blue() << " ";

                auto &track = points_3d.track();

                stream << track.size();

                for (auto &map : track.pairs()) {

                    size_t image_id = map.first;
                    size_t point_id = map.second;

                    auto &keypoint = keypoints[graphos_to_colmap_image_ids.at(image_id)][point_id];
                    auto &_undistort = undistort.at(images().at(image_id).cameraId());

                    tl::Point<float> undistort_point = _undistort.undistortPoint(tl::Point<float>(keypoint.x, keypoint.y));

                    stream << " " << static_cast<int>(graphos_to_mvs_ids.at(image_id))
                           << " " << point_id
                           << " " << undistort_point.x
                           << " " << undistort_point.y;
                }

                stream << std::endl;
            }

            stream.close();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Densification error");
    }

}

void MvsDensifier::exportToMVS()
{
    try {

        tl::Path app_path = tl::App::instance().path();
        std::string cmd_mvs("\"");
        cmd_mvs.append(app_path.parentPath().toString());
        cmd_mvs.append("\\OpenMVS\\InterfaceCOLMAP\"");
        cmd_mvs.append(" -i \"").append(outputPath().toString()).append("\\temp\\export\"");
        cmd_mvs.append(" -o \"").append(outputPath().toString()).append("\\temp\\model.mvs\"");
        tl::Message::info("Process: {}", cmd_mvs);
        tl::Process process(cmd_mvs);

        process.run();

        TL_ASSERT(process.status() == tl::Process::Status::finalized, "MVS error");

        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));

        tl::Path model = outputPath();
        model.append("temp").append("model.mvs");

        TL_ASSERT(model.exists(), "model.mvs not found");

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void MvsDensifier::densify()
{
    try {

        tl::Path app_path = tl::App::instance().path();
        std::string cmd_mvs("\"");
        cmd_mvs.append(app_path.parentPath().toString());
        cmd_mvs.append("\\OpenMVS\\DensifyPointCloud\" -w \"");
        cmd_mvs.append(outputPath().toString());
        cmd_mvs.append("\\temp\" -i model.mvs -o model_dense.mvs -v 0");
        cmd_mvs.append(" --resolution-level ").append(std::to_string(MvsProperties::resolutionLevel()));
        cmd_mvs.append(" --min-resolution ").append(std::to_string(MvsProperties::minResolution()));
        cmd_mvs.append(" --max-resolution ").append(std::to_string(MvsProperties::maxResolution()));
        cmd_mvs.append(" --number-views ").append(std::to_string(MvsProperties::numberViews()));
        cmd_mvs.append(" --number-views-fuse ").append(std::to_string(MvsProperties::numberViewsFuse()));
        if (isCudaEnabled())
            cmd_mvs.append(" --cuda-device -1");
        else
            cmd_mvs.append(" --cuda-device -2");
        //cmd_mvs.append(" --filter-point-cloud 1");

        tl::Message::info("Process: {}", cmd_mvs);
        tl::Process process(cmd_mvs);

        process.run();

        TL_ASSERT(process.status() == tl::Process::Status::finalized, "Densify Point Cloud error");

        std::this_thread::sleep_for(std::chrono::nanoseconds(5000));

        tl::Path dense_model = outputPath();
        dense_model.append("temp");
        dense_model.append("model_dense.ply");

        TL_ASSERT(dense_model.exists(), "Densify Point Cloud error");

        auto path = outputPath();
        path.append(dense_model.fileName());

        if (path.exists()) tl::Path::removeFile(path);

        tl::Path::copy(dense_model, path);

        setDenseModel(path);

        tl::Message::info("Dense model write at: {}", path.toString());

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void MvsDensifier::execute(tl::Progress *progressBar)
{
    try {

        tl::Chrono chrono("Densification finished");
        chrono.run();

        //this->clearPreviousModel();

        tl::Path undistort_path(outputPath());
        undistort_path.append("temp").append("export").append("images");
        undistort_path.createDirectories();

        /// Da problemas al escribir el fichero nvm e importarlo
        /// Intentar exportar directamente. Por ahora exportar a Colmap  utilizar InterfaceColmap para convertirlo a OpenMVS
        this->exportToColmap();
        //this->writeNVMFile(); 

        if (status() == tl::Task::Status::stopping) return;

        this->undistort(QString::fromStdWString(undistort_path.toWString()));

        if (status() == tl::Task::Status::stopping) return;

        this->exportToMVS();
        this->densify();
        if (mAutoSegmentation) this->autoSegmentation();

        this->clearTemporalFiles();

        chrono.stop();

        if (progressBar) (*progressBar)();

    } catch (...) {
        //this->clearTemporalFiles();
        TL_THROW_EXCEPTION_WITH_NESTED("MVS error");
    }


}

} // namespace graphos
