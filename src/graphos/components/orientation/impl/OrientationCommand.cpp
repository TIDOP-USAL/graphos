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
#include "graphos/core/orientation/orientation.h"
#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/posesio.h"
#include "graphos/components/orientation/impl/RelativeOrientationProcess.h"
#include "graphos/components/orientation/impl/AbsoluteOrientationProcess.h"
#include "graphos/components/orientation/impl/ImportOrientationProcess.h"

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

  ImportPoses(Project* project);
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
  tl::Point3D offset();

private:

  Project* mProject;
  tl::Point3D mOffset;
  bool mFixCalibration;
  bool mFixPoses;
};

ImportPoses::ImportPoses(Project* project)
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

  tl::Path reconstruction_path(mProject->projectFolder().toStdWString());

  if (isCoordinatesLocal()) {
    reconstruction_path.append("ori").append("relative");
  } else {
    reconstruction_path.append("ori").append("absolute");
  }
  reconstruction_path.createDirectories();

  bool clear_points = false;

  colmap::OptionManager options;
  options.AddDatabaseOptions();
  options.AddImageOptions();
  options.AddRequiredOption("input_path", &temp_path.toString());
  options.AddRequiredOption("reconstruction_path", &reconstruction_path.toString());
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

  TL_ASSERT(reconstruction.NumRegImages() >= 2, "Need at least two images for triangulation")

  colmap::IncrementalMapper mapper(&database_cache);
  mapper.BeginReconstruction(&reconstruction);

  this->triangulation(reconstruction, mapper, mapper_options->Triangulation());

  msgInfo("Retriangulation");

  CompleteAndMergeTracks(*mapper_options, &mapper);

  bundleAdjustment(reconstruction, mapper, mapper_options);

  msgInfo("Extracting colors");
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

    msgInfo("Camera offset: %lf,%lf,%lf", mOffset.x, mOffset.y, mOffset.z);

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
  for (auto it_image = mProject->imageBegin();
    it_image != mProject->imageEnd();
    it_image++) {
    CameraPose camera_pose = it_image->cameraPose();
    if (camera_pose.isEmpty()) continue;
    mOffset += (camera_pose.position() - mOffset) / i;
    i++;
  }
}

void ImportPoses::temporalReconstruction(const tl::Path& tempPath)
{
  writeImages(tempPath);
  writeCameras(tempPath);
  writePoints(tempPath);
}

