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

#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/sfm/orientationexport.h"
#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/image.h"

#include <tidop/core/messages.h>
#include <tidop/core/path.h>
#include <tidop/core/chrono.h>
#include <tidop/core/progress.h>

#include <colmap/base/reconstruction.h>
#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/controllers/hierarchical_mapper.h>
#include <colmap/controllers/bundle_adjustment.h>

namespace graphos
{

class ColmapReconstructionConvert
{

public:

  ColmapReconstructionConvert(const colmap::Reconstruction *reconstruction,
                              const std::vector<Image> &images)
    : mReconstruction(reconstruction),
      mImages(images)
  {

    for (const auto &image : mImages) {

      tl::Path image_path(image.path().toStdString());

      for (const auto &colmap_image : mReconstruction->Images()) {
        tl::Path colmap_image_path(colmap_image.second.Name());

        if (image_path.equivalent(colmap_image_path)) {
          mImageIds[colmap_image.first] = image.id();
          break;
        }
      }

    }
  }

  ~ColmapReconstructionConvert()
  {
  }

  std::vector<GroundPoint> groundPoints() const
  {
    /// Muy Lento... 
    /// Crear una tabla de equivalencias de id_image_colmap e id_image_graphos

    std::vector<GroundPoint> ground_points(mReconstruction->NumPoints3D());

    size_t i = 0;

    for (auto &points_3d : mReconstruction->Points3D()) {

      GroundPoint ground_point;
      ground_point.setPoint(tl::Point3<double>(points_3d.second.X(),
                                               points_3d.second.Y(),
                                               points_3d.second.Z()));

      tl::graph::ColorRGB rgb(points_3d.second.Color(0),
                              points_3d.second.Color(1),
                              points_3d.second.Color(2));
      ground_point.setColor(rgb.toColor());

      colmap::Track track = points_3d.second.Track();

      for (auto &pair : track.Elements()) {

        //colmap::Image colmap_image = mReconstruction->Image(pair.image_id);
        //tl::Path colmap_image_path(colmap_image.Name());
        //for (const auto &image : mImages) {
        //  tl::Path image_path(image.path().toStdString());
        //  if (image_path.equivalent(colmap_image_path)) {
        //    ground_point.addPairToTrack(image.id(), pair.point2D_idx);
        //    break;
        //  }
        //}

        ground_point.addPairToTrack(mImageIds.at(pair.image_id), pair.point2D_idx);

      }

      ground_points[i] = ground_point;

      ++i;
    }

    return ground_points;
  }

  std::unordered_map<size_t, CameraPose> cameraPoses() const
  {
    std::unordered_map<size_t, CameraPose> camera_poses;

    for (auto &pair : mReconstruction->Images()) {

      CameraPose photoOrientation;

      colmap::Image colmap_image = pair.second;
      tl::Path colmap_image_path(colmap_image.Name());

      //for (const auto &image : mImages) {

        //tl::Path image_path(image.path().toStdString());

        //if (image_path.equivalent(colmap_image_path)) {

          const Eigen::Matrix<double, 3, 4> inv_proj_matrix = colmap_image.InverseProjectionMatrix();
          const Eigen::Vector3d pc = inv_proj_matrix.rightCols<1>();
          photoOrientation.setPosition(tl::Point3D(pc(0), pc(1), pc(2)));

          Eigen::Matrix3d rot = colmap_image.RotationMatrix();
          tl::math::RotationMatrix<double> rotation_matrix;
          rotation_matrix.at(0, 0) = rot(0, 0);
          rotation_matrix.at(0, 1) = rot(0, 1);
          rotation_matrix.at(0, 2) = rot(0, 2);
          rotation_matrix.at(1, 0) = rot(1, 0);
          rotation_matrix.at(1, 1) = rot(1, 1);
          rotation_matrix.at(1, 2) = rot(1, 2);
          rotation_matrix.at(2, 0) = rot(2, 0);
          rotation_matrix.at(2, 1) = rot(2, 1);
          rotation_matrix.at(2, 2) = rot(2, 2);
          photoOrientation.setRotationMatrix(rotation_matrix);

          //camera_poses[image.id()] = photoOrientation;
          camera_poses[mImageIds.at(pair.first)] = photoOrientation;
        //  break;
        //}
      //}

    }

    return camera_poses;
  }

