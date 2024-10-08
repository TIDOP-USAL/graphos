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

#include "graphos/core/sfm/BundleAdjuster.h"

#include <colmap/util/threading.h>  // for colmap::GetEffectiveNumThreads(solver_options.num_threads);
#include <colmap/util/misc.h> // colmap::PrintHeading2("Bundle adjustment report");
#include <colmap/base/camera_models.h>

namespace graphos
{


// Clase de costo para restringir la posición de la cámara
class CameraPositionCostFunction
{

public:

    CameraPositionCostFunction(Eigen::Vector3d position, double weight)
      : mPosition(std::move(position)),
        mWeight(weight)
    {
    }

    template <typename T>
    auto operator()(const T* const tvec, T* residuals) const -> bool
    {
        residuals[0] = static_cast<T>(mWeight) * (tvec[0] - static_cast<T>(mPosition(0)));
        residuals[1] = static_cast<T>(mWeight) * (tvec[1] - static_cast<T>(mPosition(1)));
        residuals[2] = static_cast<T>(mWeight) * (tvec[2] - static_cast<T>(mPosition(2)));

        return true;
    }

    static auto create(const Eigen::Vector3d &translation, double positionWeight) -> ceres::CostFunction*
    {
        return new ceres::AutoDiffCostFunction<CameraPositionCostFunction, 3, 3>(
            new CameraPositionCostFunction(translation, positionWeight));
    }

private:

    Eigen::Vector3d mPosition;
    double mWeight;
};


// Se prueba a reemplazar por GroundControlPointCostFunction
template <typename CameraModel>
class BundleAdjustmentCostFunction
{
public:
    explicit BundleAdjustmentCostFunction(const Eigen::Vector2d &point2D, double weight = 1.)
        : observed_x_(point2D(0)), observed_y_(point2D(1)), weight_(weight)
    {
    }

    static ceres::CostFunction *Create(const Eigen::Vector2d &point2D, double weight = 1.)
    {
        return (new ceres::AutoDiffCostFunction<
            BundleAdjustmentCostFunction<CameraModel>, 2, 4, 3, 3, CameraModel::kNumParams>(
                new BundleAdjustmentCostFunction(point2D, weight)));
    }

    template <typename T>
    bool operator()(const T *const qvec, const T *const tvec,
                    const T *const point3D, const T *const camera_params,
                    T *residuals) const
    {
        // Rotate and translate.
        T projection[3];
        ceres::UnitQuaternionRotatePoint(qvec, point3D, projection);
        projection[0] += tvec[0];
        projection[1] += tvec[1];
        projection[2] += tvec[2];

        // Project to image plane.
        projection[0] /= projection[2];
        projection[1] /= projection[2];

        // Distort and transform to pixel space.
        CameraModel::WorldToImage(camera_params, projection[0], projection[1], &residuals[0], &residuals[1]);

        // Re-projection error.
        residuals[0] -= T(observed_x_);
        residuals[1] -= T(observed_y_);

        //// GRAPHOS: Se añade un peso
        if (weight_ > 1) {
            residuals[0] *= T(weight_);
            residuals[1] *= T(weight_);
        }
        //// GRAPHOS
        
        return true;
    }

private:
    const double observed_x_;
    const double observed_y_;
    const double weight_;
};

template <typename CameraModel>
class GroundControlPointCostFunction
{

public:

    GroundControlPointCostFunction(const Eigen::Vector2d &observed,
                                   const Eigen::Vector3d &gcp_position,
                                   double weight)
        : observed_(observed), gcp_position_(gcp_position), weight_(weight)
    {
    }

    template <typename T>
    bool operator()(const T *const qvec, const T *const tvec,
        const T *const camera_params, T *residuals) const
    {
        Eigen::Matrix<T, 3, 1> xyz = gcp_position_.cast<T>();

        T projection[3];
        ceres::UnitQuaternionRotatePoint(qvec, xyz.data(), projection);
        projection[0] += tvec[0];
        projection[1] += tvec[1];
        projection[2] += tvec[2];

        // Project to image plane.
        projection[0] /= projection[2];
        projection[1] /= projection[2];

        // Distort and transform to pixel space.
        CameraModel::WorldToImage(camera_params, projection[0], projection[1], &residuals[0], &residuals[1]);

        // Re-projection error.
        residuals[0] -= T(observed_(0));
        residuals[1] -= T(observed_(1));

        if (weight_ > 1) {
            residuals[0] *= T(weight_);
            residuals[1] *= T(weight_);
        }

        return true;
    }

    static ceres::CostFunction *Create(const Eigen::Vector2d &observed,
        const Eigen::Vector3d &gcp_position,
        const double weight)
    {
        return new ceres::AutoDiffCostFunction<
            GroundControlPointCostFunction, 2, 4, 3, CameraModel::kNumParams>(
                new GroundControlPointCostFunction(observed, gcp_position, weight));
    }

private:

