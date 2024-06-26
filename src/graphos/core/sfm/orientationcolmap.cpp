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

#include <tidop/core/msg/message.h>
#include <tidop/core/path.h>
#include <tidop/core/chrono.h>
#include <tidop/core/progress.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/rotation_convert.h>

#include <colmap/base/reconstruction.h>
#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/controllers/hierarchical_mapper.h>
#include <colmap/base/camera_models.h>
#include <colmap/base/cost_functions.h>


std::atomic<bool> ba_terminate(false);

namespace internal
{


struct PositionError {
  explicit PositionError(const Eigen::Vector3d& translation)
      : translation_(translation) {}

  template <typename T>
  bool operator()(const T* const cam_translation, T* residuals) const {
    residuals[0] = cam_translation[0] - T(translation_[0]);
    residuals[1] = cam_translation[1] - T(translation_[1]);
    residuals[2] = cam_translation[2] - T(translation_[2]);
    return true;
  }

  static ceres::CostFunction* Create(const Eigen::Vector3d& translation) {
    return new ceres::AutoDiffCostFunction<PositionError, 3, 3>(
        new PositionError(translation));
  }

  Eigen::Vector3d translation_;
};


// Extraido de Colmap para añadir los puntos de control

class _BundleAdjustmentConfig
    : public colmap::BundleAdjustmentConfig
{
private:

    std::unordered_map<colmap::image_t, double> cam_position_errors_;

public:

    _BundleAdjustmentConfig() : colmap::BundleAdjustmentConfig()
    {
    }

    void SetCamPositionError(colmap::image_t image_id, double position_error)
    {
        cam_position_errors_[image_id] = position_error;
    }

    double GetCamPositionError(colmap::image_t image_id) const
    {
        auto it = cam_position_errors_.find(image_id);
        return (it != cam_position_errors_.end()) ? it->second : std::numeric_limits<double>::max();
    }
};


class BundleAdjuster
{
public:
    BundleAdjuster(const colmap::BundleAdjustmentOptions &options,
                   const _BundleAdjustmentConfig &config);

    bool Solve(colmap::Reconstruction *reconstruction);

    // Get the Ceres solver summary for the last call to `Solve`.
    const ceres::Solver::Summary &Summary() const;

private:
    void SetUp(colmap::Reconstruction *reconstruction,
               ceres::LossFunction *loss_function);
    void TearDown(colmap::Reconstruction *reconstruction);

    void AddImageToProblem(const colmap::image_t image_id, colmap::Reconstruction *reconstruction,
                           ceres::LossFunction *loss_function);

    void AddPointToProblem(const colmap::point3D_t point3D_id,
                           colmap::Reconstruction *reconstruction,
                           ceres::LossFunction *loss_function);

protected:
    void ParameterizeCameras(colmap::Reconstruction *reconstruction);
    void ParameterizePoints(colmap::Reconstruction *reconstruction);