  std::shared_ptr<Calibration> readCalibration(size_t cameraId) const
  {
    std::shared_ptr<Calibration> calibration;
    if (mReconstruction->ExistsCamera(static_cast<colmap::image_t>(cameraId))) {

      const colmap::Camera &camera = mReconstruction->Camera(static_cast<colmap::image_t>(cameraId));
      std::vector<double> params = camera.Params();

      std::string model_name = camera.ModelName();

      if (model_name.compare("SIMPLE_PINHOLE") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::simple_pinhole);
        calibration->setParameter(Calibration::Parameters::focal, params[0]);
        calibration->setParameter(Calibration::Parameters::cx, params[1]);
        calibration->setParameter(Calibration::Parameters::cy, params[2]);

      } else if (model_name.compare("PINHOLE") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::pinhole);
        calibration->setParameter(Calibration::Parameters::focalx, params[0]);
        calibration->setParameter(Calibration::Parameters::focaly, params[1]);
        calibration->setParameter(Calibration::Parameters::cx, params[2]);
        calibration->setParameter(Calibration::Parameters::cy, params[3]);

      } else if (model_name.compare("SIMPLE_RADIAL") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::radial1);
        calibration->setParameter(Calibration::Parameters::focal, params[0]);
        calibration->setParameter(Calibration::Parameters::cx, params[1]);
        calibration->setParameter(Calibration::Parameters::cy, params[2]);
        calibration->setParameter(Calibration::Parameters::k1, params[3]);

      } else if (model_name.compare("RADIAL") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::radial2);
        calibration->setParameter(Calibration::Parameters::focal, params[0]);
        calibration->setParameter(Calibration::Parameters::cx, params[1]);
        calibration->setParameter(Calibration::Parameters::cy, params[2]);
        calibration->setParameter(Calibration::Parameters::k1, params[3]);
        calibration->setParameter(Calibration::Parameters::k2, params[4]);

      } else if (model_name.compare("OPENCV") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
        calibration->setParameter(Calibration::Parameters::focalx, params[0]);
        calibration->setParameter(Calibration::Parameters::focaly, params[1]);
        calibration->setParameter(Calibration::Parameters::cx, params[2]);
        calibration->setParameter(Calibration::Parameters::cy, params[3]);
        calibration->setParameter(Calibration::Parameters::k1, params[4]);
        calibration->setParameter(Calibration::Parameters::k2, params[5]);
        calibration->setParameter(Calibration::Parameters::p1, params[6]);
        calibration->setParameter(Calibration::Parameters::p2, params[7]);

      } else if (model_name.compare("OPENCV_FISHEYE") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
        calibration->setParameter(Calibration::Parameters::focalx, params[0]);
        calibration->setParameter(Calibration::Parameters::focaly, params[1]);
        calibration->setParameter(Calibration::Parameters::cx, params[2]);
        calibration->setParameter(Calibration::Parameters::cy, params[3]);
        calibration->setParameter(Calibration::Parameters::k1, params[4]);
        calibration->setParameter(Calibration::Parameters::k2, params[5]);
        calibration->setParameter(Calibration::Parameters::k3, params[6]);
        calibration->setParameter(Calibration::Parameters::k4, params[7]);

      } else if (model_name.compare("FULL_OPENCV") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_full);
        calibration->setParameter(Calibration::Parameters::focalx, params[0]);
        calibration->setParameter(Calibration::Parameters::focaly, params[1]);
        calibration->setParameter(Calibration::Parameters::cx, params[2]);
        calibration->setParameter(Calibration::Parameters::cy, params[3]);
        calibration->setParameter(Calibration::Parameters::k1, params[4]);
        calibration->setParameter(Calibration::Parameters::k2, params[5]);
        calibration->setParameter(Calibration::Parameters::p1, params[6]);
        calibration->setParameter(Calibration::Parameters::p2, params[7]);
        calibration->setParameter(Calibration::Parameters::k3, params[8]);
        calibration->setParameter(Calibration::Parameters::k4, params[9]);
        calibration->setParameter(Calibration::Parameters::k5, params[10]);
        calibration->setParameter(Calibration::Parameters::k6, params[11]);

      } else if (model_name.compare("SIMPLE_RADIAL_FISHEYE") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::simple_radial_fisheye);
        calibration->setParameter(Calibration::Parameters::focal, params[0]);
        calibration->setParameter(Calibration::Parameters::cx, params[1]);
        calibration->setParameter(Calibration::Parameters::cy, params[2]);
        calibration->setParameter(Calibration::Parameters::k1, params[3]);

      } else if (model_name.compare("RADIAL_FISHEYE") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::radial_fisheye);
        calibration->setParameter(Calibration::Parameters::focal, params[0]);
        calibration->setParameter(Calibration::Parameters::cx, params[1]);
        calibration->setParameter(Calibration::Parameters::cy, params[2]);
        calibration->setParameter(Calibration::Parameters::k1, params[3]);
        calibration->setParameter(Calibration::Parameters::k2, params[4]);

      } else if (model_name.compare("FULL_RADIAL") == 0) {

        calibration = CalibrationFactory::create(Calibration::CameraModel::radial3);
        calibration->setParameter(Calibration::Parameters::focal, params[0]);
        calibration->setParameter(Calibration::Parameters::cx, params[1]);
        calibration->setParameter(Calibration::Parameters::cy, params[2]);
        calibration->setParameter(Calibration::Parameters::k1, params[3]);
        calibration->setParameter(Calibration::Parameters::k2, params[4]);
        calibration->setParameter(Calibration::Parameters::k3, params[5]);
        calibration->setParameter(Calibration::Parameters::p1, params[6]);
        calibration->setParameter(Calibration::Parameters::p2, params[7]);

      } else {
        TL_TODO("camara no soportada")
          return nullptr;
      }

    }

    return calibration;
  }