void ImportPoses::writeImages(const tl::Path& tempPath)
{
  tl::Path images_path(tempPath);
  images_path.append("images.txt");

  std::ofstream ofs;
  ofs.open(images_path.toString(), std::ofstream::out | std::ofstream::trunc);

  if (!ofs.is_open()) throw std::runtime_error(std::string("Open fail: images.txt"));

  size_t id = 1;

  for (auto it_image = mProject->imageBegin(); it_image != mProject->imageEnd(); it_image++) {

    CameraPose camera_pose = it_image->cameraPose();
    if (camera_pose.isEmpty()) {
      id++;
      continue; /// Se saltan las imagenes no orientadas
    }

    tl::math::Quaternion<double> quaternion = camera_pose.quaternion();
    std::string file_name = it_image->path().toStdString();
    tl::Point3D position = camera_pose.position();

    if (!isCoordinatesLocal()) {
      position -= mOffset;
    }

    tl::math::Vector<double, 3> vector_camera_position = position.vector();

    tl::math::RotationMatrix<double> r_ip_ic = tl::math::RotationMatrix<double>::identity();
    r_ip_ic.at(1, 1) = -1;
    r_ip_ic.at(2, 2) = -1;

    tl::math::RotationMatrix<double> rotation_matrix;
    tl::math::RotationConverter<double>::convert(quaternion, rotation_matrix);

    tl::math::RotationMatrix<double> rotation = r_ip_ic * rotation_matrix.transpose();
    tl::math::RotationConverter<double>::convert(rotation, quaternion);
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

  for (auto it_camera = mProject->cameraBegin(); it_camera != mProject->cameraEnd(); it_camera++) {
    Camera camera = it_camera->second;
    std::string camera_type = cameraToColmapType(camera).toStdString();
    std::shared_ptr<Calibration> calibration = camera.calibration();

    if (calibration) {

      ofs << it_camera->first << " " << camera_type << " " << camera.width() << " " << camera.height() << " ";

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

      ofs << it_camera->first << " " << camera_type << " " << camera.width() << " " << camera.height() << " ";

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

  colmap::Database database(mProject->database().toStdString());

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

  auto it_image = mProject->imageBegin();
  if (it_image != mProject->imageEnd()) {
    CameraPose camera_pose = it_image->cameraPose();
    if (camera_pose.crs() != "") local = false;
  }

  return local;
}

tl::Point3D ImportPoses::offset()
{
  return mOffset;
}


}


bool rtkOrientations(const ProjectImp &project)
{
  bool bRtkOrientations = false;

  auto it = project.imageBegin();
  CameraPose camera_pose = it->cameraPose();
  if (!camera_pose.isEmpty()) {
    tl::math::Quaternion<double> q = camera_pose.quaternion();
    if (q == tl::math::Quaternion<double>::zero())
      bRtkOrientations = false;
    else
      bRtkOrientations = true;
  }

  return bRtkOrientations;
}

std::map<QString, std::array<double, 3>> cameraPositions(const ProjectImp &project)
{
  std::map<QString, std::array<double, 3>> camera_positions;
  for (auto it = project.imageBegin(); it != project.imageEnd(); it++) {
    QString path = it->path();
    CameraPose camera_pose = it->cameraPose();
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
  : Command("ori", "3D Reconstruction"),
    mProjectFile(""),
    mFixCalibration(false),
    mFixPoses(false),
    mAbsoluteOrientation(false)
{

  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentBooleanOptional("fix_calibration", 'c', "Fix calibration", &mFixCalibration));
  this->push_back(CreateArgumentBooleanOptional("fix_poses", 'p', "Fix Poses", &mFixPoses));
  this->push_back(CreateArgumentBooleanOptional("absolute_orientation", 'a', "Absolute Orientation", &mAbsoluteOrientation));

  this->addExample("ori --p 253/253.xml -a");

  this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

OrientationCommand::~OrientationCommand()
{

}

bool OrientationCommand::run()
{
  bool r = false;

  //QString file_path;
  //QString project_path;

  //try {

  //  if (!mProjectFile.exists()) {
  //    msgError("Project doesn't exist");
  //    return 1;
  //  }

  //  QString project_file = QString::fromStdWString(mProjectFile.toWString());

  //  mProject = new ProjectImp;
  //  mProject->load(project_file);
  //  QString database_path = mProject->database();
  //  std::vector<Image> images = mProject->images();
  //  std::map<int, Camera> cameras = mProject->cameras();

  //  if (mFixPoses) {

  //    internal::ImportPoses importPoses(mProject);
  //    importPoses.setFixCalibration(mFixCalibration);
  //    importPoses.setFixPoses(mFixPoses);

  //  } else {

  //    QString ori_relative = mProject->projectFolder() + "/ori/relative/";
  //    RelativeOrientationColmapAlgorithm relativeOrientationAlgorithm(database_path,
  //                                                                    ori_relative,
  //                                                                    mFixCalibration);

  //    relativeOrientationAlgorithm.run();

  //    /// Se comprueba que se han generado todos los productos

  //    QString sparse_model = ori_relative + "sparse.ply";

  //    if (QFileInfo(sparse_model).exists()) {
  //      mProject->setReconstructionPath(ori_relative);
  //      mProject->setSparseModel(sparse_model);
  //      mProject->setOffset("");

  //      ReadCameraPoses readPhotoOrientations;
  //      readPhotoOrientations.open(ori_relative);
  //      int oriented_images = 0;

  //      for (auto image = mProject->imageBegin(); image != mProject->imageEnd(); image++) {
  //        QString image_oriented = image->path();
  //        CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
  //        if (photoOrientation.position() != tl::Point3D()) {
  //          mProject->addPhotoOrientation(image->name(), photoOrientation);
  //          oriented_images++;
  //        } else {
  //          QString image_oriented = QFileInfo(image->path()).fileName();
  //          QByteArray ba = image_oriented.toLocal8Bit();
  //          const char *msg = ba.constData();
  //          msgWarning("Image %s not oriented", msg);
  //        }

  //      }

  //      msgInfo("Oriented %i images", oriented_images);

  //      ReadCalibration readCalibration;
  //      readCalibration.open(ori_relative);
  //      std::shared_ptr<Calibration> calibration;
  //      for (auto camera_it = mProject->cameraBegin(); camera_it != mProject->cameraEnd(); camera_it++) {
  //        calibration = readCalibration.calibration(camera_it->first);
  //        if (calibration) {
  //          Camera camera = camera_it->second;
  //          camera.setCalibration(calibration);
  //          mProject->updateCamera(camera_it->first, camera);
  //        }
  //      }

  //    }

  //    if (mAbsoluteOrientation) {

  //      QString ori_absolute = mProject->projectFolder() + "/ori/absolute/";
  //      std::map<QString, std::array<double, 3>> camera_positions;
  //      for (auto it = mProject->imageBegin(); it != mProject->imageEnd(); it++) {
  //        QString path = it->path();
  //        CameraPose camera_pose = it->cameraPose();
  //        if (!camera_pose.isEmpty()) {
  //          std::array<double, 3> positions = {
  //          camera_pose.position().x,
  //          camera_pose.position().y,
  //          camera_pose.position().z };
  //          camera_positions[path] = positions;
  //        }
  //      }

  //      AbsoluteOrientationColmapAlgorithm absoluteOrientationAlgorithm(ori_relative,
  //        camera_positions,
  //        ori_absolute);
  //      absoluteOrientationAlgorithm.run();

  //      QString sparse_model = ori_absolute + "/sparse.ply";
  //      if (QFileInfo::exists(sparse_model)) {
  //        mProject->setReconstructionPath(ori_absolute);
  //        mProject->setSparseModel(ori_absolute + "/sparse.ply");
  //        mProject->setOffset(ori_absolute + "/offset.txt");

  //        ReadCameraPoses readPhotoOrientations;
  //        readPhotoOrientations.open(ori_absolute);
  //        for (auto image = mProject->imageBegin(); image != mProject->imageEnd(); image++) {
  //          CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
  //          if (photoOrientation.position() != tl::Point3D()) {
  //            mProject->addPhotoOrientation(image->name(), photoOrientation);
  //          }
  //        }

  //        ReadCalibration readCalibration;
  //        readCalibration.open(ori_absolute);
  //        std::shared_ptr<Calibration> calibration;
  //        for (auto camera_it = mProject->cameraBegin(); camera_it != mProject->cameraEnd(); camera_it++) {
  //          calibration = readCalibration.calibration(camera_it->first);
  //          if (calibration) {
  //            Camera camera = camera_it->second;
  //            camera.setCalibration(calibration);
  //            mProject->updateCamera(camera_it->first, camera);
  //          }
  //        }
  //      }

  //    }

  //  }

  //  mProject->save(project_file);

  //} catch (const std::exception& e) {
  //  tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  //  r = true;
  //}

  try{

    TL_ASSERT(mProjectFile.exists(), "Project doesn't exist");
    TL_ASSERT(mProjectFile.isFile(), "Project file doesn't exist")

    QString project_file = QString::fromStdWString(mProjectFile.toWString());

    ProjectImp project;
    project.load(project_file);
    project.clearReconstruction();
    QString database_path = project.database();

    if (rtkOrientations(project)) {

      //orientation_process = std::make_unique<ImportOrientationProcess>(mModel->images(),
      //                                                                 mModel->cameras(),
      //                                                                 mModel->projectPath(),
      //                                                                 mModel->database(),
      //                                                                 mView->fixCalibration(),
      //                                                                 mView->fixPoses());

      //connect(dynamic_cast<ImportOrientationProcess *>(orientation_process.get()), &ImportOrientationProcess::importOrientationFinished,
      //        this, &OrientationPresenterImp::onAbsoluteOrientationFinished);

    } else {

      QString ori_relative_path = project.projectFolder() + "/ori/relative/";

      RelativeOrientationProcess relativeOrientationProcess(database_path,
                                                            ori_relative_path,
                                                            mFixCalibration);

      relativeOrientationProcess.run();

      QString sparse_model = ori_relative_path + "sparse.ply";

      if (QFileInfo(sparse_model).exists()) {
        project.setReconstructionPath(ori_relative_path);
        project.setSparseModel(sparse_model);
        project.setOffset("");

        ReadCameraPoses readPhotoOrientations;
        readPhotoOrientations.open(ori_relative_path);
        int oriented_images = 0;

        for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {
          QString image_oriented = image->path();
          CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
          if (photoOrientation.position() != tl::Point3D()) {
            project.addPhotoOrientation(image->name(), photoOrientation);
            oriented_images++;
          } else {
            QString image_oriented = QFileInfo(image->path()).fileName();
            QByteArray ba = image_oriented.toLocal8Bit();
            const char *msg = ba.constData();
            msgWarning("Image %s not oriented", msg);
          }

        }

        msgInfo("Oriented %i images", oriented_images);

        ReadCalibration readCalibration;
        readCalibration.open(ori_relative_path);
        std::shared_ptr<Calibration> calibration;
        for (auto camera_it = project.cameraBegin(); camera_it != project.cameraEnd(); camera_it++) {
          calibration = readCalibration.calibration(camera_it->first);
          if (calibration) {
            Camera camera = camera_it->second;
            camera.setCalibration(calibration);
            project.updateCamera(camera_it->first, camera);
          }
        }

      }

      if (mAbsoluteOrientation) {

        QString ori_absolute = project.projectFolder() + "/ori/absolute/";
        std::map<QString, std::array<double, 3>> camera_positions = cameraPositions(project);

        AbsoluteOrientationProcess absoluteOrientationProcess(ori_relative_path,
                                                              camera_positions,
                                                              ori_absolute);
        absoluteOrientationProcess.run();

        QString sparse_model = ori_absolute + "/sparse.ply";

        if (QFileInfo::exists(sparse_model)) {
          project.setReconstructionPath(ori_absolute);
          project.setSparseModel(ori_absolute + "/sparse.ply");
          project.setOffset(ori_absolute + "/offset.txt");

          ReadCameraPoses readPhotoOrientations;
          readPhotoOrientations.open(ori_absolute);
          for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {
            CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
            if (photoOrientation.position() != tl::Point3D()) {
              project.addPhotoOrientation(image->name(), photoOrientation);
            }
          }

          ReadCalibration readCalibration;
          readCalibration.open(ori_absolute);
          std::shared_ptr<Calibration> calibration;
          for (auto camera_it = project.cameraBegin(); camera_it != project.cameraEnd(); camera_it++) {
            calibration = readCalibration.calibration(camera_it->first);
            if (calibration) {
              Camera camera = camera_it->second;
              camera.setCalibration(calibration);
              project.updateCamera(camera_it->first, camera);
            }
          }
        }

      }
    }

    project.save(project_file);

  } catch (const std::exception &e) {

    printException(e);

    r = true;
  }

  return r;
}

} // namespace graphos