    const colmap::BundleAdjustmentOptions options_;
    _BundleAdjustmentConfig config_;
    std::unique_ptr<ceres::Problem> problem_;
    ceres::Solver::Summary summary_;
    std::unordered_set<colmap::camera_t> camera_ids_;
    std::unordered_map<colmap::point3D_t, size_t> point3D_num_observations_;
};


////////////////////////////////////////////////////////////////////////////////
// BundleAdjuster
////////////////////////////////////////////////////////////////////////////////

BundleAdjuster::BundleAdjuster(const colmap::BundleAdjustmentOptions& options,
                               const _BundleAdjustmentConfig& config)
    : options_(options), config_(config)
{
    CHECK(options_.Check());
}

bool BundleAdjuster::Solve(colmap::Reconstruction* reconstruction) {
  CHECK_NOTNULL(reconstruction);
  CHECK(!problem_) << "Cannot use the same BundleAdjuster multiple times";

  problem_.reset(new ceres::Problem());

  ceres::LossFunction* loss_function = options_.CreateLossFunction();
  SetUp(reconstruction, loss_function);

  if (problem_->NumResiduals() == 0) {
    return false;
  }

  ceres::Solver::Options solver_options = options_.solver_options;
  const bool has_sparse =
      solver_options.sparse_linear_algebra_library_type != ceres::NO_SPARSE;

  // Empirical choice.
  const size_t kMaxNumImagesDirectDenseSolver = 50;
  const size_t kMaxNumImagesDirectSparseSolver = 1000;
  const size_t num_images = config_.NumImages();
  if (num_images <= kMaxNumImagesDirectDenseSolver) {
    solver_options.linear_solver_type = ceres::DENSE_SCHUR;
  } else if (num_images <= kMaxNumImagesDirectSparseSolver && has_sparse) {
    solver_options.linear_solver_type = ceres::SPARSE_SCHUR;
  } else {  // Indirect sparse (preconditioned CG) solver.
    solver_options.linear_solver_type = ceres::ITERATIVE_SCHUR;
    solver_options.preconditioner_type = ceres::SCHUR_JACOBI;
  }

  if (problem_->NumResiduals() <
      options_.min_num_residuals_for_multi_threading) {
    solver_options.num_threads = 1;
#if CERES_VERSION_MAJOR < 2
    solver_options.num_linear_solver_threads = 1;
#endif  // CERES_VERSION_MAJOR
  } else {
    solver_options.num_threads =
        colmap::GetEffectiveNumThreads(solver_options.num_threads);
#if CERES_VERSION_MAJOR < 2
    solver_options.num_linear_solver_threads =
        GetEffectiveNumThreads(solver_options.num_linear_solver_threads);
#endif  // CERES_VERSION_MAJOR
  }

  std::string solver_error;
  CHECK(solver_options.IsValid(&solver_error)) << solver_error;

  ceres::Solve(solver_options, problem_.get(), &summary_);

  if (solver_options.minimizer_progress_to_stdout) {
    std::cout << std::endl;
  }

  if (options_.print_summary) {
    colmap::PrintHeading2("Bundle adjustment report");
    colmap::PrintSolverSummary(summary_);
  }

  TearDown(reconstruction);

  return true;
}

const ceres::Solver::Summary& BundleAdjuster::Summary() const {
  return summary_;
}

void BundleAdjuster::SetUp(colmap::Reconstruction* reconstruction,
                           ceres::LossFunction* loss_function) {
  // Warning: AddPointsToProblem assumes that AddImageToProblem is called first.
  // Do not change order of instructions!
  for (const colmap::image_t image_id : config_.Images()) {
    AddImageToProblem(image_id, reconstruction, loss_function);
  }
  for (const auto point3D_id : config_.VariablePoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }
  for (const auto point3D_id : config_.ConstantPoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }

  ParameterizeCameras(reconstruction);
  ParameterizePoints(reconstruction);
}

void BundleAdjuster::TearDown(colmap::Reconstruction*) {
  // Nothing to do
}

void BundleAdjuster::AddImageToProblem(const colmap::image_t image_id,
                                       colmap::Reconstruction *reconstruction,
                                       ceres::LossFunction *loss_function)
{
    colmap::Image &image = reconstruction->Image(image_id);
    colmap::Camera &camera = reconstruction->Camera(image.CameraId());

    // CostFunction assumes unit quaternions.
    image.NormalizeQvec();

    double *qvec_data = image.Qvec().data();
    double *tvec_data = image.Tvec().data();
    double *camera_params_data = camera.ParamsData();

    const bool constant_pose =
        !options_.refine_extrinsics || config_.HasConstantPose(image_id);

    double position_error = config_.GetCamPositionError(image_id);

    // Add residuals to bundle adjustment problem.
    size_t num_observations = 0;
    for (const colmap::Point2D &point2D : image.Points2D()) {
        if (!point2D.HasPoint3D()) {
            continue;
        }

        num_observations += 1;
        point3D_num_observations_[point2D.Point3DId()] += 1;

        colmap::Point3D &point3D = reconstruction->Point3D(point2D.Point3DId());
        assert(point3D.Track().Length() > 1);

        ceres::CostFunction *cost_function = nullptr;

        if (constant_pose) {
            switch (camera.ModelId()) {
                //#define CAMERA_MODEL_CASE(CameraModel)                                 \
                //  case CameraModel::kModelId:                                          \
                //    cost_function =                                                    \
                //        BundleAdjustmentConstantPoseCostFunction<CameraModel>::Create( \
                //            image.Qvec(), image.Tvec(), point2D.XY());                 \
                //    break;

            case colmap::SimplePinholeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::SimplePinholeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::PinholeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::PinholeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::SimpleRadialCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::SimpleRadialCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::SimpleRadialFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::SimpleRadialFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::RadialCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::RadialCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::RadialFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::RadialFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::OpenCVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::OpenCVCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::OpenCVFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::OpenCVFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::FullOpenCVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::FullOpenCVCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::FOVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::FOVCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            case colmap::ThinPrismFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::ThinPrismFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
                break;
            default: throw std::domain_error("Camera model does not exist");
                break;

                //#undef CAMERA_MODEL_CASE
            }

            problem_->AddResidualBlock(cost_function, loss_function,
                                       point3D.XYZ().data(), camera_params_data);
        } else {
            switch (camera.ModelId()) {
                //#define CAMERA_MODEL_CASE(CameraModel)                                   \
                //  case CameraModel::kModelId:                                            \
                //    cost_function =                                                      \
                //        BundleAdjustmentCostFunction<CameraModel>::Create(point2D.XY()); \
                //    break;
                //
                //                CAMERA_MODEL_SWITCH_CASES
                //
                //#undef CAMERA_MODEL_CASE
            case colmap::SimplePinholeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::SimplePinholeCameraModel>::Create(point2D.XY());
                break;
            case colmap::PinholeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::PinholeCameraModel>::Create(point2D.XY());
                break;
            case colmap::SimpleRadialCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::SimpleRadialCameraModel>::Create(point2D.XY());
                break;
            case colmap::SimpleRadialFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::SimpleRadialFisheyeCameraModel>::Create(point2D.XY());
                break;
            case colmap::RadialCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::RadialCameraModel>::Create(point2D.XY());
                break;
            case colmap::RadialFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::RadialFisheyeCameraModel>::Create(point2D.XY());
                break;
            case colmap::OpenCVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::OpenCVCameraModel>::Create(point2D.XY());
                break;
            case colmap::OpenCVFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::OpenCVFisheyeCameraModel>::Create(point2D.XY());
                break;
            case colmap::FullOpenCVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::FullOpenCVCameraModel>::Create(point2D.XY());
                break;
            case colmap::FOVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::FOVCameraModel>::Create(point2D.XY());
                break;
            case colmap::ThinPrismFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentCostFunction<colmap::ThinPrismFisheyeCameraModel>::Create(point2D.XY());
                break;
            default: throw std::domain_error("Camera model does not exist"); break;
            }

            problem_->AddResidualBlock(cost_function, loss_function, qvec_data,
                                       tvec_data, point3D.XYZ().data(),
                                       camera_params_data);
        }
    }

    if (num_observations > 0) {
        camera_ids_.insert(image.CameraId());

        // Set pose parameterization.
        if (!constant_pose) {
            ceres::LocalParameterization *quaternion_parameterization =
                new ceres::QuaternionParameterization;
            problem_->SetParameterization(qvec_data, quaternion_parameterization);
            //if (config_.HasConstantTvec(image_id)) {
            //    const std::vector<int> &constant_tvec_idxs =
            //        config_.ConstantTvec(image_id);
            //    ceres::SubsetParameterization *tvec_parameterization =
            //        new ceres::SubsetParameterization(3, constant_tvec_idxs);
            //    problem_->SetParameterization(tvec_data, tvec_parameterization);
            //}
            if (position_error < std::numeric_limits<double>::max()) {
                // Crear una loss function específica para las posiciones con error
                ceres::LossFunction *position_loss_function = new ceres::ScaledLoss(nullptr, position_error, ceres::TAKE_OWNERSHIP);
                problem_->AddResidualBlock(new ceres::AutoDiffCostFunction<internal::PositionError, 3, 3>(
                    new internal::PositionError(image.Tvec())), position_loss_function, tvec_data);
            }
        }
    }
}

void BundleAdjuster::AddPointToProblem(const colmap::point3D_t point3D_id,
                                       colmap::Reconstruction* reconstruction,
                                       ceres::LossFunction* loss_function)
{
    colmap::Point3D& point3D = reconstruction->Point3D(point3D_id);

    // Is 3D point already fully contained in the problem? I.e. its entire track
    // is contained in `variable_image_ids`, `constant_image_ids`,
    // `constant_x_image_ids`.
    if (point3D_num_observations_[point3D_id] == point3D.Track().Length()) {
        return;
    }

    for (const auto &track_el : point3D.Track().Elements()) {
        // Skip observations that were already added in `FillImages`.
        if (config_.HasImage(track_el.image_id)) {
            continue;
        }

        point3D_num_observations_[point3D_id] += 1;

        colmap::Image &image = reconstruction->Image(track_el.image_id);
        colmap::Camera &camera = reconstruction->Camera(image.CameraId());
        const colmap::Point2D &point2D = image.Point2D(track_el.point2D_idx);

        // We do not want to refine the camera of images that are not
        // part of `constant_image_ids_`, `constant_image_ids_`,
        // `constant_x_image_ids_`.
        if (camera_ids_.count(image.CameraId()) == 0) {
            camera_ids_.insert(image.CameraId());
            config_.SetConstantCamera(image.CameraId());
        }

        ceres::CostFunction *cost_function = nullptr;

        switch (camera.ModelId()) {
            //#define CAMERA_MODEL_CASE(CameraModel)                                 \
            //  case CameraModel::kModelId:                                          \
            //    cost_function =                                                    \
            //        BundleAdjustmentConstantPoseCostFunction<CameraModel>::Create( \
            //            image.Qvec(), image.Tvec(), point2D.XY());                 \
            //    break;
            //
            //      CAMERA_MODEL_SWITCH_CASES
            //
            //#undef CAMERA_MODEL_CASE
        case colmap::SimplePinholeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::SimplePinholeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::PinholeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::PinholeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::SimpleRadialCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::SimpleRadialCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::SimpleRadialFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::SimpleRadialFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::RadialCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::RadialCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::RadialFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::RadialFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::OpenCVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::OpenCVCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::OpenCVFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::OpenCVFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::FullOpenCVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::FullOpenCVCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::FOVCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::FOVCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        case colmap::ThinPrismFisheyeCameraModel::kModelId: cost_function = colmap::BundleAdjustmentConstantPoseCostFunction<colmap::ThinPrismFisheyeCameraModel>::Create(image.Qvec(), image.Tvec(), point2D.XY());
            break;
        default: throw std::domain_error("Camera model does not exist");
            break;
        }
        problem_->AddResidualBlock(cost_function, loss_function,
            point3D.XYZ().data(), camera.ParamsData());
    }
}

void BundleAdjuster::ParameterizeCameras(colmap::Reconstruction *reconstruction)
{
    const bool constant_camera = !options_.refine_focal_length &&
        !options_.refine_principal_point &&
        !options_.refine_extra_params;
    for (const colmap::camera_t camera_id : camera_ids_) {
        colmap::Camera &camera = reconstruction->Camera(camera_id);

        if (constant_camera || config_.IsConstantCamera(camera_id)) {
            problem_->SetParameterBlockConstant(camera.ParamsData());
            continue;
        } else {
            std::vector<int> const_camera_params;

            if (!options_.refine_focal_length) {
                const std::vector<size_t> &params_idxs = camera.FocalLengthIdxs();
                const_camera_params.insert(const_camera_params.end(),
                    params_idxs.begin(), params_idxs.end());
            }
            if (!options_.refine_principal_point) {
                const std::vector<size_t> &params_idxs = camera.PrincipalPointIdxs();
                const_camera_params.insert(const_camera_params.end(),
                    params_idxs.begin(), params_idxs.end());
            }
            if (!options_.refine_extra_params) {
                const std::vector<size_t> &params_idxs = camera.ExtraParamsIdxs();
                const_camera_params.insert(const_camera_params.end(),
                    params_idxs.begin(), params_idxs.end());
            }

            if (const_camera_params.size() > 0) {
                ceres::SubsetParameterization *camera_params_parameterization =
                    new ceres::SubsetParameterization(
                        static_cast<int>(camera.NumParams()), const_camera_params);
                problem_->SetParameterization(camera.ParamsData(),
                    camera_params_parameterization);
            }
        }
    }
}

void BundleAdjuster::ParameterizePoints(colmap::Reconstruction *reconstruction)
{
    for (const auto elem : point3D_num_observations_) {
        colmap::Point3D &point3D = reconstruction->Point3D(elem.first);
        if (point3D.Track().Length() > elem.second) {
            problem_->SetParameterBlockConstant(point3D.XYZ().data());
        }
    }

    for (const colmap::point3D_t point3D_id : config_.ConstantPoints()) {
        colmap::Point3D &point3D = reconstruction->Point3D(point3D_id);
        problem_->SetParameterBlockConstant(point3D.XYZ().data());
    }
}






// Callback functor called after each bundle adjustment iteration.
class BundleAdjustmentIterationCallback
    : public ceres::IterationCallback 
{

public:

    explicit BundleAdjustmentIterationCallback() {}

    virtual ceres::CallbackReturnType operator()(
        const ceres::IterationSummary& summary) {

        if (ba_terminate) {
            return ceres::SOLVER_TERMINATE_SUCCESSFULLY;
        } else {
            return ceres::SOLVER_CONTINUE;
        }
    }

};

}