private:

  const colmap::Reconstruction *mReconstruction;
  std::vector<Image> mImages;
  std::unordered_map<colmap::image_t, size_t> mImageIds;
};



RelativeOrientationColmapProperties::RelativeOrientationColmapProperties()
  : mRefineFocalLength(true),
    mRefinePrincipalPoint(false),
    mRefineExtraParams(true)
{

}

bool RelativeOrientationColmapProperties::refineFocalLength() const
{
  return mRefineFocalLength;
}

void RelativeOrientationColmapProperties::setRefineFocalLength(bool refineFocalLength)
{
  mRefineFocalLength = refineFocalLength;
}

bool RelativeOrientationColmapProperties::refinePrincipalPoint() const
{
  return mRefinePrincipalPoint;
}

void RelativeOrientationColmapProperties::setRefinePrincipalPoint(bool refinePrincipalPoint)
{
  mRefinePrincipalPoint = refinePrincipalPoint;
}

bool RelativeOrientationColmapProperties::refineExtraParams() const
{
  return mRefineExtraParams;
}

void RelativeOrientationColmapProperties::setRefineExtraParams(bool refineExtraParams)
{
  mRefineExtraParams = refineExtraParams;
}

void RelativeOrientationColmapProperties::reset()
{
  mRefineFocalLength = true;
  mRefinePrincipalPoint = false;
  mRefineExtraParams = true;
}

QString RelativeOrientationColmapProperties::name() const
{
  return QString("Colmap");
}



/*----------------------------------------------------------------*/



RelativeOrientationColmapTask::RelativeOrientationColmapTask(const QString &database,
                                                             const QString &outputPath,
                                                             const std::vector<Image> &images, 
                                                             const std::map<int, Camera> &cameras,
                                                             bool fixCalibration)
  : mDatabase(database),
    mOutputPath(outputPath),
    mImages(images),
    mCameras(cameras),
    mIncrementalMapper(new colmap::IncrementalMapperOptions),
    mMapper(nullptr),
    mReconstructionManager(new colmap::ReconstructionManager),
    mBundleAdjustmentController(nullptr)
{
  setRefineFocalLength(!fixCalibration);
  setRefinePrincipalPoint(!fixCalibration);
  setRefineExtraParams(!fixCalibration);
}

