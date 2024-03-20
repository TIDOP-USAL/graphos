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


#include "OrientationCommand.h"

#include "graphos/core/project.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/sfm/orientation.h"
#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/sfm/posesio.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/rotation_convert.h>

#include <colmap/base/reconstruction.h>
#include <colmap/util/option_manager.h>
#include <colmap/base/database_cache.h>
#include <colmap/controllers/incremental_mapper.h>

#include <QFileInfo>

#include <fstream>

using namespace tl;

namespace graphos
{

namespace internal
{

class ImportPoses
{
public:

    ImportPoses(Project *project);
    ~ImportPoses();

    void run();

    void setFixCalibration(bool fixCalibration);
    void setFixPoses(bool fixPoses);

private:

    void computeOffset();

    void temporalReconstruction(const tl::Path &tempPath);
    void writeImages(const tl::Path &tempPath);
    void writeCameras(const tl::Path &tempPath);
    void writePoints(const tl::Path &tempPath);
    colmap::Reconstruction importReconstruction(const tl::Path &tempPath);
    colmap::DatabaseCache loadDatabase(colmap::Reconstruction &reconstruction,
                                       std::shared_ptr<colmap::IncrementalMapperOptions> mapper_options);
    void triangulation(colmap::Reconstruction &reconstruction,
                       colmap::IncrementalMapper &mapper,
                       const colmap::IncrementalTriangulator::Options &options);
    void bundleAdjustment(colmap::Reconstruction &reconstruction,
                          colmap::IncrementalMapper &mapper,
                          std::shared_ptr<colmap::IncrementalMapperOptions> mapper_options);

    bool isCoordinatesLocal() const;
    tl::Point3<double> offset();

private:

