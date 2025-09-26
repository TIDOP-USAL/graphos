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

#include "graphos/core/sfm/Reconstruction.h"

#include <base/gps.h>

#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/sfm/BundleAdjuster.h"
#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/sfm/orientationexport.h"
#include "graphos/core/camera/Camera.h"

#include <tidop/core/exception.h>
#include <tidop/core/progress.h>

#include <colmap/base/reconstruction.h>
#include <colmap/controllers/hierarchical_mapper.h>
#include <colmap/util/option_manager.h>
#include <colmap/estimators/triangulation.h>
#include <tidop/geospatial/crs.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>

namespace graphos
{

std::atomic<bool> ba_terminate(false);

// Callback functor called after each bundle adjustment iteration.
class BundleAdjustmentIterationCallback
  : public ceres::IterationCallback
{

public:

    BundleAdjustmentIterationCallback() = default;

    auto operator()(const ceres::IterationSummary& summary) -> ceres::CallbackReturnType override
    {

        if (ba_terminate) {
            return ceres::SOLVER_TERMINATE_SUCCESSFULLY;
        }

        return ceres::SOLVER_CONTINUE;
    }

};

constexpr auto absolute_orientation_min_common_images = 3;
constexpr auto absolute_orientation_robust_alignment = true;
constexpr auto absolute_orientation_robust_alignment_max_error = 1.;



ReconstructionTask::ReconstructionTask(tl::Path database,
                                       tl::Path outputPath,
                                       const std::vector<Image> &images,
                                       const std::map<int, Camera> &cameras,
                                       Options options,
                                       tl::Path groundControlPoints)
  : mDatabase(std::move(database)),
    mOutputPath(std::move(outputPath)),
    mImages(images),
    mCameras(cameras),
    mOptions(options),
    mGroundControlPoints(std::move(groundControlPoints)),
    mMinCommonImages(absolute_orientation_min_common_images),
    mRobustAlignment(absolute_orientation_robust_alignment),
    mRobustAlignmentMaxError(absolute_orientation_robust_alignment_max_error),
    mIncrementalMapperOptions(new colmap::IncrementalMapperOptions),
    mMapper(nullptr),
    mReconstructionManager(new colmap::ReconstructionManager),
    mGeoTools(tl::GeoTools::getInstance())
{
    if (mOptions.isEnabled(Options::absolute_orientation)) {

        if (!mGroundControlPoints.exists()) {
            if (mOptions.isEnabled(Options::use_gcp)) {
                tl::Message::warning("'Options::use_gcp' is active but there are no GCPs. 'Options::use_gcp' is deactivated.");
                mOptions.disable(Options::use_gcp);
            }
        } else {
            ///TODO: Hay problemas al combinar puntos de control y posiciones de las camaras
            ///      Da mas prioridad a las posiciones de las cámaras
            mOptions.disable(Options::use_poses);
        }

        if (mOptions.isEnabled(Options::use_poses)) {

            bool found_poses = false;
                for (const auto &image : mImages) {

                    if (!image.cameraPose().isEmpty()) {
                        found_poses = true;
                        break;
                    }
                }

            if (!found_poses) {
                tl::Message::warning("'Options::use_poses' is active but there are no camera poses. 'Options::use_poses' is deactivated.");
                mOptions.disable(Options::use_poses);
            }
        }

        if (mOptions.isDisabled(Options::use_gcp) &&
            mOptions.isDisabled(Options::use_poses)) {

            tl::Message::warning("'Options::absolute_orientation' is enabled but no control points or camera positions are available. Absolute orientation is disabled.");
            mOptions.disable(Options::absolute_orientation);
        }
    }


    // Calibration options
    colmap::Database colmap_database(mDatabase.toUtf8());

    for (auto &camera : mCameras) {

        auto graphos_camera_id = camera.first;
        auto &graphos_camera = camera.second;
        auto colmap_camera_id = static_cast<colmap::image_t>(graphos_camera_id);

        TL_ASSERT(colmap_database.ExistsCamera(colmap_camera_id), "Camera not found in database");
        colmap::Camera camera_colmap = colmap_database.ReadCamera(colmap_camera_id);

        auto &adjusted_calibration = graphos_camera.calibration();
        auto &prior_calibration = graphos_camera.priorCalibration();

        if (adjusted_calibration == nullptr && prior_calibration == nullptr) {
            continue;
        }


        if (mOptions.isEnabled(Options::use_adjusted_calibration) && adjusted_calibration) {

            auto params = adjusted_calibration->toVector();
            if (params.size() == camera_colmap.NumParams()) {
                camera_colmap.SetParams(params);
                camera_colmap.SetPriorFocalLength(true);
            }

        } else if (mOptions.isEnabled(Options::use_prior_calibration)) {

            std::vector<double> params;
            if (prior_calibration) {
                params = prior_calibration->toVector();
            } else if (adjusted_calibration) {
                params = adjusted_calibration->toVector();
            } else {
                continue;
            }

            if (params.size() == camera_colmap.NumParams()) {
                camera_colmap.SetParams(params);
                camera_colmap.SetPriorFocalLength(true);
            }

        } else {

            double focal_lenght = graphos_camera.focal();
            size_t width = static_cast<size_t>(graphos_camera.width());
            size_t height = static_cast<size_t>(graphos_camera.height());
            if (focal_lenght > 0.) {
                camera_colmap.SetPriorFocalLength(true);
            } else {
                focal_lenght = 1.2 * std::max(width, height);
                camera_colmap.SetPriorFocalLength(false);
            }
            camera_colmap.SetFocalLength(focal_lenght);
            camera_colmap.SetWidth(width);
            camera_colmap.SetHeight(height);
        }

        colmap_database.UpdateCamera(camera_colmap);
    }

    colmap_database.Close();
}

ReconstructionTask::~ReconstructionTask()
{
    if (mIncrementalMapperOptions) {
        delete mIncrementalMapperOptions;
        mIncrementalMapperOptions = nullptr;
    }

    if (mMapper) {
        delete mMapper;
        mMapper = nullptr;
    }

    mReconstructionManager->Clear();
    mReconstructionManager.reset();
}

auto ReconstructionTask::cameras() const -> std::map<int, Camera>
{
    return mCameras;
}

auto ReconstructionTask::cameraPosesErrors() const -> std::unordered_map<size_t, double>
{
    return mCameraPosesErrors;
}

auto ReconstructionTask::report() const -> OrientationReport
{
    return mOrientationReport;
}

auto ReconstructionTask::enuCrs() const -> std::string
{
    return mEnuCrs;
}

void ReconstructionTask::setMinCommonImages(int minCommonImages)
{
    mMinCommonImages = minCommonImages;
}

void ReconstructionTask::setRobustAlignment(bool robustAlignment)
{
    mRobustAlignment = robustAlignment;
}

void ReconstructionTask::setRobustAlignmentMaxError(double robustAlignmentMaxError)
{
    mRobustAlignmentMaxError = robustAlignmentMaxError;
}

void ReconstructionTask::clear()
{
    mMinCommonImages = absolute_orientation_min_common_images;
    mRobustAlignment = absolute_orientation_robust_alignment;
    mRobustAlignmentMaxError = absolute_orientation_robust_alignment_max_error;
}

void ReconstructionTask::stop()
{
    TaskBase::stop();

    if (mMapper && mMapper->IsRunning())
        mMapper->Stop();

    ba_terminate = true;
}

void ReconstructionTask::execute(tl::Progress *progressBar)
{

    try {

        mReconstructionManager->Clear();

        if (!mOutputPath.exists() && !mOutputPath.createDirectories()) {
            TL_THROW_EXCEPTION("Directory couldn't be created: {}", mOutputPath.toString());
        }

        if (mMapper) {
            delete mMapper;
            mMapper = nullptr;
        }

        /// ¿Si se establece como fija la calibración?
        mIncrementalMapperOptions->ba_refine_focal_length = !mOptions.isEnabled(Options::fix_calibration);
        mIncrementalMapperOptions->ba_refine_principal_point = false;
        mIncrementalMapperOptions->ba_refine_extra_params = !mOptions.isEnabled(Options::fix_calibration);

        mMapper = new colmap::IncrementalMapperController(mIncrementalMapperOptions, "", mDatabase.toUtf8(), mReconstructionManager.get());

        size_t prev_num_reconstructions = 0;
        mMapper->AddCallback(colmap::IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [&]() {

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

        mMapper->AddCallback(colmap::IncrementalMapperController::NEXT_IMAGE_REG_CALLBACK, [&]() {
                //if (progressBar) (*progressBar)();
                //msgInfo("-----");
            });

        mMapper->Start();
        mMapper->Wait();

        if (status() == Status::stopping) return;

        TL_ASSERT(mReconstructionManager->Size() > 0, "Reconstruction fail");

        colmap::OptionManager option_manager;
        option_manager.bundle_adjustment->refine_focal_length = !mOptions.isEnabled(Options::fix_calibration);
        option_manager.bundle_adjustment->refine_principal_point = !mOptions.isEnabled(Options::fix_calibration);
        option_manager.bundle_adjustment->refine_extra_params = !mOptions.isEnabled(Options::fix_calibration);

        colmap::Reconstruction &reconstruction = mReconstructionManager->Get(0);

        const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

        TL_ASSERT(reg_image_ids.size() >= 2, "Need at least two views.");

        // Avoid degeneracies in bundle adjustment.
        reconstruction.FilterObservationsWithNegativeDepth();

        colmap::BundleAdjustmentOptions ba_options = *option_manager.bundle_adjustment;
        ba_options.solver_options.minimizer_progress_to_stdout = false;
        ba_options.solver_options.logging_type = ceres::LoggingType::SILENT;

        BundleAdjustmentIterationCallback iteration_callback;
        ba_options.solver_options.callbacks.push_back(&iteration_callback);

        // Configure bundle adjustment.
        BundleAdjustmentConfig ba_config;
        for (const colmap::image_t image_id : reg_image_ids) {
            ba_config.AddImage(image_id);
        }
        ba_config.SetConstantPose(reg_image_ids[0]);
        ba_config.SetConstantTvec(reg_image_ids[1], {0});

        ceres::Solver::Summary summary;

        //{
        //    ba_terminate = false;

        //    BundleAdjuster bundle_adjuster(ba_options, ba_config);
        //    bundle_adjuster.solve(&reconstruction);

        //    if (status() == Status::stopping) return;

        //    ba_terminate = true;
        //    summary = bundle_adjuster.summary();
        //}

        // TODO: Igual mejor que sea un bucle
        //if (summary.termination_type == ceres::NO_CONVERGENCE) {

        //    ba_terminate = false;

        //    BundleAdjuster bundle_adjuster(ba_options, ba_config);
        //    bundle_adjuster.solve(&reconstruction);

        //    if (status() == Status::stopping) return;

        //    ba_terminate = true;
        //    summary = bundle_adjuster.summary();
        //}
        for (int i = 0; i < 5; ++i) {
            ba_terminate = false;

            BundleAdjuster bundle_adjuster(ba_options, ba_config);
            bundle_adjuster.solve(&reconstruction);

            if (status() == Status::stopping) return;

            ba_terminate = true;
            summary = bundle_adjuster.summary();

            if (summary.termination_type == ceres::CONVERGENCE) break;
        }

        mOrientationReport.iterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
        mOrientationReport.initialCost = std::sqrt(summary.initial_cost / summary.num_residuals_reduced);
        mOrientationReport.finalCost = std::sqrt(summary.final_cost / summary.num_residuals_reduced);
        mOrientationReport.termination = "CONVERGENCE";
        mOrientationReport.time = this->time();

        TL_ASSERT(summary.termination_type == ceres::CONVERGENCE, "Bundle adjust: NO CONVERGENCE");

        if (status() == Status::stopping) return;

        tl::Message::success("Relative orientation finished in {:.2} minutes", mOrientationReport.time / 60.);

        

        if (mOptions.isEnabled(Options::absolute_orientation)) {

            /// Conversión de identificadores de imagen de GRAPHOS a COLMAP
            std::unordered_map<uint32_t, size_t> image_ids_colmap_to_graphos;
            std::unordered_map<size_t, uint32_t> image_ids_graphos_to_colmap;

            for (const auto &image : mImages) {

                tl::Path image_path(image.path().toStdString());

                for (const auto &colmap_image : reconstruction.Images()) {
                    tl::Path colmap_image_path(colmap_image.second.Name());

                    if (image_path.equivalent(colmap_image_path)) {
                        image_ids_colmap_to_graphos[colmap_image.first] = image.id();
                        image_ids_graphos_to_colmap[image.id()] = colmap_image.first;
                        break;
                    }
                }
            }

            double robust_fitting_error = 2.0;
            std::unordered_map<size_t, tl::Point3<double>> cameras_enu;

            if (mOptions.isEnabled(Options::use_poses)) {

                std::unordered_map<size_t, tl::Point3<double>> cameras_geographic;
                
                for (const auto &image : mImages) {
                
                    if (image_ids_graphos_to_colmap[image.id()] == 0) continue;
                
                    // Conversion a coordendas ENU
                
                    if (image.cameraPose().isEmpty()) continue;
                
                    auto camera_epsg = image.cameraPose().crs();
                    auto camera_crs = std::make_shared<tl::Crs>(camera_epsg.toStdString());
                    TL_ASSERT(camera_crs->isValid(), "Invalid CRS: {}", camera_epsg.toStdString());
                
                    auto epsg_code = camera_epsg.toStdString();
                    auto camera_coordinates = image.cameraPose().position();
                    if (epsg_code != "EPSG:4326")
                        mGeoTools->ptrCRSsTools()->crsOperation(epsg_code, "EPSG:4326", camera_coordinates.x, camera_coordinates.y, camera_coordinates.z);
                
                    cameras_geographic[image.id()] = camera_coordinates;
                }


                /// Cálculo del centro

                tl::Point3<double> geographic_center;
                for (const auto &coordinates : cameras_geographic) {
                    geographic_center += coordinates.second / static_cast<double>(cameras_geographic.size());
                }

                tl::Path enu_path = mOutputPath;
                enu_path.append("enu.txt");
                mEnuCrs = mGeoTools->ptrCRSsTools()->getCRSEnu("EPSG:4326", geographic_center.x, geographic_center.y, geographic_center.z);

                for (const auto &geographic_coordinates : cameras_geographic) {
                    auto image_id = geographic_coordinates.first;
                    auto coordinates = geographic_coordinates.second;
                    mGeoTools->ptrCRSsTools()->crsOperation("EPSG:4326", mEnuCrs, coordinates.x, coordinates.y, coordinates.z);
                    cameras_enu[image_id] = coordinates;
                }


                /// Transformación de semejanza
                bool robust_alignment = mRobustAlignment;
                colmap::RANSACOptions ransac_options;
                ransac_options.max_error = mRobustAlignmentMaxError;

                if (robust_alignment && ransac_options.max_error <= 0) {
                    throw std::runtime_error("Similarity transformation: You must provide a maximum alignment error > 0");
                }

                std::vector<std::string> ref_image_names;
                std::vector<Eigen::Vector3d> ref_locations;

                for (const auto &camera_enu : cameras_enu) {
                    auto graphos_id = camera_enu.first;
                    auto enu_coordinates = camera_enu.second;
                    auto colmap_id = image_ids_graphos_to_colmap[graphos_id];
                    const auto &image = reconstruction.Image(colmap_id);
                    ref_image_names.push_back(image.Name());

                    Eigen::Vector3d camera_coordinates;
                    camera_coordinates[0] = enu_coordinates.x;
                    camera_coordinates[1] = enu_coordinates.y;
                    camera_coordinates[2] = enu_coordinates.z;

                    ref_locations.push_back(camera_coordinates);
                }

                if (status() == Status::stopping) return;

                bool alignment_success;
                if (robust_alignment) {
                    alignment_success = reconstruction.AlignRobust(ref_image_names, ref_locations, mMinCommonImages, ransac_options);
                } else {
                    alignment_success = reconstruction.Align(ref_image_names, ref_locations, mMinCommonImages);
                }

                if (status() == Status::stopping) return;

                TL_ASSERT(alignment_success, "Similarity transformation: Alignment failed");


                std::vector<double> errors;
                errors.reserve(ref_image_names.size());

                for (size_t i = 0; i < ref_image_names.size(); ++i) {
                    const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
                    if (image != nullptr) {
                        double error = (image->ProjectionCenter() - ref_locations[i]).norm();
                        errors.push_back(error);
                    }
                }

                robust_fitting_error = colmap::Median(errors);

                tl::Message::info("Similarity transformation: Alignment error: {} (mean), {} (median)", colmap::Mean(errors), colmap::Median(errors));

                if (status() == Status::stopping) return;

            }

            //} 


            // RTK
            // - Se añaden las camaras al ajuste de haces
            //BundleAdjustmentConfig ba_config;
            //for (const colmap::image_t image_id : reconstruction.RegImageIds()) {
            //    ba_config.AddImage(image_id);
            //    ba_config.setCameraPositionAccuracy(image_id, 0.01);
            //}
            // En OpenMVG utiliza la mediana del error calculado en la transformacíón de semejanza
            // problem.AddResidualBlock(
            //     cost_function,
            //     new ceres::HuberLoss(
            //         Square(pose_center_robust_fitting_error)),
            //     &map_poses.at(prior->id_view)[0]);
            // Aqui se obtiene con colmap::Median(errors)
            // Habría que aplicarlo como
            // ceres::CostFunction *control_point_cost_function = ControlPointCostFunction::create(point3D, weight_cp);
            //problem_->AddResidualBlock(control_point_cost_function, new ceres::HuberLoss(Square(colmap::Median(errors))), gcp.point.data());

            if (mOptions.isEnabled(Options::absolute_orientation)) {

                /// Lectura de los puntos de control

                std::vector<GCP> control_points_enu;
                std::vector<GroundControlPoint> ground_control_points;
                std::string ground_control_points_crs;

                if (mOptions.isEnabled(Options::use_gcp)) {


                    tl::Path gcp_file = mOutputPath;
                    gcp_file.append("georef.xml");
                    auto gcp_reader = GCPsReaderFactory::create("GRAPHOS");
                    gcp_reader->read(gcp_file);
                    ground_control_points = gcp_reader->gcps();
                    ground_control_points_crs = gcp_reader->epsgCode();

                    TL_ASSERT(!ground_control_points_crs.empty(), "Unknow CRS for ground control points");

                    if (mEnuCrs.empty()) {


                        /// Cálculo del centro
                        tl::Point3<double> geographic_center;
                        double i = 1.;
                        for (const auto &gcp : ground_control_points) {
                            auto coordinates = gcp;
                            if (ground_control_points_crs != "EPSG:4326")
                                mGeoTools->ptrCRSsTools()->crsOperation(ground_control_points_crs, "EPSG:4326", coordinates.x, coordinates.y, coordinates.z);
                            geographic_center += coordinates / static_cast<double>(ground_control_points.size());
                        }

                        tl::Path enu_path = mOutputPath;
                        enu_path.append("enu.txt");
                        mEnuCrs = mGeoTools->ptrCRSsTools()->getCRSEnu("EPSG:4326", geographic_center.x, geographic_center.y, geographic_center.z);

                    }

                    std::vector<Eigen::Vector3d> src;
                    std::vector<Eigen::Vector3d> dst;
                    std::vector<std::string> gcp_name;

                    colmap::IncrementalTriangulator::Options options;

                    // Setup estimation options.
                    colmap::EstimateTriangulationOptions tri_options;
                    tri_options.min_tri_angle = colmap::DegToRad(options.min_angle);
                    tri_options.residual_type = colmap::TriangulationEstimator::ResidualType::REPROJECTION_ERROR;
                    tri_options.ransac_options.max_error = options.complete_max_reproj_error;
                    tri_options.ransac_options.confidence = 0.9999;
                    tri_options.ransac_options.min_inlier_ratio = 0.02;
                    tri_options.ransac_options.max_num_trials = 10000;


                    colmap::Database database;
                    database.Open(mDatabase.toUtf8());

                    for (auto &ground_control_point : ground_control_points) {

                        std::vector<colmap::TriangulationEstimator::PointData> points_data;
                        std::vector<colmap::TriangulationEstimator::PoseData> poses_data;

                        for (auto &camera : reconstruction.Cameras()) {

                            for (auto &image : reconstruction.Images()) {

                                if (image.second.CameraId() == camera.second.CameraId()) {

                                    auto &track = ground_control_point.track();

                                    if (track.existPoint(image_ids_colmap_to_graphos[image.second.ImageId()])) {

                                        tl::Point<double> point = track.point(image_ids_colmap_to_graphos[image.second.ImageId()]);

                                        colmap::TriangulationEstimator::PointData point_data;
                                        point_data.point = Eigen::Vector2d(point.x, point.y);
                                        point_data.point_normalized = camera.second.ImageToWorld(point_data.point);
                                        points_data.push_back(point_data);

                                        colmap::TriangulationEstimator::PoseData pose_data;
                                        pose_data.proj_matrix = image.second.ProjectionMatrix();
                                        pose_data.proj_center = image.second.ProjectionCenter();
                                        pose_data.camera = &camera.second;
                                        poses_data.push_back(pose_data);
                                    }

                                }
                            }
                        }

                        if (points_data.size() < 2) continue;

                        Eigen::Vector3d xyz;
                        std::vector<char> inlier_mask;
                        if (colmap::EstimateTriangulation(tri_options, points_data, poses_data, &inlier_mask, &xyz)) {

                            src.push_back(xyz);

                            auto gcps_enu = ground_control_point;
                            //if (ground_control_points_crs != "EPSG:4326")
                                mGeoTools->ptrCRSsTools()->crsOperation(ground_control_points_crs, mEnuCrs, gcps_enu.x, gcps_enu.y, gcps_enu.z);

                            GCP _gcp;
                            _gcp.point = {gcps_enu.x, gcps_enu.y, gcps_enu.z};
                            _gcp.track = ground_control_point.track();
                            _gcp.name = ground_control_point.name();
                            control_points_enu.push_back(_gcp);

                            dst.emplace_back(gcps_enu.x, gcps_enu.y, gcps_enu.z);
                            gcp_name.push_back(ground_control_point.name());
                        }

                    }

                    /// Esto solo si no tenemos datos GNSS
                    
                    if (mOptions.isDisabled(Options::use_poses)) {

                        TL_ASSERT(src.size() > 3, "Similarity transformation: Insufficient number of points");

                        colmap::SimilarityTransform3 similarity_transform;
                        similarity_transform.Estimate(src, dst);

                        reconstruction.Transform(similarity_transform);

                        std::vector<double> errors;
                        errors.reserve(dst.size());

                        for (size_t i = 0; i < dst.size(); ++i) {
                            similarity_transform.TransformPoint(&src.at(i));
                            errors.push_back((src[i] - dst[i]).norm());
                        }

                        //robust_fitting_error = colmap::Median(errors);

                        tl::Message::info("Georeference error: {} (mean), {} (median)", colmap::Mean(errors), colmap::Median(errors));

                    }
                }

                // Configure bundle adjustment.
                BundleAdjustmentConfig ba_config;
                ba_config.setRobustFittingError(robust_fitting_error);
                for (const colmap::image_t image_id : reg_image_ids) {
                    ba_config.AddImage(image_id);
                    if (mOptions.isEnabled(Options::use_poses)){

                        size_t image_graphos_id = image_ids_colmap_to_graphos[image_id];
                        tl::Vector3d accuracy{10., 10., 10.};
                        for (auto &image : mImages) {
                            if (image.id() == image_graphos_id) {
                                auto camera_pose = image.cameraPose();
                                if (!camera_pose.isEmpty()) {
                                    if (mOptions.isEnabled(Options::use_rtk_positioning_accuracy)) {
                                        accuracy = camera_pose.accuracy();
                                    } else if (camera_pose.rtkFlag() == 50){
                                        accuracy = {0.01, 0.01, 0.03};
                                        //accuracy = {0.01, 0.01, 0.01};
                                    } else if (camera_pose.rtkFlag() == 34){
                                        accuracy = {0.2, 0.2, 0.5};
                                    } else if (camera_pose.rtkFlag() == 16) {
                                        accuracy = {10., 10., 10.};
                                    }
                                }
                                break;
                            }
                        }

                        ba_config.setCameraPositionAccuracy(image_id, cameras_enu[image_graphos_id], accuracy);

                    }
                }


                if (mOptions.isEnabled(Options::use_gcp)) {
                    TL_ASSERT(control_points_enu.size() > 3 || mOptions.isEnabled(Options::use_poses), "A minimum of 3 ground control points is required");
                    if (control_points_enu.size() > 3) {
                        ba_config.setGroundControlPoints(control_points_enu);
                    }
                }

                ba_config.setImageIdsGraphosToColmap(image_ids_graphos_to_colmap); //Por ahora...
                ba_config.setImageIdsColmapToGraphos(image_ids_colmap_to_graphos);

                /// Configuración de OpenMVG

                colmap::BundleAdjustmentOptions ba_options;
                ba_options.solver_options.logging_type = ceres::LoggingType::SILENT;
                //ba_options.solver_options.function_tolerance = 0.0;
//                ba_options.solver_options.gradient_tolerance = 1e-10;
//                ba_options.solver_options.parameter_tolerance = 1e-8;
                ba_options.solver_options.minimizer_progress_to_stdout = false;
//                ba_options.solver_options.max_num_iterations = 50;
//                ba_options.solver_options.max_linear_solver_iterations = 500;
//                ba_options.solver_options.max_num_consecutive_invalid_steps = 10;
//                ba_options.solver_options.max_consecutive_nonmonotonic_steps = 10;
                ba_options.solver_options.num_threads = 4; // 1;
//#if CERES_VERSION_MAJOR < 2
//                solver_options.num_linear_solver_threads = -1;
//#endif  // CERES_VERSION_MAJOR
                //ba_options.loss_function_type = colmap::BundleAdjustmentOptions::LossFunctionType::CAUCHY;
                //ba_options.loss_function_scale = 4.;
                //ba_options.refine_focal_length = false;// true;
                //ba_options.refine_principal_point = false;
                //ba_options.refine_extra_params = false;// true;

                ceres::Solver::Summary summary;
                
                for (size_t i = 0; i < 5; i++) {
                    ba_terminate = false;

                    BundleAdjuster bundle_adjuster(ba_options, ba_config);
                    bundle_adjuster.solve(&reconstruction);
                
                    if (status() == Status::stopping) return;
                
                    ba_terminate = true;
                    summary = bundle_adjuster.summary();

                    if (summary.termination_type == ceres::CONVERGENCE) break;
                }

                // Calculo de los errores en el ajuste de haces

                if (mOptions.isEnabled(Options::use_gcp)){

                    std::vector<Eigen::Vector3d> src;
                    std::vector<Eigen::Vector3d> dst;
                    std::vector<std::string> gcp_name;

                    colmap::IncrementalTriangulator::Options options;

                    // Setup estimation options.
                    colmap::EstimateTriangulationOptions tri_options;
                    tri_options.min_tri_angle = colmap::DegToRad(options.min_angle);
                    tri_options.residual_type = colmap::TriangulationEstimator::ResidualType::REPROJECTION_ERROR;
                    tri_options.ransac_options.max_error = options.complete_max_reproj_error;
                    tri_options.ransac_options.confidence = 0.9999;
                    tri_options.ransac_options.min_inlier_ratio = 0.02;
                    tri_options.ransac_options.max_num_trials = 10000;

                    for (size_t i = 0; i < control_points_enu.size(); ++i) {

                        std::vector<colmap::TriangulationEstimator::PointData> points_data;
                        std::vector<colmap::TriangulationEstimator::PoseData> poses_data;

                        for (auto &camera : reconstruction.Cameras()) {

                            for (auto &image : reconstruction.Images()) {

                                if (image.second.CameraId() == camera.second.CameraId()) {

                                    auto &track = control_points_enu[i].track;

                                    size_t graphos_image_id = image_ids_colmap_to_graphos[image.second.ImageId()];
                                    if (track.existPoint(graphos_image_id)) {

                                        tl::Point<double> point = track.point(graphos_image_id);

                                        colmap::TriangulationEstimator::PointData point_data;
                                        point_data.point = Eigen::Vector2d(point.x, point.y);
                                        point_data.point_normalized = camera.second.ImageToWorld(point_data.point);
                                        points_data.push_back(point_data);

                                        colmap::TriangulationEstimator::PoseData pose_data;
                                        pose_data.proj_matrix = image.second.ProjectionMatrix();
                                        pose_data.proj_center = image.second.ProjectionCenter();
                                        pose_data.camera = &camera.second;
                                        poses_data.push_back(pose_data);

                                        /// Residuals. Lo calculo en coordenadas ENU
                                        Eigen::Vector3d gcp(control_points_enu[i].point.x(), control_points_enu[i].point.y(), control_points_enu[i].point.z());
                                        auto rotate_point = image.second.RotationMatrix() * gcp + image.second.Tvec();
                                        auto x = rotate_point.x() / rotate_point.z();
                                        auto y = rotate_point.y() / rotate_point.z();
                                        auto residuals = camera.second.WorldToImage({x, y});
                                        residuals -= point_data.point;
                                        ground_control_points[i].addErrorToTrack(graphos_image_id, {residuals.x(), residuals.y()});
                                    }

                                }
                            }
                        }

                        if (points_data.size() < 2) continue;

                        Eigen::Vector3d xyz;
                        std::vector<char> inlier_mask;
                        if (colmap::EstimateTriangulation(tri_options, points_data, poses_data, &inlier_mask, &xyz)) {
                            src.push_back(xyz);
                            dst.emplace_back(control_points_enu[i].point.x(), control_points_enu[i].point.y(), control_points_enu[i].point.z());
                            gcp_name.push_back(control_points_enu[i].name);
                        }

                    }


                    std::vector<double> errors;
                    errors.reserve(dst.size());

                    for (size_t i = 0; i < dst.size(); ++i) {
                        auto error = (src[i] - dst[i]).norm();
                        ground_control_points[i].setError(error);
                        errors.push_back(error);

                        tl::Message::info("Ground Control Point {}: Error -> {}", gcp_name[i], errors[i]);
                    }

                    tl::Path gcp_file = mOutputPath;
                    gcp_file.append("georef.xml");
                    auto gcp_writer = GCPsWriterFactory::create("GRAPHOS");
                    gcp_writer->setEPSGCode(ground_control_points_crs);
                    gcp_writer->setGCPs(ground_control_points);
                    gcp_writer->write(gcp_file);

                    mOrientationReport.alignmentErrorMean = colmap::Mean(errors);
                    mOrientationReport.alignmentErrorMedian = colmap::Median(errors);

                    tl::Message::info("Ground Control Points error: {} (mean), {} (median)",
                                      mOrientationReport.alignmentErrorMean, mOrientationReport.alignmentErrorMedian);


                } 

                if (mOptions.isEnabled(Options::use_poses)){

                    std::vector<double> errors;
                    errors.reserve(mImages.size());
                    std::vector<double> errors_x;
                    std::vector<double> errors_y;
                    std::vector<double> errors_z;
                    errors_x.reserve(mImages.size());
                    errors_y.reserve(mImages.size());
                    errors_z.reserve(mImages.size());

                    for (const auto &image : mImages)
                    {

                        if (reconstruction.ExistsImage(image_ids_graphos_to_colmap[image.id()])) {

                            const colmap::Image &colmap_image = reconstruction.Image(image_ids_graphos_to_colmap[image.id()]);
                            tl::Point3<double> position = image.cameraPose().position();
                            Eigen::Vector3d pos_ini;
                            pos_ini[0] = cameras_enu[image.id()].x;
                            pos_ini[1] = cameras_enu[image.id()].y;
                            pos_ini[2] = cameras_enu[image.id()].z;
                            std::string accuracy;
                            if (mOptions.isDisabled(Options::use_rtk_positioning_accuracy)) {
                                accuracy = "10.";
                            } else {
                                accuracy = std::to_string(image.cameraPose().accuracy().x())
                                           .append(" ")
                                           .append(std::to_string(image.cameraPose().accuracy().y()))
                                           .append(" ")
                                           .append(std::to_string(image.cameraPose().accuracy().z()));
                            }
                            auto pos_final = colmap_image.ProjectionCenter();
                            auto error_xyz = pos_final - pos_ini;
                            double error = (error_xyz).norm();
                            double error_x = std::abs(error_xyz.x());
                            double error_y = std::abs(error_xyz.y());
                            double error_z = std::abs(error_xyz.z());
                            //tl::Message::info("Camera pose error: {} -> Error:{}", image.name().toStdString(), error);

                            std::cout << std::fixed << std::setprecision(6)
                                << image.name().toStdString() << "\t"
                                << pos_ini.x() << "\t" << pos_ini.y() << "\t" << pos_ini.z() << "\t"
                                << accuracy << "\t" << error << "\t"
                                << error_xyz.x() << "\t" << error_xyz.y() << "\t" << error_xyz.z() << "\t"
                                << pos_final.x() << "\t" << pos_final.y() << "\t" << pos_final.z()
                                << std::endl;

                            errors.push_back(error);
                            errors_x.push_back(error_x);
                            errors_y.push_back(error_y);
                            errors_z.push_back(error_z);
                        }
                    }

                    mOrientationReport.alignmentErrorMean = colmap::Mean(errors);
                    mOrientationReport.alignmentErrorMedian = colmap::Median(errors);

                    tl::Message::info("Alignment error: {} (mean), {} (median)", mOrientationReport.alignmentErrorMean, mOrientationReport.alignmentErrorMedian);

                    std::cout << std::fixed << "Error x,y,z: [" << colmap::Median(errors_x) << ", " << colmap::Median(errors_y) << ", " << colmap::Median(errors_z) << "]" << std::endl;
                }

                // Fin calculo de errores en el ajuste de haces
                
                

                mOrientationReport.iterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
                mOrientationReport.initialCost = std::sqrt(summary.initial_cost / summary.num_residuals_reduced);
                mOrientationReport.finalCost = std::sqrt(summary.final_cost / summary.num_residuals_reduced);
                mOrientationReport.termination = "CONVERGENCE";

                TL_ASSERT(summary.termination_type == ceres::CONVERGENCE, "Bundle adjust: NO CONVERGENCE");

                if (status() == Status::stopping) return;

                tl::Message::success("Absolute orientation finished in {:.2} minutes", (this->time() - mOrientationReport.time) / 60.);
            }


        }


        /// Write Ground points

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

        for (auto &camera : mCameras) {

            std::shared_ptr<Calibration> calibration = convert.readCalibration(camera.first);

            if (calibration) {
                camera.second.setCalibration(calibration);
            }
        }

        if (status() == tl::Task::Status::stopping) return;

        /// Write Sparse Cloud

        tl::Path sparse_path(mOutputPath);
        sparse_path.append("sparse.ply");
        OrientationExport orientation_export(&reconstruction);
        orientation_export.exportPLY(sparse_path);


        if (progressBar) (*progressBar)();

        mOrientationReport.time = this->time();

        tl::Message::success("3D reconstruction finished in {:.2} minutes", mOrientationReport.time / 60.);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Relative Orientation error");
    }
}

} // namespace graphos