RelativeOrientationColmapTask::~RelativeOrientationColmapTask()
{
  if (mIncrementalMapper) {
    delete mIncrementalMapper;
    mIncrementalMapper = nullptr;
  }

  if (mMapper) {
    delete mMapper;
    mMapper = nullptr;
  }

  if (mBundleAdjustmentController) {
    delete mBundleAdjustmentController;
    mBundleAdjustmentController = nullptr;
  }

  mReconstructionManager->Clear();
  mReconstructionManager.reset();
}

std::map<int, Camera> RelativeOrientationColmapTask::cameras() const
{
  return mCameras;
}

void RelativeOrientationColmapTask::stop()
{
  TaskBase::stop();

  if (mMapper && mMapper->IsRunning())
    mMapper->Stop();

  if (mBundleAdjustmentController && mBundleAdjustmentController->IsRunning())
    mBundleAdjustmentController->Stop();
}

void RelativeOrientationColmapTask::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Relative Orientation finished");
    chrono.run();

    mReconstructionManager->Clear();

    std::string sparse_path = mOutputPath.toStdString();

    tl::Path dir(sparse_path);
    if (!dir.exists() && !dir.createDirectories()) {
      throw std::runtime_error(std::string("Directory couldn't be created: ").append(sparse_path));
    }

    if (mMapper) {
      delete mMapper;
      mMapper = nullptr;
    }

    mIncrementalMapper->ba_refine_focal_length = refineFocalLength();
    mIncrementalMapper->ba_refine_principal_point = false;
    mIncrementalMapper->ba_refine_extra_params = refineExtraParams();

    mMapper = new colmap::IncrementalMapperController(mIncrementalMapper,
                                                      "",
                                                      mDatabase.toStdString(),
                                                      mReconstructionManager.get());

    size_t prev_num_reconstructions = 0;
    mMapper->AddCallback(
      colmap::IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [&]() {

        try {
          // If the number of reconstructions has not changed, the last model
          // was discarded for some reason.
          if (mReconstructionManager->Size() > prev_num_reconstructions) {
            const std::string reconstruction_path = sparse_path;
            const auto &reconstruction = mReconstructionManager->Get(prev_num_reconstructions);
            //colmap::CreateDirIfNotExists(reconstruction_path);
            //reconstruction.Write(reconstruction_path);
            //mOptions->Write(JoinPaths(reconstruction_path, "project.ini"));
            ///TODO: Por ahora sólo trabajamos con una reconstrucción
            //prev_num_reconstructions = mReconstructionManager->Size();
          }
        } catch (std::exception &e) {
          msgError(e.what());
        }

      });

    mMapper->AddCallback(
      colmap::IncrementalMapperController::NEXT_IMAGE_REG_CALLBACK, [&]() {
        //if (progressBar) (*progressBar)();
        //msgInfo("-----");
      });

    mMapper->Start(); ///TODO: ¿Como detectar que se ha producido un error?
    mMapper->Wait();

    if (status() == tl::Task::Status::stopping) return;

    if (mReconstructionManager->Size() == 0) throw std::runtime_error("Reconstruction fail");

    colmap::OptionManager optionManager;
    optionManager.bundle_adjustment->refine_focal_length = refineFocalLength();
    optionManager.bundle_adjustment->refine_principal_point = refinePrincipalPoint();
    optionManager.bundle_adjustment->refine_extra_params = refineExtraParams();

    colmap::Reconstruction &reconstruction = mReconstructionManager->Get(0);

    mBundleAdjustmentController = new colmap::BundleAdjustmentController(optionManager, &reconstruction);

    mBundleAdjustmentController->Start();
    mBundleAdjustmentController->Wait();

    if (status() == tl::Task::Status::stopping) return;

    ColmapReconstructionConvert convert(&reconstruction, mImages);

    std::vector<GroundPoint> ground_points = convert.groundPoints();
    auto gp_writer = GroundPointsWriterFactory::create("GRAPHOS");
    gp_writer->setGroundPoints(ground_points);
    tl::Path ground_points_path(sparse_path);
    ground_points_path.append("ground_points.bin");
    gp_writer->write(ground_points_path);


    auto camera_poses = convert.cameraPoses();
    auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
    poses_writer->setCameraPoses(camera_poses);
    tl::Path poses_path(sparse_path);
    poses_path.append("poses.bin");
    poses_writer->write(poses_path);

    for (auto &camera : mCameras) {

      std::shared_ptr<Calibration> calibration = convert.readCalibration(camera.first);

      if (calibration) {
        camera.second.setCalibration(calibration);
      }
    }

    OrientationExport orientationExport(&reconstruction);

    QString path = QString::fromStdString(sparse_path);
    orientationExport.exportBinary(path); // TODO: Por ahora lo guardo y lo borro al finalizar
    orientationExport.exportPLY(path + "/sparse.ply");


    if (status() == tl::Task::Status::stopping) return;

    chrono.stop();

    if (progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Relative Orientation error");
  }
}