namespace graphos
{


ColmapReconstructionConvert::ColmapReconstructionConvert(const colmap::Reconstruction *reconstruction, 
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

auto ColmapReconstructionConvert::groundPoints() const -> std::vector<GroundPoint>
{
    std::vector<GroundPoint> ground_points(mReconstruction->NumPoints3D());

    size_t i = 0;

    for (auto &points_3d : mReconstruction->Points3D()) {

        GroundPoint ground_point;
        ground_point.setPoint(tl::Point3<double>(points_3d.second.X(),
                              points_3d.second.Y(),
                              points_3d.second.Z()));

        tl::ColorRGB rgb(points_3d.second.Color(0),
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

auto ColmapReconstructionConvert::cameraPoses() const -> std::unordered_map<size_t, CameraPose>
{
    std::unordered_map<size_t, CameraPose> camera_poses;

    for (auto &pair : mReconstruction->Images()) {

        CameraPose photoOrientation;

        colmap::Image colmap_image = pair.second;
        tl::Path colmap_image_path(colmap_image.Name());

        const Eigen::Matrix<double, 3, 4> inv_proj_matrix = colmap_image.InverseProjectionMatrix();
        const Eigen::Vector3d pc = inv_proj_matrix.rightCols<1>();
        photoOrientation.setPosition(tl::Point3<double>(pc(0), pc(1), pc(2)));

        //auto &qvec = colmap_image.Qvec();
        //Eigen::Vector4d normalized_qvec = colmap::NormalizeQuaternion(qvec);

        Eigen::Matrix3d rot = colmap_image.RotationMatrix();
        tl::RotationMatrix<double> rotation_matrix;
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

auto ColmapReconstructionConvert::readCalibration(size_t cameraId) const -> std::shared_ptr<Calibration>
{
    std::shared_ptr<Calibration> calibration;
    if (mReconstruction->ExistsCamera(static_cast<colmap::image_t>(cameraId))) {

        const colmap::Camera &camera = mReconstruction->Camera(static_cast<colmap::image_t>(cameraId));
        std::vector<double> params = camera.Params();

        std::string model_name = camera.ModelName();

        if (model_name == "SIMPLE_PINHOLE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::simple_pinhole);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);

        } else if (model_name == "PINHOLE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::pinhole);
            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
            calibration->setParameter(Calibration::Parameters::cx, params[2]);
            calibration->setParameter(Calibration::Parameters::cy, params[3]);

        } else if (model_name == "SIMPLE_RADIAL") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::radial1);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);

        } else if (model_name == "RADIAL") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::radial2);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);
            calibration->setParameter(Calibration::Parameters::k2, params[4]);

        } else if (model_name == "OPENCV") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
            calibration->setParameter(Calibration::Parameters::cx, params[2]);
            calibration->setParameter(Calibration::Parameters::cy, params[3]);
            calibration->setParameter(Calibration::Parameters::k1, params[4]);
            calibration->setParameter(Calibration::Parameters::k2, params[5]);
            calibration->setParameter(Calibration::Parameters::p1, params[6]);
            calibration->setParameter(Calibration::Parameters::p2, params[7]);

        } else if (model_name == "OPENCV_FISHEYE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
            calibration->setParameter(Calibration::Parameters::focalx, params[0]);
            calibration->setParameter(Calibration::Parameters::focaly, params[1]);
            calibration->setParameter(Calibration::Parameters::cx, params[2]);
            calibration->setParameter(Calibration::Parameters::cy, params[3]);
            calibration->setParameter(Calibration::Parameters::k1, params[4]);
            calibration->setParameter(Calibration::Parameters::k2, params[5]);
            calibration->setParameter(Calibration::Parameters::k3, params[6]);
            calibration->setParameter(Calibration::Parameters::k4, params[7]);

        } else if (model_name == "FULL_OPENCV") {

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

        } else if (model_name == "SIMPLE_RADIAL_FISHEYE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::simple_radial_fisheye);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);

        } else if (model_name == "RADIAL_FISHEYE") {

            calibration = CalibrationFactory::create(Calibration::CameraModel::radial_fisheye);
            calibration->setParameter(Calibration::Parameters::focal, params[0]);
            calibration->setParameter(Calibration::Parameters::cx, params[1]);
            calibration->setParameter(Calibration::Parameters::cy, params[2]);
            calibration->setParameter(Calibration::Parameters::k1, params[3]);
            calibration->setParameter(Calibration::Parameters::k2, params[4]);

        } else if (model_name == "FULL_RADIAL") {

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


RelativeOrientationColmapProperties::RelativeOrientationColmapProperties()
  : mRefineFocalLength(true),
    mRefinePrincipalPoint(false),
    mRefineExtraParams(true)
{

}

auto RelativeOrientationColmapProperties::refineFocalLength() const -> bool
{
    return mRefineFocalLength;
}

void RelativeOrientationColmapProperties::setRefineFocalLength(bool refineFocalLength)
{
    mRefineFocalLength = refineFocalLength;
}

auto RelativeOrientationColmapProperties::refinePrincipalPoint() const -> bool
{
    return mRefinePrincipalPoint;
}

void RelativeOrientationColmapProperties::setRefinePrincipalPoint(bool refinePrincipalPoint)
{
    mRefinePrincipalPoint = refinePrincipalPoint;
}

auto RelativeOrientationColmapProperties::refineExtraParams() const -> bool
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

auto RelativeOrientationColmapProperties::name() const -> QString
{
    return QString("Colmap");
}







RelativeOrientationColmapTask::RelativeOrientationColmapTask(tl::Path database,
                                                             tl::Path outputPath,
                                                             const std::vector<Image> &images,
                                                             const std::map<int, Camera> &cameras,
                                                             bool fixCalibration)
  : mDatabase(std::move(database)),
    mOutputPath(std::move(outputPath)),
    mImages(images),
    mCameras(cameras),
    mIncrementalMapper(new colmap::IncrementalMapperOptions),
    mMapper(nullptr),
    mReconstructionManager(new colmap::ReconstructionManager)
{
    RelativeOrientationColmapProperties::setRefineFocalLength(!fixCalibration);
    RelativeOrientationColmapProperties::setRefinePrincipalPoint(!fixCalibration);
    RelativeOrientationColmapProperties::setRefineExtraParams(!fixCalibration);
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

    mReconstructionManager->Clear();
    mReconstructionManager.reset();
}

auto RelativeOrientationColmapTask::cameras() const -> std::map<int, Camera>
{
    return mCameras;
}

auto RelativeOrientationColmapTask::report() const -> OrientationReport
{
    return mOrientationReport;
}

void RelativeOrientationColmapTask::stop()
{
    TaskBase::stop();

    if (mMapper && mMapper->IsRunning())
        mMapper->Stop();

    ba_terminate = true;
}

/// Comprobar que funcione el ajuste de haces extraido de Colmap y sacarlo a un fichero a parte para poder usarlo desde la herramienta de georeferencia
///
void RelativeOrientationColmapTask::execute(tl::Progress *progressBar)
{
    try {


        mReconstructionManager->Clear();

        if (!mOutputPath.exists() && !mOutputPath.createDirectories()) {
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
                    tl::printException(e);
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

        colmap::OptionManager option_manager;
        option_manager.bundle_adjustment->refine_focal_length = refineFocalLength();
        option_manager.bundle_adjustment->refine_principal_point = refinePrincipalPoint();
        option_manager.bundle_adjustment->refine_extra_params = refineExtraParams();

        colmap::Reconstruction &reconstruction = mReconstructionManager->Get(0);

        //mBundleAdjustmentController = new colmap::BundleAdjustmentController(optionManager, &reconstruction);

        //mBundleAdjustmentController->Start();
        //mBundleAdjustmentController->Wait();

        const std::vector<colmap::image_t>& reg_image_ids = reconstruction.RegImageIds();

        TL_ASSERT(reg_image_ids.size() >= 2, "Need at least two views.");

        // Avoid degeneracies in bundle adjustment.
        reconstruction.FilterObservationsWithNegativeDepth();

        colmap::BundleAdjustmentOptions ba_options = *option_manager.bundle_adjustment;
        ba_options.solver_options.minimizer_progress_to_stdout = true;
        ba_options.solver_options.logging_type = ceres::LoggingType::SILENT;

        internal::BundleAdjustmentIterationCallback iteration_callback;
        ba_options.solver_options.callbacks.push_back(&iteration_callback);

        // Configure bundle adjustment.
        colmap::BundleAdjustmentConfig ba_config;
        for (const colmap::image_t image_id : reg_image_ids) {
            ba_config.AddImage(image_id);
        }
        ba_config.SetConstantPose(reg_image_ids[0]);
        ba_config.SetConstantTvec(reg_image_ids[1], { 0 });

        ceres::Solver::Summary summary;

        {
            ba_terminate = false;
            // Run bundle adjustment.
            colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
            bundle_adjuster.Solve(&reconstruction);

            if (status() == tl::Task::Status::stopping) return;

            ba_terminate = true;
            summary = bundle_adjuster.Summary();
        }

        

        if (summary.termination_type == ceres::NO_CONVERGENCE) {

            ba_terminate = false;

            // Se vuelve a intentar el ajuste de haces
            colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
            bundle_adjuster.Solve(&reconstruction);

            if (status() == tl::Task::Status::stopping) return;

            ba_terminate = true;
            summary = bundle_adjuster.Summary();
        }

        mOrientationReport.iterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
        mOrientationReport.initialCost = std::sqrt(summary.initial_cost / summary.num_residuals_reduced);
        mOrientationReport.finalCost = std::sqrt(summary.final_cost / summary.num_residuals_reduced);
        mOrientationReport.termination = "CONVERGENCE";

        TL_ASSERT(summary.termination_type == ceres::CONVERGENCE, "Bundle adjust: NO CONVERGENCE");

        if (status() == Status::stopping) return;

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
        orientationExport.exportBinary(mOutputPath); // TODO: Por ahora lo guardo y lo borro al finalizar
        tl::Path sparse_file = mOutputPath;
        sparse_file.append("sparse.ply");
        orientationExport.exportPLY(sparse_file);

        if (status() == Status::stopping) return;

        tl::Message::success("Relative orientation finished in {:.2} minutes", time() / 60.);

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Relative Orientation error");
    }
}






constexpr auto absolute_orientation_colmap_min_common_images = 3;
constexpr auto absolute_orientation_colmap_robust_alignment = true;
constexpr auto absolute_orientation_colmap_robust_alignment_max_error = 1.;

AbsoluteOrientationColmapProperties::AbsoluteOrientationColmapProperties()
  : mMinCommonImages(absolute_orientation_colmap_min_common_images),
    mRobustAlignment(absolute_orientation_colmap_robust_alignment),
    mRobustAlignmentMaxError(absolute_orientation_colmap_robust_alignment_max_error)
{
}

auto AbsoluteOrientationColmapProperties::minCommonImages() const -> int
{
  return mMinCommonImages;
}

void AbsoluteOrientationColmapProperties::setMinCommonImages(int minCommonImages)
{
  mMinCommonImages = minCommonImages;
}

auto AbsoluteOrientationColmapProperties::robustAlignment() const -> bool
{
  return mRobustAlignment;
}

void AbsoluteOrientationColmapProperties::setRobustAlignment(bool robustAlignment)
{
  mRobustAlignment = robustAlignment;
}

auto AbsoluteOrientationColmapProperties::robustAlignmentMaxError() const -> double
{
  return mRobustAlignmentMaxError;
}

void AbsoluteOrientationColmapProperties::setRobustAlignmentMaxError(double robustAlignmentMaxError)
{
    mRobustAlignmentMaxError = robustAlignmentMaxError;
}

void AbsoluteOrientationColmapProperties::reset()
{
    mMinCommonImages = absolute_orientation_colmap_min_common_images;
    mRobustAlignment = absolute_orientation_colmap_robust_alignment;
    mRobustAlignmentMaxError = absolute_orientation_colmap_robust_alignment_max_error;
}

auto AbsoluteOrientationColmapProperties::name() const -> QString
{
  return QString("Colmap");
}








AbsoluteOrientationColmapTask::AbsoluteOrientationColmapTask(tl::Path inputPath,
                                                             const std::vector<Image> &images)
  : mInputPath(std::move(inputPath)),
    mImages(images)
{
}

AbsoluteOrientationColmapTask::~AbsoluteOrientationColmapTask() = default;

auto AbsoluteOrientationColmapTask::cameraPosesErrors() const -> std::unordered_map<size_t, double>
{
    return mCameraPosesErrors;
}

void AbsoluteOrientationColmapTask::stop()
{
    TaskBase::stop();
}


/// Hay que comprobar si existen puntos de control. En ese caso se da prioridad al uso de los puntos de control.
/// Ver la herramienta de georeferencia.
void AbsoluteOrientationColmapTask::execute(tl::Progress *progressBar)
{
    try {

        bool robust_alignment = robustAlignment();
        colmap::RANSACOptions ransac_options;
        ransac_options.max_error = robustAlignmentMaxError();

        TL_ASSERT(mInputPath.exists() && mInputPath.isDirectory(), "Invalid reconstruction");
        
        auto cameras_colmap = tl::Path(mInputPath).append("cameras.bin");
        auto images_colmap = tl::Path(mInputPath).append("images.bin");
        auto points3d_colmap = tl::Path(mInputPath).append("points3D.bin");

        TL_ASSERT(cameras_colmap.exists() && images_colmap.exists() && points3d_colmap.exists(), "Invalid reconstruction");

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

        if (status() == Status::stopping) return;

        tl::Path offset_path = mInputPath;
        offset_path.append("offset.txt");

        for (auto &pos : ref_locations) {
            pos -= offset;
        }

        TL_ASSERT(mInputPath.exists(), "Reconstruction not found in path: {}", mInputPath.toString());

        if (status() == Status::stopping) return;

        bool alignment_success;
        if (robust_alignment) {
            alignment_success = reconstruction.AlignRobust(
                ref_image_names, ref_locations, minCommonImages(), ransac_options);
        } else {
            alignment_success = reconstruction.Align(ref_image_names, ref_locations, minCommonImages());
        }

        if (status() == Status::stopping) return;

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
        orientationExport.exportPLY(sparse_path);


        /// writeOffset
        
        offsetWrite(offset_path, tl::Point3<double>(offset[0], offset[1], offset[2]));
        tl::Message::info("Camera offset: {},{},{}", offset[0], offset[1], offset[2]);


        std::vector<double> errors;
        errors.reserve(ref_image_names.size());

        for (size_t i = 0; i < ref_image_names.size(); ++i) {
            const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
            if (image != nullptr) {
                double error = (image->ProjectionCenter() - ref_locations[i]).norm();
                errors.push_back(error);
            }
        }

        tl::Message::info("Alignment error: {} (mean), {} (median)",
                          colmap::Mean(errors), colmap::Median(errors));

        if (status() == Status::stopping) return;

        tl::Message::success("Absolute orientation finished in {:.2} minutes", time() / 60.);

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Absolute Orientation error");
    }
}





ImportPosesTask::ImportPosesTask(const std::vector<Image> &images,
                                 const std::map<int, Camera> &cameras,
                                 tl::Path outputPath,
                                 tl::Path database,
                                 bool fixCalibration,
                                 bool fixPoses)
  : mImages(images),
    mCameras(cameras),
    mOutputPath(std::move(outputPath)),
    mDatabase(std::move(database)),
    mFixCalibration(fixCalibration),
    mFixPoses(fixPoses)
{
    computeOffset();
}

ImportPosesTask::~ImportPosesTask() = default;

auto ImportPosesTask::cameras() const -> std::map<int, Camera>
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
                        mGraphosToColmapId[image.id()] = static_cast<int>(colmap_image.ImageId());
                        break;
                    }
                }

            }