    Project *mProject;
    tl::Point3<double> mOffset;
    bool mFixCalibration;
    bool mFixPoses;
};

ImportPoses::ImportPoses(Project *project)
    : mProject(project)
{
    computeOffset();
}

ImportPoses::~ImportPoses()
{
}

void ImportPoses::run()
{
    tl::TemporalDir temp_dir;
    tl::Path temp_path = temp_dir.path();

    temporalReconstruction(temp_path);

    colmap::Reconstruction reconstruction = importReconstruction(temp_path);

    tl::Path reconstruction_path(mProject->projectFolder());
    reconstruction_path.append("sfm");
    reconstruction_path.createDirectories();

    bool clear_points = false;

    std::string input_path = temp_path.toString();
    std::string recon_path = reconstruction_path.toString();
    colmap::OptionManager options;
    options.AddDatabaseOptions();
    options.AddImageOptions();
    options.AddRequiredOption("input_path", &input_path);
    options.AddRequiredOption("reconstruction_path", &recon_path);
    options.AddDefaultOption(
        "clear_points", &clear_points,
        "Whether to clear all existing points and observations");
    options.AddMapperOptions();

    if (!reconstruction_path.exists()) {
        throw std::runtime_error(std::string("ERROR: 'reconstruction_path' is not a directory"));
    }

    auto mapper_options = options.mapper;

    colmap::DatabaseCache database_cache = loadDatabase(reconstruction, mapper_options);

    std::cout << std::endl;

    TL_ASSERT(reconstruction.NumRegImages() >= 2, "Need at least two images for triangulation");

    colmap::IncrementalMapper mapper(&database_cache);
    mapper.BeginReconstruction(&reconstruction);

    this->triangulation(reconstruction, mapper, mapper_options->Triangulation());

    tl::Message::info("Retriangulation");

    CompleteAndMergeTracks(*mapper_options, &mapper);

    bundleAdjustment(reconstruction, mapper, mapper_options);

    tl::Message::info("Extracting colors");
    reconstruction.ExtractColorsForAllImages("");

    const bool kDiscardReconstruction = false;
    mapper.EndReconstruction(kDiscardReconstruction);

    reconstruction.Write(reconstruction_path.toString());
    reconstruction.WriteText(reconstruction_path.toString());
    reconstruction.ExportPLY(tl::Path(reconstruction_path).append("sparse.ply").toString());

    std::ofstream stream(tl::Path(reconstruction_path).append("offset.txt").toString(), std::ios::trunc);
    if (stream.is_open()) {
        stream << QString::number(mOffset.x, 'f', 6).toStdString() << " "
            << QString::number(mOffset.y, 'f', 6).toStdString() << " "
            << QString::number(mOffset.z, 'f', 6).toStdString() << std::endl;

        tl::Message::info("Camera offset: {},{},{}", mOffset.x, mOffset.y, mOffset.z);

        stream.close();
    }
}

void ImportPoses::setFixCalibration(bool fixCalibration)
{
    mFixCalibration = fixCalibration;
}

void ImportPoses::setFixPoses(bool fixPoses)
{
    mFixPoses = fixPoses;
}

void ImportPoses::computeOffset()
{
    int i = 1;
    for (const auto &image : mProject->images()) {
        CameraPose camera_pose = image.second.cameraPose();
        if (camera_pose.isEmpty()) continue;
        mOffset += (camera_pose.position() - mOffset) / i;
        i++;
    }
}

void ImportPoses::temporalReconstruction(const tl::Path &tempPath)
{
    writeImages(tempPath);
    writeCameras(tempPath);
    writePoints(tempPath);
}

void ImportPoses::writeImages(const tl::Path &tempPath)
{
    tl::Path images_path(tempPath);
    images_path.append("images.txt");

    std::ofstream ofs;
    ofs.open(images_path.toString(), std::ofstream::out | std::ofstream::trunc);

    if (!ofs.is_open()) throw std::runtime_error(std::string("Open fail: images.txt"));

    size_t id = 1;

    for (const auto &image : mProject->images()) {

        CameraPose camera_pose = image.second.cameraPose();
        if (camera_pose.isEmpty()) {
            id++;
            continue; /// Se saltan las imagenes no orientadas
        }

        tl::Quaternion<double> quaternion = camera_pose.quaternion();
        std::string file_name = image.second.path().toStdString();
        tl::Point3<double> position = camera_pose.position();

        if (!isCoordinatesLocal()) {
            position -= mOffset;
        }

        tl::Vector<double, 3> vector_camera_position = position.vector();

        tl::RotationMatrix<double> r_ip_ic = tl::RotationMatrix<double>::identity();
        r_ip_ic.at(1, 1) = -1;
        r_ip_ic.at(2, 2) = -1;

        tl::RotationMatrix<double> rotation_matrix;
        tl::RotationConverter<double>::convert(quaternion, rotation_matrix);

        tl::RotationMatrix<double> rotation = r_ip_ic * rotation_matrix.transpose();
        tl::RotationConverter<double>::convert(rotation, quaternion);
        quaternion.normalize();

        vector_camera_position = rotation * -vector_camera_position;

        ofs << std::fixed << id++ << " " << QString::number(quaternion.w, 'g', 10).toStdString() << " "
            << QString::number(quaternion.x, 'g', 10).toStdString() << " "
            << QString::number(quaternion.y, 'g', 10).toStdString() << " "
            << QString::number(quaternion.z, 'g', 10).toStdString() << " "
            << QString::number(vector_camera_position[0], 'g', 10).toStdString() << " "
            << QString::number(vector_camera_position[1], 'g', 10).toStdString() << " "
            << QString::number(vector_camera_position[2], 'g', 10).toStdString() << " 1 " << file_name << std::endl;
        ofs << std::endl;

    }

    ofs.close();
}


void ImportPoses::writeCameras(const tl::Path &tempPath)
{
    tl::Path cameras_path(tempPath);
    cameras_path.append("cameras.txt");

    std::ofstream ofs;
    ofs.open(cameras_path.toString(), std::ofstream::out | std::ofstream::trunc);

    if (!ofs.is_open()) throw std::runtime_error(std::string("Open fail: cameras.txt"));

    ofs << "# Camera list with one line of data per camera: \n";
    ofs << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n";
    ofs << "# Number of cameras: " << mProject->camerasCount() << "\n";

    for (const auto &_camera : mProject->cameras()) {

        size_t camera_id = _camera.first;
        Camera camera = _camera.second;

        std::string camera_type = cameraToColmapType(camera).toStdString();
        std::shared_ptr<Calibration> calibration = camera.calibration();

        if (calibration) {

            ofs << camera_id << " " << camera_type << " " << camera.width() << " " << camera.height() << " ";

            if (camera_type == "SIMPLE_RADIAL" ||
                camera_type == "RADIAL" ||
                camera_type == "FULL_RADIAL" ||
                camera_type == "SIMPLE_RADIAL_FISHEYE" ||
                camera_type == "RADIAL_FISHEYE") {
                double focal = calibration->existParameter(Calibration::Parameters::focal) ?
                    calibration->parameter(Calibration::Parameters::focal) :
                    std::min(camera.width(), camera.height());
                ofs << QString::number(focal, 'g', 10).toStdString() << " ";
            } else {
                double focal_x = calibration->existParameter(Calibration::Parameters::focalx) ?
                    calibration->parameter(Calibration::Parameters::focalx) :
                    std::min(camera.width(), camera.height());
                double focal_y = calibration->existParameter(Calibration::Parameters::focaly) ?
                    calibration->parameter(Calibration::Parameters::focaly) :
                    std::min(camera.width(), camera.height());
                ofs << QString::number(focal_x, 'g', 10).toStdString() << " " << QString::number(focal_y, 'g', 10).toStdString() << " ";
            }

            double cx = calibration->existParameter(Calibration::Parameters::cx) ?
                calibration->parameter(Calibration::Parameters::cx) :
                camera.width() / 2.;
            double cy = calibration->existParameter(Calibration::Parameters::cy) ?
                calibration->parameter(Calibration::Parameters::cy) :
                camera.height() / 2.;

            ofs << QString::number(cx, 'g', 10).toStdString() << " "
                << QString::number(cy, 'g', 10).toStdString();

            if (camera_type == "SIMPLE_RADIAL" ||
                camera_type == "RADIAL" ||
                camera_type == "FULL_RADIAL" ||
                camera_type == "OPENCV" ||
                camera_type == "OPENCV_FISHEYE" ||
                camera_type == "FULL_OPENCV" ||
                camera_type == "SIMPLE_RADIAL_FISHEYE" ||
                camera_type == "RADIAL_FISHEYE" ||
                camera_type == "THIN_PRISM_FISHEYE") {
                double k1 = calibration->existParameter(Calibration::Parameters::k1) ?
                    calibration->parameter(Calibration::Parameters::k1) : 0.0;
                ofs << " " << QString::number(k1, 'g', 10).toStdString();
            }

            if (camera_type == "RADIAL" ||
                camera_type == "FULL_RADIAL" ||
                camera_type == "OPENCV" ||
                camera_type == "OPENCV_FISHEYE" ||
                camera_type == "FULL_OPENCV" ||
                camera_type == "RADIAL_FISHEYE" ||
                camera_type == "THIN_PRISM_FISHEYE") {
                double k2 = calibration->existParameter(Calibration::Parameters::k2) ?
                    calibration->parameter(Calibration::Parameters::k2) : 0.0;
                ofs << " " << QString::number(k2, 'g', 10).toStdString();
            }

            if (camera_type == "OPENCV" ||
                camera_type == "FULL_OPENCV" ||
                camera_type == "THIN_PRISM_FISHEYE" ||
                camera_type == "FULL_RADIAL") {
                double p1 = calibration->existParameter(Calibration::Parameters::p1) ?
                    calibration->parameter(Calibration::Parameters::p1) : 0.0;
                double p2 = calibration->existParameter(Calibration::Parameters::p2) ?
                    calibration->parameter(Calibration::Parameters::p2) : 0.0;
                ofs << " " << QString::number(p1, 'g', 10).toStdString()
                    << " " << QString::number(p2, 'g', 10).toStdString();
            }

            if (camera_type == "OPENCV_FISHEYE" ||
                camera_type == "FULL_OPENCV" ||
                camera_type == "THIN_PRISM_FISHEYE" ||
                camera_type == "FULL_RADIAL") {
                double k3 = calibration->existParameter(Calibration::Parameters::k3) ?
                    calibration->parameter(Calibration::Parameters::k3) : 0.0;
                ofs << " " << QString::number(k3, 'g', 10).toStdString();
            }

            if (camera_type == "OPENCV_FISHEYE" ||
                camera_type == "FULL_OPENCV" ||
                camera_type == "THIN_PRISM_FISHEYE") {
                double k4 = calibration->existParameter(Calibration::Parameters::k4) ?
                    calibration->parameter(Calibration::Parameters::k4) : 0.0;
                ofs << " " << QString::number(k4, 'g', 10).toStdString();
            }

            if (camera_type == "FULL_OPENCV") {
                double k5 = calibration->existParameter(Calibration::Parameters::k5) ?
                    calibration->parameter(Calibration::Parameters::k5) : 0.0;
                double k6 = calibration->existParameter(Calibration::Parameters::k6) ?
                    calibration->parameter(Calibration::Parameters::k6) : 0.0;
                ofs << " " << QString::number(k5, 'g', 10).toStdString()
                    << " " << QString::number(k6, 'g', 10).toStdString();
            }

        } else {

            double focal = std::min(camera.width(), camera.height());
            double cx = camera.width() / 2.;
            double cy = camera.height() / 2.;

            ofs << camera_id << " " << camera_type << " " << camera.width() << " " << camera.height() << " ";

            if (camera_type == "SIMPLE_RADIAL" ||
                camera_type == "RADIAL" ||
                camera_type == "FULL_RADIAL" ||
                camera_type == "SIMPLE_RADIAL_FISHEYE" ||
                camera_type == "RADIAL_FISHEYE") {
                ofs << focal << " ";
            } else {
                ofs << focal << " " << focal << " ";
            }

            ofs << cx << " " << cy;

            if (camera_type == "SIMPLE_RADIAL")
                ofs << " 0.0";

            if (camera_type == "RADIAL")
                ofs << " 0.0 0.0";

            if (camera_type == "OPENCV")
                ofs << " 0.0 0.0 0.0 0.0";

            if (camera_type == "OPENCV_FISHEYE")
                ofs << " 0.0 0.0 0.0 0.0";

            if (camera_type == "FULL_OPENCV")
                ofs << " 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0";

            if (camera_type == "SIMPLE_RADIAL_FISHEYE")
                ofs << " 0.0";

            if (camera_type == "RADIAL_FISHEYE")
                ofs << " 0.0 0.0";

            if (camera_type == "THIN_PRISM_FISHEYE")
                ofs << " 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0";

            if (camera_type.compare("FULL_RADIAL") == 0)
                ofs << " 0.0 0.0 0.0 0.0 0.0 0.0";

        }

        ofs << std::endl;
    }

    ofs.close();
}

void ImportPoses::writePoints(const tl::Path &tempPath)
{
    tl::Path points3d_path(tempPath);
    points3d_path.append("points3D.txt");

    std::ofstream ofs;
    ofs.open(points3d_path.toString(), std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

colmap::Reconstruction ImportPoses::importReconstruction(const tl::Path &tempPath)
{
    colmap::Reconstruction reconstruction;
    reconstruction.Read(tempPath.toString());
    return reconstruction;
}

colmap::DatabaseCache ImportPoses::loadDatabase(colmap::Reconstruction &reconstruction,
                                                std::shared_ptr<colmap::IncrementalMapperOptions> mapper_options)
{
    colmap::DatabaseCache database_cache;

    tl::Chrono timer("Elapsed time:");
    timer.run();

    colmap::Database database(mProject->database().toString());

    size_t min_num_matches = static_cast<size_t>(mapper_options->min_num_matches);
    database_cache.Load(database, min_num_matches,
                        mapper_options->ignore_watermarks,
                        mapper_options->image_names);

    //if (clear_points) {
    //  reconstruction.DeleteAllPoints2DAndPoints3D();
    //  reconstruction.TranscribeImageIdsToDatabase(database);
    //}

    std::cout << std::endl;
    timer.stop();

    return database_cache;
}

void ImportPoses::triangulation(colmap::Reconstruction &reconstruction,
                                colmap::IncrementalMapper &mapper,
                                const colmap::IncrementalTriangulator::Options &options)
{

    const auto &reg_image_ids = reconstruction.RegImageIds();

    for (size_t i = 0; i < reg_image_ids.size(); ++i) {
        const colmap::image_t image_id = reg_image_ids[i];

        const auto &image = reconstruction.Image(image_id);

        const size_t num_existing_points3D = image.NumPoints3D();

        std::cout << "  => Image sees " << num_existing_points3D << " / "
            << image.NumObservations() << " points/n";

        mapper.TriangulateImage(options, image_id);

        std::cout << "  => Triangulated "
            << (image.NumPoints3D() - num_existing_points3D) << " points/n";
    }

}

void ImportPoses::bundleAdjustment(colmap::Reconstruction &reconstruction,
                                   colmap::IncrementalMapper &mapper,
                                   std::shared_ptr<colmap::IncrementalMapperOptions> mapper_options)
{
    auto ba_options = mapper_options->GlobalBundleAdjustment();

    ba_options.refine_focal_length = !mFixCalibration;
    ba_options.refine_principal_point = false;
    ba_options.refine_extra_params = !mFixCalibration;
    ba_options.refine_extrinsics = !mFixPoses;

    // Configure bundle adjustment.
    colmap::BundleAdjustmentConfig ba_config;
    for (const colmap::image_t image_id : reconstruction.RegImageIds()) {
        ba_config.AddImage(image_id);
    }

    for (int i = 0; i < mapper_options->ba_global_max_refinements; ++i) {
        // Avoid degeneracies in bundle adjustment.
        reconstruction.FilterObservationsWithNegativeDepth();

        const size_t num_observations = reconstruction.ComputeNumObservations();

        //PrintHeading1("Bundle adjustment");
        //graphos::BundleAdjuster bundle_adjuster(ba_options, ba_config);
        colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
        CHECK(bundle_adjuster.Solve(&reconstruction));

        size_t num_changed_observations = 0;
        num_changed_observations += CompleteAndMergeTracks(*mapper_options, &mapper);
        num_changed_observations += FilterPoints(*mapper_options, &mapper);
        const double changed =
            static_cast<double>(num_changed_observations) / num_observations;
        std::cout << colmap::StringPrintf("  => Changed observations: %.6f", changed)
            << std::endl;
        if (changed < mapper_options->ba_global_max_refinement_change) {
            break;
        }
    }

    // Se incluye el punto principal en el ajuste
    if (!mFixCalibration) {
        ba_options.refine_principal_point = true;
        for (int i = 0; i < mapper_options->ba_global_max_refinements; ++i) {
            // Avoid degeneracies in bundle adjustment.
            reconstruction.FilterObservationsWithNegativeDepth();

            const size_t num_observations = reconstruction.ComputeNumObservations();

            //PrintHeading1("Bundle adjustment");
            //graphos::BundleAdjuster bundle_adjuster(ba_options, ba_config);
            colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
            if (!bundle_adjuster.Solve(&reconstruction)) throw std::runtime_error(std::string("Reconstruction error"));

            size_t num_changed_observations = 0;
            num_changed_observations += CompleteAndMergeTracks(*mapper_options, &mapper);
            num_changed_observations += FilterPoints(*mapper_options, &mapper);
            const double changed =
                static_cast<double>(num_changed_observations) / num_observations;
            std::cout << colmap::StringPrintf("  => Changed observations: %.6f", changed)
                << std::endl;
            if (changed < mapper_options->ba_global_max_refinement_change) {
                break;
            }
        }
    }
}

bool ImportPoses::isCoordinatesLocal() const
{
    bool local = true;

    auto it_image = mProject->images().begin();
    if (it_image != mProject->images().end()) {
        CameraPose camera_pose = it_image->second.cameraPose();
        if (camera_pose.crs() != "") local = false;
    }

    return local;
}

tl::Point3<double> ImportPoses::offset()
{
    return mOffset;
}


}


bool rtkOrientations(const ProjectImp &project)
{
    bool bRtkOrientations = false;

    auto it = project.images().begin();
    CameraPose camera_pose = it->second.cameraPose();
    if (!camera_pose.isEmpty()) {
        tl::Quaternion<double> q = camera_pose.quaternion();
        if (q == tl::Quaternion<double>::zero())
            bRtkOrientations = false;
        else
            bRtkOrientations = true;
    }

    return bRtkOrientations;
}

std::map<QString, std::array<double, 3>> cameraPositions(const ProjectImp &project)
{
    std::map<QString, std::array<double, 3>> camera_positions;

    for (const auto &image : project.images()) {

        QString path = image.second.path();
        CameraPose camera_pose = image.second.cameraPose();

        if (!camera_pose.isEmpty()) {
            std::array<double, 3> positions = {
            camera_pose.position().x,
            camera_pose.position().y,
            camera_pose.position().z};
            camera_positions[path] = positions;
        }

    }

    return camera_positions;
}

OrientationCommand::OrientationCommand()
  : Command("ori", "3D Reconstruction")
{

    this->addArgument<std::string>("prj", 'p', "Project file");
    this->addArgument<bool>("fix_calibration", 'c', "Fix calibration", false);
    this->addArgument<bool>("fix_poses", 'p', "Fix Poses", false);
    this->addArgument<bool>("absolute_orientation", 'a', "Absolute Orientation", false);

    this->addExample("ori -p 253/253.xml -a");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

OrientationCommand::~OrientationCommand()
{

}

bool OrientationCommand::run()
{
    bool r = false;

    try {

        tl::Path project_path = this->value<std::string>("prj");
        bool fix_calibration = this->value<bool>("fix_calibration");
        bool fix_poses = this->value<bool>("fix_poses");
        bool absolute_orientation = this->value<bool>("absolute_orientation");

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(project_path);
        project.clearReconstruction();
        tl::Path database_path = project.database();
        tl::Path sfm_path = project.projectFolder();
        sfm_path.append("sfm");

        std::vector<Image> images;
        for (const auto &image : project.images()) {
            images.push_back(image.second);
        }

        if (rtkOrientations(project)) {

            ImportPosesTask import_orientation_task(images,
                                                    project.cameras(),
                                                    sfm_path,
                                                    database_path,
                                                    fix_calibration,
                                                    fix_poses);

            import_orientation_task.run();

            auto cameras = import_orientation_task.cameras();

            tl::Path offset_path = sfm_path;
            offset_path.append("offset.txt");

            tl::Path sparse_model_path = sfm_path;
            sparse_model_path.append("sparse.ply");

            tl::Path ground_points_path = sfm_path;
            ground_points_path.append("ground_points.bin");

            tl::Path poses_path = sfm_path;
            poses_path.append("poses.bin");

            if (sparse_model_path.exists() &&
                ground_points_path.exists() &&
                poses_path.exists()) {

                project.setSparseModel(sparse_model_path);
                project.setOffset(offset_path);
                project.setGroundPoints(ground_points_path);

                auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
                poses_reader->read(poses_path);
                auto poses = poses_reader->cameraPoses();

                for (const auto &camera_pose : poses) {
                    project.addPhotoOrientation(camera_pose.first, camera_pose.second);
                }

                tl::Message::info("Oriented {} images", poses.size());

                for (const auto &camera : cameras) {
                    project.updateCamera(camera.first, camera.second);
                }
            }

        } else {



            RelativeOrientationColmapTask relative_orientation_task(database_path,
                                                                    sfm_path,
                                                                    images,
                                                                    project.cameras(),
                                                                    fix_calibration);

            relative_orientation_task.run();

            auto cameras = relative_orientation_task.cameras();

            /// Se comprueba que se han generado todos los productos


            tl::Path sparse_model_path = sfm_path;
            sparse_model_path.append("sparse.ply");

            tl::Path ground_points_path = sfm_path;
            ground_points_path.append("ground_points.bin");

            tl::Path poses_path = sfm_path;
            poses_path.append("poses.bin");

            if (sparse_model_path.exists() &&
                ground_points_path.exists() &&
                poses_path.exists()) {

                project.setSparseModel(sparse_model_path);
                project.setOffset(tl::Path(""));
                project.setGroundPoints(ground_points_path);

                auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
                poses_reader->read(poses_path);
                auto poses = poses_reader->cameraPoses();

                for (const auto &camera_pose : poses) {
                    project.addPhotoOrientation(camera_pose.first, camera_pose.second);
                }

                tl::Message::info("Oriented {} images", poses.size());

                for (const auto &camera : cameras) {
                    project.updateCamera(camera.first, camera.second);
                }

                auto report = relative_orientation_task.report();
                report.time = relative_orientation_task.time();
                report.orientedImages = static_cast<int>(poses.size());
                report.type = "Relative";
                project.setOrientationReport(report);
            }

            if (absolute_orientation) {

                std::map<QString, std::array<double, 3>> camera_positions = cameraPositions(project);

                AbsoluteOrientationColmapTask absolute_orientation_task(sfm_path,
                                                                        images);
                absolute_orientation_task.run();

                tl::Path offset_path = sfm_path;
                offset_path.append("offset.txt");

                tl::Path poses_path = sfm_path;
                poses_path.append("poses.bin");

                if (offset_path.exists()) {
                    project.setOffset(offset_path);
                }

                if (poses_path.exists()) {
                    auto poses_reader = CameraPosesReaderFactory::create("GRAPHOS");
                    poses_reader->read(poses_path);
                    auto poses = poses_reader->cameraPoses();

                    for (const auto &camera_pose : poses) {
                        project.addPhotoOrientation(camera_pose.first, camera_pose.second);
                    }
                }

                auto report = project.orientationReport();
                report.type = "Absolute";
                report.time += absolute_orientation_task.time();
                project.setOrientationReport(report);

            }
        }

        project.save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    return r;
}

} // namespace graphos