/*----------------------------------------------------------------*/

constexpr auto min_common_images = 3;
constexpr auto robust_alignment = true;
constexpr auto robust_alignment_max_error = 1.;

AbsoluteOrientationColmapProperties::AbsoluteOrientationColmapProperties()
  : mMinCommonImages(min_common_images),
    mRobustAlignment(robust_alignment),
    mRobustAlignmentMaxError(robust_alignment_max_error)
{
}

int AbsoluteOrientationColmapProperties::minCommonImages() const
{
  return mMinCommonImages;
}

void AbsoluteOrientationColmapProperties::setMinCommonImages(int minCommonImages)
{
  mMinCommonImages = minCommonImages;
}

bool AbsoluteOrientationColmapProperties::robustAlignment() const
{
  return mRobustAlignment;
}

void AbsoluteOrientationColmapProperties::setRobustAlignment(bool robustAlignment)
{
  mRobustAlignment = robustAlignment;
}

double AbsoluteOrientationColmapProperties::robustAlignmentMaxError() const
{
  return mRobustAlignmentMaxError;
}

void AbsoluteOrientationColmapProperties::setRobustAlignmentMaxError(double robustAlignmentMaxError)
{
  mRobustAlignmentMaxError = robustAlignmentMaxError;
}

void AbsoluteOrientationColmapProperties::reset()
{
  mMinCommonImages = min_common_images;
  mRobustAlignment = robust_alignment;
  mRobustAlignmentMaxError = robust_alignment_max_error;
}

QString AbsoluteOrientationColmapProperties::name() const
{
  return QString("Colmap");
}



/*----------------------------------------------------------------*/



AbsoluteOrientationColmapTask::AbsoluteOrientationColmapTask(const QString &inputPath,
                                                             const std::vector<Image> &images)
  : mInputPath(inputPath),
    mImages(images)
{
}

AbsoluteOrientationColmapTask::~AbsoluteOrientationColmapTask()
{
}

std::unordered_map<size_t, double> AbsoluteOrientationColmapTask::cameraPosesErrors() const
{
  return mCameraPosesErrors;
}

void AbsoluteOrientationColmapTask::stop()
{
  TaskBase::stop();
}