    const Eigen::Vector2d observed_;
    const Eigen::Vector3d gcp_position_;
    const double weight_;
};


BundleAdjuster::BundleAdjuster(colmap::BundleAdjustmentOptions options,
                               BundleAdjustmentConfig config)
  : options_(std::move(options)),
    config_(std::move(config))
{
    CHECK(options_.Check());
}

bool BundleAdjuster::solve(colmap::Reconstruction *reconstruction)
{
    CHECK_NOTNULL(reconstruction);
    CHECK(!problem_) << "Cannot use the same BundleAdjuster multiple times";

    //// OpenMVG
    //ceres::LossFunction *loss_function = new ceres::HuberLoss(4.0 * 4.0);
    //ceres::Problem::Options problem_options;
    //problem_options.loss_function_ownership = ceres::DO_NOT_TAKE_OWNERSHIP;
    //problem_.reset(new ceres::Problem(problem_options));
    //// OpenMVG

    problem_.reset(new ceres::Problem());
    ceres::LossFunction *loss_function = options_.CreateLossFunction();
    
    setUp(reconstruction, loss_function);

    if (problem_->NumResiduals() == 0) {
        return false;
    }

    ceres::Solver::Options solver_options = options_.solver_options;
    const bool has_sparse = solver_options.sparse_linear_algebra_library_type != ceres::NO_SPARSE;

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

    if (problem_->NumResiduals() < options_.min_num_residuals_for_multi_threading) {
        solver_options.num_threads = 1;
#if CERES_VERSION_MAJOR < 2
        solver_options.num_linear_solver_threads = 1;
#endif  // CERES_VERSION_MAJOR
    } else {
        solver_options.num_threads = colmap::GetEffectiveNumThreads(solver_options.num_threads);
#if CERES_VERSION_MAJOR < 2
        solver_options.num_linear_solver_threads = colmap::GetEffectiveNumThreads(solver_options.num_linear_solver_threads);
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

    //tearDown(reconstruction);

    return true;
}

const ceres::Solver::Summary &BundleAdjuster::summary() const
{
    return summary_;
}

void BundleAdjuster::setUp(colmap::Reconstruction *reconstruction,
                           ceres::LossFunction *lossFunction)
{
    // Warning: AddPointsToProblem assumes that AddImageToProblem is called first.
    // Do not change order of instructions!
    for (const colmap::image_t image_id : config_.Images()) {
        addImageToProblem(image_id, reconstruction, lossFunction);
    }
    for (const auto point3D_id : config_.VariablePoints()) {
        addPointToProblem(point3D_id, reconstruction, lossFunction);
    }
    for (const auto point3D_id : config_.ConstantPoints()) {
        addPointToProblem(point3D_id, reconstruction, lossFunction);
    }

    //// GRAPHOS
    //Se cargan los puntos de control. Los puntos de control son constantes
    for (auto &gcp : config_.controlPoints()) {
        //0.5 Es lo que usa openMVG por defecto
        addControlPointToProblem(gcp, reconstruction, 0.2, lossFunction);
    }
    //// GRAPHOS
    
    parameterizeCameras(reconstruction);
    parameterizePoints(reconstruction);

}

void BundleAdjuster::addImageToProblem(const colmap::image_t imageId,
                                       colmap::Reconstruction *reconstruction,
                                       ceres::LossFunction *lossFunction)
{
    colmap::Image &image = reconstruction->Image(imageId);
    colmap::Camera &camera = reconstruction->Camera(image.CameraId());

    // CostFunction assumes unit quaternions.
    image.NormalizeQvec();

    double *qvec_data = image.Qvec().data();
    double *tvec_data = image.Tvec().data();
    double *camera_params_data = camera.ParamsData();

    const bool constant_pose = !options_.refine_extrinsics || config_.HasConstantPose(imageId);


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

            problem_->AddResidualBlock(cost_function, lossFunction, point3D.XYZ().data(), camera_params_data);

        } else {

            switch (camera.ModelId()) {
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

            problem_->AddResidualBlock(cost_function, lossFunction, qvec_data, tvec_data, point3D.XYZ().data(), camera_params_data);

        }
    }

    if (num_observations > 0) {
        camera_ids_.insert(image.CameraId());

        // Set pose parameterization.
        if (!constant_pose) {

            ceres::LocalParameterization *quaternion_parameterization = new ceres::QuaternionParameterization;
            problem_->SetParameterization(qvec_data, quaternion_parameterization);

            if (config_.HasConstantTvec(imageId)) {
                const std::vector<int> &constant_tvec_idxs = config_.ConstantTvec(imageId);
                ceres::SubsetParameterization *tvec_parameterization = new ceres::SubsetParameterization(3, constant_tvec_idxs);
                problem_->SetParameterization(tvec_data, tvec_parameterization);
            }

            //// GRAPHOS
            double position_error = config_.getCamPositionError(imageId);
            //double position_weight = (1. / position_error) * 100.;
            // https://github.com/openMVG/openMVG/issues/1822#issuecomment-766335311
            double position_weight = config_.getCamPositionError(imageId) * image.NumPoints3D();

            // Añadir restricciones de posición de la cámara
            if (position_error > 0) {
                ceres::CostFunction *position_cost_function = CameraPositionCostFunction::create(image.Tvec(), position_weight);
                // Para RTK no se utiliza función de coste
                problem_->AddResidualBlock(position_cost_function, nullptr, tvec_data);
                
            }

            //// GRAPHOS
        }
    }
}

void BundleAdjuster::addPointToProblem(const colmap::point3D_t point3DId,
                                       colmap::Reconstruction *reconstruction,
                                       ceres::LossFunction *lossFunction)
{
    colmap::Point3D &point3D = reconstruction->Point3D(point3DId);

    // Is 3D point already fully contained in the problem? I.e. its entire track
    // is contained in `variable_image_ids`, `constant_image_ids`,
    // `constant_x_image_ids`.
    if (point3D_num_observations_[point3DId] == point3D.Track().Length()) {
        return;
    }

    for (const auto &track_el : point3D.Track().Elements()) {
        // Skip observations that were already added in `FillImages`.
        if (config_.HasImage(track_el.image_id)) {
            continue;
        }

        point3D_num_observations_[point3DId] += 1;

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

        problem_->AddResidualBlock(cost_function, lossFunction, point3D.XYZ().data(), camera.ParamsData());
    }
}

void BundleAdjuster::addControlPointToProblem(GCP &ground_points,
                                              colmap::Reconstruction *reconstruction,
                                              double ground_point_error,
                                              ceres::LossFunction *lossFunction)
{
    double weight = (1. / ground_point_error);

    auto &track = ground_points.track;

    Eigen::Vector3d point_3d(ground_points.point.x(), ground_points.point.y(), ground_points.point.z());

    for (const auto &track_elements : track.points()) {

        auto graphos_image_id = track_elements.first;
        colmap::image_t colmap_image_id = config_.colmapId(graphos_image_id);
        auto &track_point = track_elements.second;

        colmap::Image &image = reconstruction->Image(colmap_image_id);
        colmap::Camera &camera = reconstruction->Camera(image.CameraId());
        double *qvec_data = image.Qvec().data();
        double *tvec_data = image.Tvec().data();
        Eigen::Vector2d point2D(track_point.x, track_point.y);

        ceres::CostFunction *cost_function = nullptr;

        switch (camera.ModelId()) {
        case colmap::SimplePinholeCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::SimplePinholeCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::PinholeCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::PinholeCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::SimpleRadialCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::SimpleRadialCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::SimpleRadialFisheyeCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::SimpleRadialFisheyeCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::RadialCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::RadialCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::RadialFisheyeCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::RadialFisheyeCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::OpenCVCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::OpenCVCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::OpenCVFisheyeCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::OpenCVFisheyeCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::FullOpenCVCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::FullOpenCVCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::FOVCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::FOVCameraModel>::Create(point2D, point_3d, weight);
            break;
        case colmap::ThinPrismFisheyeCameraModel::kModelId: cost_function = GroundControlPointCostFunction<colmap::ThinPrismFisheyeCameraModel>::Create(point2D, point_3d, weight);
            break;
        default: throw std::domain_error("Camera model does not exist");
            break;
        }

        problem_->AddResidualBlock(cost_function, lossFunction, qvec_data, tvec_data, camera.ParamsData());

    }

}

void BundleAdjuster::parameterizeCameras(colmap::Reconstruction *reconstruction) const
{
    const bool constant_camera = !options_.refine_focal_length &&
                                 !options_.refine_principal_point &&
                                 !options_.refine_extra_params;

    for (const colmap::camera_t camera_id : camera_ids_) {

        colmap::Camera &camera = reconstruction->Camera(camera_id);

        if (constant_camera || config_.IsConstantCamera(camera_id)) {
            problem_->SetParameterBlockConstant(camera.ParamsData());
        } else {
            std::vector<int> const_camera_params;

            if (!options_.refine_focal_length) {
                const std::vector<size_t> &params_idxs = camera.FocalLengthIdxs();
                const_camera_params.insert(const_camera_params.end(), params_idxs.begin(), params_idxs.end());
            }

            if (!options_.refine_principal_point) {
                const std::vector<size_t> &params_idxs = camera.PrincipalPointIdxs();
                const_camera_params.insert(const_camera_params.end(), params_idxs.begin(), params_idxs.end());
            }

            if (!options_.refine_extra_params) {
                const std::vector<size_t> &params_idxs = camera.ExtraParamsIdxs();
                const_camera_params.insert(const_camera_params.end(), params_idxs.begin(), params_idxs.end());
            }

            if (!const_camera_params.empty()) {
                auto camera_params_parameterization = new ceres::SubsetParameterization(static_cast<int>(camera.NumParams()), const_camera_params);
                problem_->SetParameterization(camera.ParamsData(), camera_params_parameterization);
            }
        }
    }
}

void BundleAdjuster::parameterizePoints(colmap::Reconstruction *reconstruction) const
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



} // namespace graphos