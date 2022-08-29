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
#include "graphos/core/camera/Colmap.h"

#include <tidop/core/messages.h>
#include <tidop/core/path.h>
#include <tidop/core/chrono.h>
#include <tidop/core/progress.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/rotation_convert.h>

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

      camera_poses[mImageIds.at(pair.first)] = photoOrientation;

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



RelativeOrientationColmapTask::RelativeOrientationColmapTask(const tl::Path &database,
                                                             const tl::Path &outputPath,
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

    if (mOutputPath.createDirectories()) {
      throw std::runtime_error(std::string("Directory couldn't be created: ").append(mOutputPath.toString()));
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
                                                      mDatabase.toString(),
                                                      mReconstructionManager.get());

    size_t prev_num_reconstructions = 0;
    mMapper->AddCallback(
      colmap::IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [&]() {

        try {
          // If the number of reconstructions has not changed, the last model
          // was discarded for some reason.
          if (mReconstructionManager->Size() > prev_num_reconstructions) {
            const std::string reconstruction_path = mOutputPath.toString();
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
    tl::Path ground_points_path(mOutputPath);
    ground_points_path.append("ground_points.bin");
    gp_writer->write(ground_points_path);


    auto camera_poses = convert.cameraPoses();
    auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
    poses_writer->setCameraPoses(camera_poses);
    tl::Path poses_path(mOutputPath);
    poses_path.append("poses.bin");
    poses_writer->write(poses_path);

    for (auto &camera : mCameras) {

      std::shared_ptr<Calibration> calibration = convert.readCalibration(camera.first);

      if (calibration) {
        camera.second.setCalibration(calibration);
      }
    }

    OrientationExport orientationExport(&reconstruction);

    orientationExport.exportBinary(QString::fromStdWString(mOutputPath.toWString())); // TODO: Por ahora lo guardo y lo borro al finalizar
    //orientationExport.exportText(path);
    tl::Path sparse_file = mOutputPath;
    sparse_file.append("sparse.ply");
    orientationExport.exportPLY(QString::fromStdWString(sparse_file.toWString()));

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



AbsoluteOrientationColmapTask::AbsoluteOrientationColmapTask(const tl::Path &inputPath,
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

    tl::Chrono chrono("Absolute Orientation finished");
    chrono.run();

    bool robust_alignment = robustAlignment();
    colmap::RANSACOptions ransac_options;
    ransac_options.max_error = robustAlignmentMaxError();
    int min_common_images = minCommonImages();

    TL_ASSERT(mInputPath.exists() && mInputPath.isDirectory(), "Invalid reconstruction");

    if (robust_alignment && ransac_options.max_error <= 0) {
      throw std::runtime_error("ERROR: You must provide a maximum alignment error > 0");
    }

    colmap::Reconstruction reconstruction;
    reconstruction.Read(mInputPath.toString());

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

    tl::Path offset_path = mInputPath;
    offset_path.append("offset.txt");

    for (auto &pos : ref_locations) {
      pos -= offset;
    }

    if (!tl::Path(mInputPath.toString()).exists())
      throw std::runtime_error(std::string("Reconstruction not found in path: ").append(mInputPath.toString()));

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
    tl::Path ground_points_path(mInputPath);
    ground_points_path.append("ground_points.bin");
    gp_writer->write(ground_points_path);

    // Write Camera Poses

    auto camera_poses = convert.cameraPoses();
    auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
    poses_writer->setCameraPoses(camera_poses);
    tl::Path poses_path(mInputPath);
    poses_path.append("poses.bin");
    poses_writer->write(poses_path);

    if (status() == tl::Task::Status::stopping) return;

    /// Write Sparse Cloud

    tl::Path sparse_path(mInputPath);
    sparse_path.append("sparse.ply");
    OrientationExport orientationExport(&reconstruction);
    //orientationExport.exportBinary(QString::fromStdString(sparse_path.toString()));
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


ImportPosesTask::ImportPosesTask(const std::vector<Image> &images,
                                 const std::map<int, Camera> &cameras,
                                 const tl::Path &outputPath,
                                 const tl::Path &database,
                                 bool fixCalibration,
                                 bool fixPoses)
  : tl::TaskBase(),
    mImages(images),
    mCameras(cameras),
    mOutputPath(outputPath),
    mDatabase(database),
    mFixCalibration(fixCalibration),
    mFixPoses(fixPoses)
{
  computeOffset();
}

ImportPosesTask::~ImportPosesTask()
{
}

std::map<int, Camera> ImportPosesTask::cameras() const
{
  return mCameras;
}

void ImportPosesTask::setFixCalibration(bool fixCalibration)
{
  mFixCalibration = fixCalibration;
}

void ImportPosesTask::setFixPoses(bool fixPoses)
{
  mFixPoses = fixPoses;
}

void ImportPosesTask::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Import Orientation finished");
    chrono.run();

    {
      colmap::Database database(mDatabase.toString());

      for (const auto &image : mImages) {

        tl::Path image_path(image.path().toStdString());

        for (const auto &colmap_image : database.ReadAllImages()) {
          tl::Path colmap_image_path(colmap_image.Name());

          if (image_path.equivalent(colmap_image_path)) {
            mGraphosToColmapId[image.id()] = colmap_image.ImageId();
            break;
          }
        }

      }
    }


    tl::TemporalDir temp_dir;
    tl::Path temp_path = temp_dir.path();

    temporalReconstruction(temp_path);

    if (status() == tl::Task::Status::stopping) return;

    /// Triangulación y ajuste de haces
    {

      mOutputPath.createDirectories();

      bool clear_points = false;

      colmap::OptionManager options;
      options.AddDatabaseOptions();
      options.AddImageOptions();
      options.AddRequiredOption("input_path", &temp_path.toString());
      options.AddRequiredOption("reconstruction_path", &mOutputPath.toString());
      options.AddDefaultOption("clear_points", &clear_points, "Whether to clear all existing points and observations");
      options.AddMapperOptions();

      if (!mOutputPath.exists()) {
        throw std::runtime_error(std::string("ERROR: 'reconstruction_path' is not a directory"));
      }

      auto mapper_options = options.mapper;

      msgInfo("Loading model");

      colmap::Reconstruction reconstruction;
      reconstruction.Read(temp_path.toString());

      msgInfo("Loading database");

      colmap::DatabaseCache database_cache;

      {
        tl::Chrono timer("Elapsed time:");
        timer.run();

        colmap::Database database(mDatabase.toString());

        size_t min_num_matches = static_cast<size_t>(mapper_options->min_num_matches);
        database_cache.Load(database, min_num_matches,
                            mapper_options->ignore_watermarks,
                            mapper_options->image_names);

        if (clear_points) {
          reconstruction.DeleteAllPoints2DAndPoints3D();
          reconstruction.TranscribeImageIdsToDatabase(database);
        }

        std::cout << std::endl;
        timer.stop();
      }

      std::cout << std::endl;

      if (status() == tl::Task::Status::stopping) return;

      TL_ASSERT(reconstruction.NumRegImages() >= 2, "Need at least two images for triangulation");

      colmap::IncrementalMapper mapper(&database_cache);
      mapper.BeginReconstruction(&reconstruction);

      //////////////////////////////////////////////////////////////////////////////
      // Triangulation
      //////////////////////////////////////////////////////////////////////////////

      auto triangulation_options = mapper_options->Triangulation();

      const auto &reg_image_ids = reconstruction.RegImageIds();

      for (size_t i = 0; i < reg_image_ids.size(); ++i) {

        if (status() == tl::Task::Status::stopping) return;

        const colmap::image_t image_id = reg_image_ids[i];

        const auto &image = reconstruction.Image(image_id);

        const size_t num_existing_points3D = image.NumPoints3D();

        std::cout << "  => Image sees " << num_existing_points3D << " / "
          << image.NumObservations() << " points" << std::endl;

        mapper.TriangulateImage(triangulation_options, image_id);

        std::cout << "  => Triangulated "
          << (image.NumPoints3D() - num_existing_points3D) << " points"
          << std::endl;
      }

      //////////////////////////////////////////////////////////////////////////////
      // Retriangulation
      //////////////////////////////////////////////////////////////////////////////

      msgInfo("Retriangulation");

      CompleteAndMergeTracks(*mapper_options, &mapper);

      //////////////////////////////////////////////////////////////////////////////
      // Bundle adjustment
      //////////////////////////////////////////////////////////////////////////////

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

      if (status() == tl::Task::Status::stopping) return;

      for (int i = 0; i < mapper_options->ba_global_max_refinements; ++i) {
        // Avoid degeneracies in bundle adjustment.
        reconstruction.FilterObservationsWithNegativeDepth();

        const size_t num_observations = reconstruction.ComputeNumObservations();

        colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
        TL_ASSERT(bundle_adjuster.Solve(&reconstruction), "Bundle adjust error");

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

      if (status() == tl::Task::Status::stopping) return;

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

      msgInfo("Extracting colors");
      reconstruction.ExtractColorsForAllImages("");

      const bool kDiscardReconstruction = false;
      mapper.EndReconstruction(kDiscardReconstruction);

      ColmapReconstructionConvert convert(&reconstruction, mImages);
      std::vector<GroundPoint> ground_points = convert.groundPoints();
      auto gp_writer = GroundPointsWriterFactory::create("GRAPHOS");
      gp_writer->setGroundPoints(ground_points);
      tl::Path ground_points_path(mOutputPath);
      ground_points_path.append("ground_points.bin");
      gp_writer->write(ground_points_path);

      // Write Camera Poses

      auto camera_poses = convert.cameraPoses();
      auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
      poses_writer->setCameraPoses(camera_poses);
      tl::Path poses_path(mOutputPath);
      poses_path.append("poses.bin");
      poses_writer->write(poses_path);

      if (status() == tl::Task::Status::stopping) return;

      for (auto &camera : mCameras) {

        std::shared_ptr<Calibration> calibration = convert.readCalibration(camera.first);

        if (calibration) {
          camera.second.setCalibration(calibration);
        }
      }

      /// Write Sparse Cloud

      tl::Path sparse_path(mOutputPath);
      sparse_path.append("sparse.ply");
      OrientationExport orientationExport(&reconstruction);
      orientationExport.exportPLY(QString::fromStdString(sparse_path.toString()));

      /// writeOffset
      tl::Path offset_path(mOutputPath);
      offset_path.append("offset.txt");
      std::ofstream stream(offset_path.toString(), std::ios::trunc);
      if (stream.is_open()) {
        stream << QString::number(mOffset.x, 'f', 6).toStdString() << " "
               << QString::number(mOffset.y, 'f', 6).toStdString() << " "
               << QString::number(mOffset.z, 'f', 6).toStdString() << std::endl;

        msgInfo("Camera offset: %lf,%lf,%lf", mOffset.x, mOffset.y, mOffset.z);

        stream.close();
      }

    }

    chrono.stop();

    if (progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Import orientation error");
  }

}

void ImportPosesTask::computeOffset()
{
  int i = 1;
  for (const auto &image : mImages) {
    CameraPose camera_pose = image.cameraPose();
    if (camera_pose.isEmpty()) continue;
    mOffset += (camera_pose.position() - mOffset) / i;
    i++;
  }
}

void ImportPosesTask::temporalReconstruction(const tl::Path &tempPath)
{
  try {

    writeImages(tempPath);
    
    if (status() == tl::Task::Status::stopping) return;

    writeCameras(tempPath);
    
    if (status() == tl::Task::Status::stopping) return;

    writePoints(tempPath);

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}

void ImportPosesTask::writeImages(const tl::Path &tempPath)
{
  try {

    tl::Path images_path(tempPath);
    images_path.append("images.txt");

    std::ofstream ofs;
    ofs.open(images_path.toString(), std::ofstream::out | std::ofstream::trunc);

    if (!ofs.is_open()) throw std::runtime_error(std::string("Open fail: images.txt"));

    for (const auto &image : mImages) {

      CameraPose camera_pose = image.cameraPose();
      if (camera_pose.isEmpty()) {
        continue; /// Se saltan las imagenes no orientadas
      }

      tl::math::Quaternion<double> quaternion = camera_pose.quaternion();
      std::string file_name = image.path().toStdString();
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

      ofs << std::fixed << mGraphosToColmapId[image.id()] << " " << QString::number(quaternion.w, 'g', 10).toStdString() << " "
        << QString::number(quaternion.x, 'g', 10).toStdString() << " "
        << QString::number(quaternion.y, 'g', 10).toStdString() << " "
        << QString::number(quaternion.z, 'g', 10).toStdString() << " "
        << QString::number(vector_camera_position[0], 'g', 10).toStdString() << " "
        << QString::number(vector_camera_position[1], 'g', 10).toStdString() << " "
        << QString::number(vector_camera_position[2], 'g', 10).toStdString() << " 1 " << file_name << std::endl;
      ofs << std::endl;

    }

    ofs.close();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}

void ImportPosesTask::writeCameras(const tl::Path &tempPath)
{
  try {

    tl::Path cameras_path(tempPath);
    cameras_path.append("cameras.txt");

    std::ofstream ofs;
    ofs.open(cameras_path.toString(), std::ofstream::out | std::ofstream::trunc);

    if (!ofs.is_open()) throw std::runtime_error(std::string("Open fail: cameras.txt"));

    ofs << "# Camera list with one line of data per camera: \n";
    ofs << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n";
    ofs << "# Number of cameras: " << mCameras.size() << "\n";

    for (const auto &_camera : mCameras) {

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

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

}

void ImportPosesTask::writePoints(const tl::Path &tempPath)
{
  tl::Path points3d_path(tempPath);
  points3d_path.append("points3D.txt");

  std::ofstream ofs;
  ofs.open(points3d_path.toString(), std::ofstream::out | std::ofstream::trunc);
  ofs.close();
}

bool ImportPosesTask::isCoordinatesLocal() const
{
  bool local = true;

  for (const auto &image : mImages) {
    CameraPose camera_pose = image.cameraPose();
    if (camera_pose.crs() != "") local = false;
  }

  return local;
}

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