void AbsoluteOrientationColmapTask::execute(tl::Progress *progressBar)
{
  try {

    //msgInfo("Starting Absolute Orientation");

    tl::Chrono chrono("Absolute Orientation finished");
    chrono.run();

    bool robust_alignment = robustAlignment();
    colmap::RANSACOptions ransac_options;
    ransac_options.max_error = robustAlignmentMaxError();
    int min_common_images = minCommonImages();

    /// TODO: Cambiar por un error si no existe el directorio
    tl::Path dir(mInputPath.toStdString());
    //if (!dir.exists() && !dir.createDirectories()) {
    //  throw std::runtime_error(std::string("Directory couldn't be created: ").append(mOutputPath.toStdString()));
    //}

    if (robust_alignment && ransac_options.max_error <= 0) {
      throw std::runtime_error("ERROR: You must provide a maximum alignment error > 0");
    }

    colmap::Reconstruction reconstruction;
    reconstruction.Read(mInputPath.toStdString());

    std::vector<std::string> ref_image_names;
    std::vector<Eigen::Vector3d> ref_locations;

    Eigen::Vector3d offset(0., 0., 0.);

    double i = 1.;

    for (const auto &graphos_image : mImages) {

      tl::Path graphos_image_path(graphos_image.path().toStdString());

      for (const auto &colmap_image : reconstruction.Images()) {

        tl::Path colmap_image_path(colmap_image.second.Name());

        if (graphos_image_path.equivalent(colmap_image_path)) {

          ref_image_names.push_back(colmap_image.second.Name());

          tl::Point3<double> coordinates = graphos_image.cameraPose().position();

          Eigen::Vector3d camera_coordinates;
          camera_coordinates[0] = coordinates.x;
          camera_coordinates[1] = coordinates.y;
          camera_coordinates[2] = coordinates.z;

          //Para evitar desbordamiento
          offset += (camera_coordinates - offset) / (i);
          ref_locations.push_back(camera_coordinates);
          i++;

          break;
        }
      }
    }

    if (status() == tl::Task::Status::stopping) return;

    tl::Path offset_path = dir;
    offset_path.append("offset.txt");

    for (auto &pos : ref_locations) {
      pos -= offset;
    }

    if (!tl::Path(mInputPath.toStdString()).exists())
      throw std::runtime_error(std::string("Reconstruction not found in path: ").append(mInputPath.toStdString()));



    if (status() == tl::Task::Status::stopping) return;

    bool alignment_success;
    if (robust_alignment) {
      alignment_success = reconstruction.AlignRobust(
        ref_image_names, ref_locations, min_common_images, ransac_options);
    } else {
      alignment_success = reconstruction.Align(ref_image_names, ref_locations, min_common_images);
    }

    if (status() == tl::Task::Status::stopping) return;

    if (!alignment_success) throw std::runtime_error("Absolute Orientation failed");

    /// Write Ground points

    //reconstruction.Write(mOutputPath.toStdString());
    ColmapReconstructionConvert convert(&reconstruction, mImages);
    std::vector<GroundPoint> ground_points = convert.groundPoints();
    auto gp_writer = GroundPointsWriterFactory::create("GRAPHOS");
    gp_writer->setGroundPoints(ground_points);
    tl::Path ground_points_path(dir);
    ground_points_path.append("ground_points.bin");
    gp_writer->write(ground_points_path);

    // Write Camera Poses

    auto camera_poses = convert.cameraPoses();
    auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
    poses_writer->setCameraPoses(camera_poses);
    tl::Path poses_path(dir);
    poses_path.append("poses.bin");
    poses_writer->write(poses_path);

    if (status() == tl::Task::Status::stopping) return;

    /// Write Sparse Cloud

    tl::Path sparse_path(dir);
    sparse_path.append("sparse.ply");
    OrientationExport orientationExport(&reconstruction);
    orientationExport.exportBinary(QString::fromStdString(sparse_path.toString()));
    orientationExport.exportPLY(QString::fromStdString(sparse_path.toString()));

    /// writeOffset
    std::ofstream stream(offset_path.toString(), std::ios::trunc);
    if (stream.is_open()) {
      stream << QString::number(offset[0], 'f', 6).toStdString() << " "
        << QString::number(offset[1], 'f', 6).toStdString() << " "
        << QString::number(offset[2], 'f', 6).toStdString() << std::endl;

      msgInfo("Camera offset: %lf,%lf,%lf", offset[0], offset[1], offset[2]);

      stream.close();
    }


    std::vector<double> errors;
    errors.reserve(ref_image_names.size());

    for (size_t i = 0; i < ref_image_names.size(); ++i) {
      const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
      if (image != nullptr) {
        double error = (image->ProjectionCenter() - ref_locations[i]).norm();
        //std::cout << image->Name() << "[ Error: " << error << std::endl;
        errors.push_back(error);
      }
    }

    msgInfo("Alignment error: %f (mean), %f (median)",
            colmap::Mean(errors), colmap::Median(errors));

    if (status() == tl::Task::Status::stopping) return;

    chrono.stop();

    if (progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Absolute Orientation error");
  }
}

/*
void AbsoluteOrientationColmapAlgorithm::run()
{
  bool robust_alignment = AbsoluteOrientationColmapProperties::robustAlignment();
  colmap::RANSACOptions ransac_options;
  ransac_options.max_error = AbsoluteOrientationColmapProperties::robustAlignmentMaxError();
  int min_common_images = AbsoluteOrientationColmapProperties::minCommonImages();

  tl::Path dir(mOutputPath.toStdString());
  if (!dir.exists() && !dir.createDirectories()) {
    throw std::runtime_error(std::string("Directory couldn't be created: ").append(mOutputPath.toStdString()));
  }

  if (robust_alignment && ransac_options.max_error <= 0) {
    throw std::runtime_error("ERROR: You must provide a maximum alignment error > 0");
  }

  std::vector<std::string> ref_image_names;
  std::vector<Eigen::Vector3d> ref_locations;
  //std::vector<std::string> lines = colmap::ReadTextFileLines(mImagePath.toStdString());
  //for (const auto line : lines) {
  //  std::stringstream line_parser(line);
  //  std::string image_name = "";
  //  Eigen::Vector3d camera_position;
  //  line_parser >> image_name >> camera_position[0] >> camera_position[1] >>
  //    camera_position[2];
  //  ref_image_names.push_back(image_name);
  //  ref_locations.push_back(camera_position);
  //}

  Eigen::Vector3d offset(0.,0.,0.);

  double i = 1.;
  for (const auto &cameraPosition : mCameraPositions) {
    std::string image_name = cameraPosition.first.toStdString();
    ref_image_names.push_back(image_name);
    Eigen::Vector3d camera_position;
    camera_position[0] = cameraPosition.second[0];
    camera_position[1] = cameraPosition.second[1];
    camera_position[2] = cameraPosition.second[2];

    //Para evitar desbordamiento
    offset += (camera_position - offset)/(i);
    ref_locations.push_back(camera_position);
    i++;
  }
  

  QString offset_file = mOutputPath + "/offset.txt";
  std::ofstream stream(offset_file.toStdString(), std::ios::trunc);
  if (stream.is_open()) {
    stream << QString::number(offset[0], 'f', 6).toStdString() << " "
           << QString::number(offset[1], 'f', 6).toStdString() << " "
           << QString::number(offset[2], 'f', 6).toStdString() << std::endl;

    msgInfo("Camera offset: %lf,%lf,%lf", offset[0], offset[1], offset[2] );

    stream.close();
  }

  for (auto &pos : ref_locations){
    pos -= offset;
  }

  if (!tl::Path(mInputPath.toStdString()).exists())
    throw std::runtime_error(std::string("Reconstruction not found in path: ").append(mInputPath.toStdString()));

  colmap::Reconstruction reconstruction;
  reconstruction.Read(mInputPath.toStdString());

  bool alignment_success;
  if (robust_alignment) {
    alignment_success = reconstruction.AlignRobust(
      ref_image_names, ref_locations, min_common_images, ransac_options);
  } else {
    alignment_success =
      reconstruction.Align(ref_image_names, ref_locations, min_common_images);
  }

  if (!alignment_success) throw std::runtime_error("Absolute Orientation failed");

  reconstruction.Write(mOutputPath.toStdString());

  OrientationExport orientationExport(&reconstruction);
  orientationExport.exportPLY(mOutputPath + "/sparse.ply");

  std::vector<double> errors;
  errors.reserve(ref_image_names.size());

  for (size_t i = 0; i < ref_image_names.size(); ++i) {
    const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
    if (image != nullptr) {
      double error = (image->ProjectionCenter() - ref_locations[i]).norm();
      std::cout << image->Name() << "[ Error: " << error << std::endl;
      errors.push_back(error);
    }
  }

  msgInfo("Alignment error: %f (mean), %f (median)",
          colmap::Mean(errors), colmap::Median(errors));
}
*/


/*----------------------------------------------------------------*/



void colmapRemoveOrientations(const std::vector<std::string> &images, 
                              const std::string &reconstruction)
{
  colmap::Reconstruction _reconstruction;
  _reconstruction.Read(reconstruction);

  for (const auto &image : images) {
    if (const colmap::Image *_image = _reconstruction.FindImageWithName(image)) {
      _reconstruction.DeRegisterImage(_image->ImageId());
    }
  }

  _reconstruction.Write(reconstruction);
}

} // namespace graphos