            database.Close();
        }


        tl::TemporalDir temp_dir;
        tl::Path temp_path = temp_dir.path();

        temporalReconstruction(temp_path);

        if (status() == Status::stopping) return;

        /// Triangulación y ajuste de haces
        {

            mOutputPath.createDirectories();

            bool clear_points = false;

            std::string input_path = temp_path.toString();
            std::string reconstruction_path = mOutputPath.toString();

            colmap::OptionManager options;
            options.AddDatabaseOptions();
            options.AddImageOptions();
            options.AddRequiredOption("input_path", &input_path);
            options.AddRequiredOption("reconstruction_path", &reconstruction_path);
            options.AddDefaultOption("clear_points", &clear_points, "Whether to clear all existing points and observations");
            options.AddMapperOptions();

            TL_ASSERT(mOutputPath.exists(), "ERROR: 'reconstruction_path' is not a directory");

            auto &mapper_options = options.mapper;

            tl::Message::info("Loading model");

            colmap::Reconstruction reconstruction;
            reconstruction.Read(temp_path.toString());

            for (const auto &image : mImages) {

                CameraPose camera_pose = image.cameraPose();
                if (camera_pose.isEmpty()) {
                    continue; /// Se saltan las imagenes no orientadas
                }

                size_t image_id = image.id();
                reconstruction.Image(mGraphosToColmapId[image_id]).Name() = image.path().toStdString();

            }

            tl::Message::info("Loading database");

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

                database.Close();

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

            for (unsigned int image_id : reg_image_ids)
            {

                if (status() == Status::stopping) return;

                const auto &image = reconstruction.Image(image_id);

                const size_t num_existing_points3D = image.NumPoints3D();

                std::cout << "  => Image sees " << num_existing_points3D << " / "
                          << image.NumObservations() << " points" << '\n';

                mapper.TriangulateImage(triangulation_options, image_id);

                std::cout << "  => Triangulated "
                          << (image.NumPoints3D() - num_existing_points3D) << " points"
                          << std::endl;
            }

            //////////////////////////////////////////////////////////////////////////////
            // Retriangulation
            //////////////////////////////////////////////////////////////////////////////

            tl::Message::info("Retriangulation");

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

            if (status() == Status::stopping) return;

            for (int i = 0; i < mapper_options->ba_global_max_refinements; ++i) {
                // Avoid degeneracies in bundle adjustment.
                reconstruction.FilterObservationsWithNegativeDepth();

                size_t num_observations = reconstruction.ComputeNumObservations();

                colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
                TL_ASSERT(bundle_adjuster.Solve(&reconstruction), "Bundle adjust error");

                size_t num_changed_observations = 0;
                num_changed_observations += CompleteAndMergeTracks(*mapper_options, &mapper);
                num_changed_observations += FilterPoints(*mapper_options, &mapper);
                double changed = static_cast<double>(num_changed_observations) / static_cast<double>(num_observations);
                std::cout << colmap::StringPrintf("  => Changed observations: %.6f", changed) << std::endl;
                if (changed < mapper_options->ba_global_max_refinement_change) {
                    break;
                }
            }

            if (status() == Status::stopping) return;

            // Se incluye el punto principal en el ajuste
            if (!mFixCalibration) {
                ba_options.refine_principal_point = true;
                for (int i = 0; i < mapper_options->ba_global_max_refinements; ++i) {
                    // Avoid degeneracies in bundle adjustment.
                    reconstruction.FilterObservationsWithNegativeDepth();

                    size_t num_observations = reconstruction.ComputeNumObservations();

                    //PrintHeading1("Bundle adjustment");
                    //graphos::BundleAdjuster bundle_adjuster(ba_options, ba_config);
                    colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
                    if (!bundle_adjuster.Solve(&reconstruction)) throw std::runtime_error(std::string("Reconstruction error"));

                    size_t num_changed_observations = 0;
                    num_changed_observations += CompleteAndMergeTracks(*mapper_options, &mapper);
                    num_changed_observations += FilterPoints(*mapper_options, &mapper);
                    double changed = static_cast<double>(num_changed_observations) / static_cast<double>(num_observations);
                    std::cout << colmap::StringPrintf("  => Changed observations: %.6f", changed) << std::endl;
                    if (changed < mapper_options->ba_global_max_refinement_change) {
                        break;
                    }
                }
            }

            tl::Message::info("Extracting colors");
            reconstruction.ExtractColorsForAllImages("");

            bool discard_reconstruction = false;
            mapper.EndReconstruction(discard_reconstruction);

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

            if (status() == Status::stopping) return;

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
            orientationExport.exportPLY(sparse_path);

            /// writeOffset
            tl::Path offset_path(mOutputPath);
            offset_path.append("offset.txt");
            std::ofstream stream(offset_path.toString(), std::ios::trunc);
            if (stream.is_open()) {
                stream << QString::number(mOffset.x, 'f', 6).toStdString() << " "
                       << QString::number(mOffset.y, 'f', 6).toStdString() << " "
                       << QString::number(mOffset.z, 'f', 6).toStdString() << '\n';

                tl::Message::info("Camera offset: {},{},{}", mOffset.x, mOffset.y, mOffset.z);

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

        if (status() == Status::stopping) return;

        writeCameras(tempPath);

        if (status() == Status::stopping) return;

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

            tl::Quaternion<double> quaternion = camera_pose.quaternion();
            TL_TODO("Las rutas con espacios pueden dar problemas")
            std::string file_name = image.path().toStdString();
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

            ofs << std::fixed << mGraphosToColmapId[image.id()] << " " << QString::number(quaternion.w, 'g', 10).toStdString() << " "
                << QString::number(quaternion.x, 'g', 10).toStdString() << " "
                << QString::number(quaternion.y, 'g', 10).toStdString() << " "
                << QString::number(quaternion.z, 'g', 10).toStdString() << " "
                << QString::number(vector_camera_position[0], 'g', 10).toStdString() << " "
                << QString::number(vector_camera_position[1], 'g', 10).toStdString() << " "
                << QString::number(vector_camera_position[2], 'g', 10).toStdString() << " 1 " << file_name << '\n';
            ofs << '\n';

        }

        ofs.close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void ImportPosesTask::writeCameras(const tl::Path &tempPath) const
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

            auto camera_type = cameraToColmapType(camera).toStdString();
            auto calibration = camera.calibration();

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

                if (camera_type == "FULL_RADIAL")
                    ofs << " 0.0 0.0 0.0 0.0 0.0 0.0";

            }

            ofs << '\n';
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
