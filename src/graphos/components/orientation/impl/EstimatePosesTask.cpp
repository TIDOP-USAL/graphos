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

#include "graphos/components/orientation/impl/EstimatePosesTask.h"

#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/CameraRepository.h"
#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/orientation/ColmapReconstructionConvert.h"
#include "graphos/core/orientation/io/CameraPosesWriter.h"
#include "graphos/core/orientation/io/OrientationExport.h"

#include <tidop/core/base/Exception.h>
#include <tidop/core/task/Progress.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>

#include <colmap/scene/reconstruction.h>
#include <colmap/controllers/incremental_pipeline.h>
#include <colmap/controllers/global_pipeline.h>
#include <colmap/controllers/option_manager.h>
#include <colmap/estimators/bundle_adjustment.h>
#include <colmap/estimators/bundle_adjustment_ceres.h>

#include <ceres/ceres.h>

namespace graphos
{

std::atomic<bool> ba_terminate(false);

constexpr auto absolute_orientation_min_common_images = 3;
constexpr auto absolute_orientation_robust_alignment = true;
constexpr auto absolute_orientation_robust_alignment_max_error = 1.;



EstimatePosesTask::EstimatePosesTask(tl::Path database,
                                     tl::Path outputPath,
                                     const ImageRepository &imageRepo,
                                     CameraRepository &cameraRepo,
                                     Options options,
                                     tl::Path groundControlPoints)
  : mDatabase(std::move(database)),
    mOutputPath(std::move(outputPath)),
    mImageRepo(imageRepo),
    mCameraRepo(cameraRepo),
    mOptions(options),
    mGroundControlPoints(std::move(groundControlPoints)),
    mMinCommonImages(absolute_orientation_min_common_images),
    mRobustAlignment(absolute_orientation_robust_alignment),
    mRobustAlignmentMaxError(absolute_orientation_robust_alignment_max_error),
    mMapper(nullptr),
    mGeoTools(tl::GeoTools::getInstance()),
    mEnuCrs("")
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
                for (const auto &[_, image] : mImageRepo) {

                    if (!image.cameraPose().isEmpty()) {
                        found_poses = true;
                        break;
                    }
                }

            if (!found_poses) {
                tl::Message::warning("'Options::use_poses' is active but there are no camera cameraPosesFile. 'Options::use_poses' is deactivated.");
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
    auto colmap_database = colmap::Database::Open(mDatabase.toUtf8());

    for (auto &[camera_id, camera] : mCameraRepo) {

        auto colmap_camera_id = static_cast<colmap::image_t>(camera_id);

        TL_ASSERT(colmap_database->ExistsCamera(colmap_camera_id), "Camera not found in database");
        colmap::Camera camera_colmap = colmap_database->ReadCamera(colmap_camera_id);

        auto adjusted_calibration = camera.calibration();
        auto prior_calibration = camera.priorCalibration();

        if (adjusted_calibration == nullptr && prior_calibration == nullptr) {
            continue;
        }

        if (mOptions.isEnabled(Options::use_adjusted_calibration) && adjusted_calibration) {

            auto params = adjusted_calibration->toVector();
            if (params.size() == camera_colmap.params.size()) {
                camera_colmap.params = params;
                camera_colmap.has_prior_focal_length = true;
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

            if (params.size() == camera_colmap.params.size()) {
                camera_colmap.params = params;
                camera_colmap.has_prior_focal_length = true;
            }

        } else {

            double focal_lenght = camera.focal();
            size_t width = static_cast<size_t>(camera.width());
            size_t height = static_cast<size_t>(camera.height());
            if (focal_lenght > 0.) {
                camera_colmap.has_prior_focal_length = true;
            } else {
                focal_lenght = 1.2 * std::max(width, height);
                camera_colmap.has_prior_focal_length = false;
            }
            camera_colmap.SetFocalLength(focal_lenght);
            camera_colmap.width = width;
            camera_colmap.height = height;
        }

        colmap_database->UpdateCamera(camera_colmap);
    }

    colmap_database->Close();
}

EstimatePosesTask::~EstimatePosesTask()
{
    if (mMapper) {
        delete mMapper;
        mMapper = nullptr;
    }
}

auto EstimatePosesTask::cameraPosesErrors() const -> std::unordered_map<size_t, double>
{
    return mCameraPosesErrors;
}

auto EstimatePosesTask::report() const -> OrientationReport
{
    return mOrientationReport;
}

void EstimatePosesTask::setMinCommonImages(int minCommonImages)
{
    mMinCommonImages = minCommonImages;
}

void EstimatePosesTask::setRobustAlignment(bool robustAlignment)
{
    mRobustAlignment = robustAlignment;
}

void EstimatePosesTask::setRobustAlignmentMaxError(double robustAlignmentMaxError)
{
    mRobustAlignmentMaxError = robustAlignmentMaxError;
}

void EstimatePosesTask::clear()
{
    mMinCommonImages = absolute_orientation_min_common_images;
    mRobustAlignment = absolute_orientation_robust_alignment;
    mRobustAlignmentMaxError = absolute_orientation_robust_alignment_max_error;
}

void EstimatePosesTask::stop()
{
    //Task::stop();

    //if (mMapper && mMapper->IsRunning())
    //    mMapper->Stop();

    ba_terminate = true;
}

void EstimatePosesTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{
    try {

        auto reconstruction_manager = std::make_shared<colmap::ReconstructionManager>();

        if (!mOutputPath.exists() && !mOutputPath.createDirectories()) {
            TL_THROW_EXCEPTION("Directory couldn't be created: {}", mOutputPath.toString());
        }

        if (mMapper) {
            delete mMapper;
            mMapper = nullptr;
        }

        auto database = colmap::Database::Open(mDatabase.toUtf8());

        if (mOptions.isEnabled(Options::orientation_global)) {

            tl::Message::info("Starting Global SfM Pipeline...");

            colmap::GlobalPipelineOptions options;
            mMapper = new colmap::GlobalPipeline(options, database, reconstruction_manager);

        } else {

            tl::Message::info("Starting Incremental SfM Pipeline...");

            auto options = std::make_shared<colmap::IncrementalPipelineOptions>();
            options->ba_refine_focal_length = !mOptions.isEnabled(Options::fix_calibration);
            options->ba_refine_principal_point = false;
            options->ba_refine_extra_params = !mOptions.isEnabled(Options::fix_calibration);

            if (mOptions.isEnabled(Options::use_poses)) {

                options->use_prior_position = true;
                options->use_robust_loss_on_prior_position = true;
                options->prior_position_loss_scale = 1.0;

                if (mOptions.isDisabled(Options::use_rtk_positioning_accuracy)) {

                    /// TODO: modificar PosePrior

                //    auto database = colmap::Database::Open(mDatabase.toUtf8());
                //    colmap::DatabaseTransaction transaction(database.get());

                //    for (const auto &[image_id, image] : mImageRepo) {

                //        if (image.cameraPose().isEmpty()) continue;

                //        std::string image_path = image.path().toString();
                //        colmap::image_t colmap_image_id{};

                //        if (database->ExistsImageWithName(image_path)) {

                //            auto colmap_image = database->ReadImageWithName(image_path);
                //            if (colmap_image) {

                //                auto colmap_image_id = colmap_image->ImageId();


                //        if (database->ExistsPosePrior(colmap_id)) {
                //            auto prior = database.ReadPosePrior(colmap_id);

                //            // Determinar desviación estándar (std) según el rtkFlag individual
                //            Eigen::Vector3d accuracy{10.0, 10.0, 10.0}; // Por defecto: Single / Standalone GPS

                //            switch (image.cameraPose().rtkFlag()) {
                //            case 50: // RTK Fix
                //                accuracy = Eigen::Vector3d(0.01, 0.01, 0.03);
                //                break;
                //            case 34: // RTK Float
                //                accuracy = Eigen::Vector3d(0.2, 0.2, 0.5);
                //                break;
                //            case 16: // Single GPS
                //            default:
                //                accuracy = Eigen::Vector3d(10.0, 10.0, 10.0);
                //                break;
                //            }

                //            // Construir la nueva matriz de covarianza
                //            Eigen::Matrix3d cov = Eigen::Matrix3d::Zero();
                //            cov(0, 0) = accuracy.x() * accuracy.x();
                //            cov(1, 1) = accuracy.y() * accuracy.y();
                //            cov(2, 2) = accuracy.z() * accuracy.z();

                //            prior.position_covariance = cov;
                //            database->UpdatePosePrior(prior);
                //        }
                //    }
                }
            }

            mMapper = new colmap::IncrementalPipeline(options, database, reconstruction_manager);

            // TODO: Comprobar si esto sigue siendo correcto
            size_t prev_num_reconstructions = 0;
            mMapper->AddCallback(colmap::IncrementalPipeline::LAST_IMAGE_REG_CALLBACK, [&]() {
                try {

                    // If the number of reconstructions has not changed, the last model
                    // was discarded for some reason.
                    if (reconstruction_manager->Size() > prev_num_reconstructions) {
                        //const std::string reconstruction_path = mOutputPath.toString();
                        //const auto &reconstruction = reconstruction_manager->Get(prev_num_reconstructions);
                        //colmap::CreateDirIfNotExists(reconstruction_path);
                        //reconstruction.Write(reconstruction_path);
                        //mOptions->Write(JoinPaths(reconstruction_path, "project.ini"));
                        ///TODO: Por ahora sólo trabajamos con una reconstrucción
                        //prev_num_reconstructions = reconstruction_manager->Size();
                    }

                } catch (std::exception &e) {
                    tl::printException(e);
                }
            });

            mMapper->AddCallback(colmap::IncrementalPipeline::NEXT_IMAGE_REG_CALLBACK, [&]() {
                //if (progressBar) (*progressBar)();
                //msgInfo("-----");
            });
        }

        mMapper->Run();

        if (status() == Status::stopping || stopToken.stop_requested()) {
            return;
        }

        TL_ASSERT(reconstruction_manager->Size() > 0, "Reconstruction failed: No models were generated");

        // 6. Obtención del modelo principal reconstruido
        std::shared_ptr<colmap::Reconstruction> reconstruction = reconstruction_manager->Get(0);
        const std::vector<colmap::image_t> &reg_image_ids = reconstruction->RegImageIds();

        TL_ASSERT(reg_image_ids.size() >= 2, "Need at least two registered views.");

        // Limpieza básica de observaciones inválidas
        //reconstruction->FilterObservationsWithNegativeDepth();
        // Ver RunPointFiltering

        // 7. Ajuste de Haces Final (Global Bundle Adjustment)
        colmap::OptionManager option_manager;
        option_manager.bundle_adjustment->refine_focal_length = !mOptions.isEnabled(Options::fix_calibration);
        option_manager.bundle_adjustment->refine_principal_point = !mOptions.isEnabled(Options::fix_calibration);
        option_manager.bundle_adjustment->refine_extra_params = !mOptions.isEnabled(Options::fix_calibration);

        colmap::BundleAdjustmentOptions ba_options = *option_manager.bundle_adjustment;
        ba_options.print_summary = false;

        colmap::BundleAdjustmentConfig ba_config;
        for (const colmap::image_t image_id : reg_image_ids) {
            ba_config.AddImage(image_id);
        }

        if (!mOptions.isEnabled(Options::use_poses)) {
            ba_config.FixGauge(colmap::BundleAdjustmentGauge::TWO_CAMS_FROM_WORLD);
        }

        std::shared_ptr<colmap::BundleAdjustmentSummary> summary;
        for (int i = 0; i < 5; ++i) {
            ba_terminate = false;

            std::unique_ptr<colmap::BundleAdjuster> bundle_adjuster;

            if (mOptions.isEnabled(Options::use_poses)) {

                colmap::PosePriorBundleAdjustmentOptions prior_options;
                prior_options.ceres->prior_position_loss_function_type = colmap::CeresBundleAdjustmentOptions::LossFunctionType::CAUCHY;
                prior_options.ceres->prior_position_loss_scale = 1.0;

                auto pose_priors = database->ReadAllPosePriors();
                bundle_adjuster = colmap::CreatePosePriorBundleAdjuster(ba_options, prior_options, ba_config, pose_priors, *reconstruction);
            } else {
                bundle_adjuster = colmap::CreateDefaultBundleAdjuster(ba_options, ba_config, *reconstruction);
            }

            summary = bundle_adjuster->Solve();

            if (status() == Status::stopping || stopToken.stop_requested()) return;

            ba_terminate = true;

            if (summary && summary->IsSolutionUsable()) break;
        }

        if (summary && summary->IsSolutionUsable()) {
            auto ceres_summary_ptr = std::dynamic_pointer_cast<colmap::CeresBundleAdjustmentSummary>(summary);

            if (ceres_summary_ptr) {
                const auto &cs = ceres_summary_ptr->ceres_summary;

                mOrientationReport.iterations = cs.num_successful_steps + cs.num_unsuccessful_steps;

                if (cs.num_residuals_reduced > 0) {
                    mOrientationReport.initialCost = std::sqrt(cs.initial_cost / cs.num_residuals_reduced);
                    mOrientationReport.finalCost = std::sqrt(cs.final_cost / cs.num_residuals_reduced);
                }
            } else {
                mOrientationReport.iterations = 0;
                mOrientationReport.initialCost = 0.0;
                mOrientationReport.finalCost = 0.0;
            }
            mOrientationReport.termination = "CONVERGENCE";
        }
        mOrientationReport.time = this->time();

        TL_ASSERT(summary->termination_type == colmap::BundleAdjustmentTerminationType::CONVERGENCE, "Bundle adjustment failed to converge");

        tl::Message::success("Relative orientation finished in {:.2f} minutes", mOrientationReport.time / 60.0);

        // 8. Exportación y Generación de informes
        if (mOptions.isEnabled(Options::orientation_global)) {
            reconstruction->ExtractColorsForAllImages("");
        }

        ColmapReconstructionConvert convert(reconstruction.get(), mImageRepo.all());

        // Puntos del terreno (Puntos 3D triangulados)
        std::vector<GroundPoint> ground_points = convert.groundPoints();
        auto gp_writer = GroundPointsWriterFactory::create("GRAPHOS");
        gp_writer->setGroundPoints(ground_points);
        tl::Path ground_points_path(mOutputPath);
        ground_points_path.append("ground_points.bin");
        gp_writer->write(ground_points_path);

        // Posiciones / Poses calculadas
        auto camera_poses = convert.cameraPoses();
        auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
        poses_writer->setCameraPoses(camera_poses);
        tl::Path poses_path(mOutputPath);
        poses_path.append("cameraPosesFile.bin");
        poses_writer->write(poses_path);

        // Actualización de cámaras
        for (auto &[camera_id, camera] : mCameraRepo) {

            std::shared_ptr<Calibration> calibration = convert.readCalibration(camera_id);

            if (calibration) {
                camera.setCalibration(calibration);
            }
        }

        // Exportación de la nube dispersa en PLY
        tl::Path sparse_path(mOutputPath);
        sparse_path.append("sparse.ply");
        OrientationExport orientation_export(reconstruction.get());
        orientation_export.exportPLY(sparse_path);

        if (progressBar) (*progressBar)();

        mOrientationReport.time = this->time();
        tl::Message::success("3D reconstruction finished in {:.2f} minutes", mOrientationReport.time / 60.0);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Orientation estimation error");
    }
}

// Codigo para Colmap 3.7
//void EstimatePosesTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
//{
//
//    try {
//
//        mReconstructionManager->Clear();
//
//        if (!mOutputPath.exists() && !mOutputPath.createDirectories()) {
//            TL_THROW_EXCEPTION("Directory couldn't be created: {}", mOutputPath.toString());
//        }
//
//        if (mMapper) {
//            delete mMapper;
//            mMapper = nullptr;
//        }
//
//        /// ¿Si se establece como fija la calibración?
//        mIncrementalMapperOptions->ba_refine_focal_length = !mOptions.isEnabled(Options::fix_calibration);
//        mIncrementalMapperOptions->ba_refine_principal_point = false;
//        mIncrementalMapperOptions->ba_refine_extra_params = !mOptions.isEnabled(Options::fix_calibration);
//
//        mMapper = new colmap::IncrementalMapperController(mIncrementalMapperOptions, "", mDatabase.toUtf8(), mReconstructionManager.get());
//
//        size_t prev_num_reconstructions = 0;
//        mMapper->AddCallback(colmap::IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [&]() {
//
//                try {
//
//                    // If the number of reconstructions has not changed, the last model
//                    // was discarded for some reason.
//                    if (mReconstructionManager->Size() > prev_num_reconstructions) {
//                        const std::string reconstruction_path = mOutputPath.toString();
//                        const auto &reconstruction = mReconstructionManager->Get(prev_num_reconstructions);
//                        //colmap::CreateDirIfNotExists(reconstruction_path);
//                        //reconstruction.Write(reconstruction_path);
//                        //mOptions->Write(JoinPaths(reconstruction_path, "project.ini"));
//                        ///TODO: Por ahora sólo trabajamos con una reconstrucción
//                        //prev_num_reconstructions = mReconstructionManager->Size();
//                    }
//
//                } catch (std::exception &e) {
//                    tl::printException(e);
//                }
//
//            });
//
//        mMapper->AddCallback(colmap::IncrementalMapperController::NEXT_IMAGE_REG_CALLBACK, [&]() {
//                //if (progressBar) (*progressBar)();
//                //msgInfo("-----");
//            });
//
//        mMapper->Start();
//        mMapper->Wait();
//
//        if (status() == Status::stopping) return;
//
//        TL_ASSERT(mReconstructionManager->Size() > 0, "Reconstruction fail");
//
//        colmap::OptionManager option_manager;
//        option_manager.bundle_adjustment->refine_focal_length = !mOptions.isEnabled(Options::fix_calibration);
//        option_manager.bundle_adjustment->refine_principal_point = !mOptions.isEnabled(Options::fix_calibration);
//        option_manager.bundle_adjustment->refine_extra_params = !mOptions.isEnabled(Options::fix_calibration);
//
//        colmap::Reconstruction &reconstruction = mReconstructionManager->Get(0);
//
//        const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();
//
//        TL_ASSERT(reg_image_ids.size() >= 2, "Need at least two views.");
//
//        // Avoid degeneracies in bundle adjustment.
//        reconstruction.FilterObservationsWithNegativeDepth();
//
//        colmap::BundleAdjustmentOptions ba_options = *option_manager.bundle_adjustment;
//        ba_options.solver_options.minimizer_progress_to_stdout = false;
//        ba_options.solver_options.logging_type = ceres::LoggingType::SILENT;
//
//        BundleAdjustmentIterationCallback iteration_callback;
//        ba_options.solver_options.callbacks.push_back(&iteration_callback);
//
//        // Configure bundle adjustment.
//        BundleAdjustmentConfig ba_config;
//        for (const colmap::image_t image_id : reg_image_ids) {
//            ba_config.AddImage(image_id);
//        }
//        ba_config.SetConstantPose(reg_image_ids[0]);
//        ba_config.SetConstantTvec(reg_image_ids[1], {0});
//
//        ceres::Solver::Summary summary;
//
//        //{
//        //    ba_terminate = false;
//
//        //    BundleAdjuster bundle_adjuster(ba_options, ba_config);
//        //    bundle_adjuster.solve(&reconstruction);
//
//        //    if (status() == Status::stopping) return;
//
//        //    ba_terminate = true;
//        //    summary = bundle_adjuster.summary();
//        //}
//
//        // TODO: Igual mejor que sea un bucle
//        //if (summary.termination_type == ceres::NO_CONVERGENCE) {
//
//        //    ba_terminate = false;
//
//        //    BundleAdjuster bundle_adjuster(ba_options, ba_config);
//        //    bundle_adjuster.solve(&reconstruction);
//
//        //    if (status() == Status::stopping) return;
//
//        //    ba_terminate = true;
//        //    summary = bundle_adjuster.summary();
//        //}
//        // Ajustar en función del numero de imagenes
//        for (int i = 0; i < 5; ++i) {
//            ba_terminate = false;
//
//            BundleAdjuster bundle_adjuster(ba_options, ba_config);
//            bundle_adjuster.solve(&reconstruction);
//
//            if (status() == Status::stopping) return;
//
//            ba_terminate = true;
//            summary = bundle_adjuster.summary();
//
//            if (summary.termination_type == ceres::CONVERGENCE) break;
//        }
//
//        mOrientationReport.iterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
//        mOrientationReport.initialCost = std::sqrt(summary.initial_cost / summary.num_residuals_reduced);
//        mOrientationReport.finalCost = std::sqrt(summary.final_cost / summary.num_residuals_reduced);
//        mOrientationReport.termination = "CONVERGENCE";
//        mOrientationReport.time = this->time();
//
//        TL_ASSERT(summary.termination_type == ceres::CONVERGENCE, "Bundle adjust: NO CONVERGENCE");
//
//        if (status() == Status::stopping) return;
//
//        tl::Message::success("Relative orientation finished in {:.2} minutes", mOrientationReport.time / 60.);
//
//        
//
//        if (mOptions.isEnabled(Options::absolute_orientation)) {
//
//            /// Conversión de identificadores de imagen de GRAPHOS a COLMAP
//            std::unordered_map<uint32_t, size_t> image_ids_colmap_to_graphos;
//            std::unordered_map<size_t, uint32_t> image_ids_graphos_to_colmap;
//
//            for (const auto &image : mImages) {
//
//                tl::Path image_path(image.path().toStdString());
//                size_t image_id = Image::id(image);
//                for (const auto &colmap_image : reconstruction.Images()) {
//                    tl::Path colmap_image_path(colmap_image.second.Name());
//
//                    if (image_path.equivalent(colmap_image_path)) {
//                        image_ids_colmap_to_graphos[colmap_image.first] = image_id;
//                        image_ids_graphos_to_colmap[image_id] = colmap_image.first;
//                        break;
//                    }
//                }
//            }
//
//            double robust_fitting_error = 2.0;
//            std::unordered_map<size_t, tl::Point3d> cameras_enu;
//
//            if (mOptions.isEnabled(Options::use_poses)) {
//
//                std::unordered_map<size_t, tl::Point3d> cameras_geographic;
//                
//                for (const auto &image : mImages) {
//                
//                    size_t image_id = Image::id(image);
//                    if (image_ids_graphos_to_colmap[image_id] == 0) continue;
//                
//                    // Conversion a coordendas ENU
//                
//                    if (image.cameraPose().isEmpty()) continue;
//                
//                    auto camera_epsg = image.cameraPose().crs();
//                    auto camera_crs = std::make_shared<tl::Crs>(camera_epsg.toStdString());
//                    TL_ASSERT(camera_crs->isValid(), "Invalid CRS: {}", camera_epsg.toStdString());
//                
//                    auto epsg_code = camera_epsg.toStdString();
//                    auto camera_coordinates = image.cameraPose().position();
//                    if (epsg_code != "EPSG:4326")
//                        mGeoTools->ptrCRSsTools()->crsOperation(epsg_code, "EPSG:4326", camera_coordinates.x, camera_coordinates.y, camera_coordinates.z);
//                
//                    cameras_geographic[image_id] = camera_coordinates;
//                }
//
//
//                /// Cálculo del centro
//
//                tl::Point3d geographic_center;
//                for (const auto &coordinates : cameras_geographic) {
//                    geographic_center += coordinates.second / static_cast<double>(cameras_geographic.size());
//                }
//
//                mEnuCrs = mGeoTools->ptrCRSsTools()->getCRSEnu("EPSG:4326", geographic_center.x, geographic_center.y, geographic_center.z);
//
//                for (const auto &geographic_coordinates : cameras_geographic) {
//                    auto image_id = geographic_coordinates.first;
//                    auto coordinates = geographic_coordinates.second;
//                    mGeoTools->ptrCRSsTools()->crsOperation("EPSG:4326", mEnuCrs, coordinates.x, coordinates.y, coordinates.z);
//                    cameras_enu[image_id] = coordinates;
//                }
//
//
//                /// Transformación de semejanza
//                bool robust_alignment = mRobustAlignment;
//                colmap::RANSACOptions ransac_options;
//                ransac_options.max_error = mRobustAlignmentMaxError;
//
//                if (robust_alignment && ransac_options.max_error <= 0) {
//                    throw std::runtime_error("Similarity transformation: You must provide a maximum alignment error > 0");
//                }
//
//                std::vector<std::string> ref_image_names;
//                std::vector<Eigen::Vector3d> ref_locations;
//
//                for (const auto &camera_enu : cameras_enu) {
//                    auto graphos_id = camera_enu.first;
//                    auto enu_coordinates = camera_enu.second;
//                    auto colmap_id = image_ids_graphos_to_colmap[graphos_id];
//                    const auto &image = reconstruction.Image(colmap_id);
//                    ref_image_names.push_back(image.Name());
//
//                    Eigen::Vector3d camera_coordinates;
//                    camera_coordinates[0] = enu_coordinates.x;
//                    camera_coordinates[1] = enu_coordinates.y;
//                    camera_coordinates[2] = enu_coordinates.z;
//
//                    ref_locations.push_back(camera_coordinates);
//                }
//
//                if (status() == Status::stopping) return;
//
//                bool alignment_success;
//                if (robust_alignment) {
//                    alignment_success = reconstruction.AlignRobust(ref_image_names, ref_locations, mMinCommonImages, ransac_options);
//                } else {
//                    alignment_success = reconstruction.Align(ref_image_names, ref_locations, mMinCommonImages);
//                }
//
//                if (status() == Status::stopping) return;
//
//                TL_ASSERT(alignment_success, "Similarity transformation: Alignment failed");
//
//
//                std::vector<double> errors;
//                errors.reserve(ref_image_names.size());
//
//                for (size_t i = 0; i < ref_image_names.size(); ++i) {
//                    const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
//                    if (image != nullptr) {
//                        double error = (image->ProjectionCenter() - ref_locations[i]).norm();
//                        errors.push_back(error);
//                    }
//                }
//
//                robust_fitting_error = colmap::Median(errors);
//
//                tl::Message::info("Similarity transformation: Alignment error: {} (mean), {} (median)", colmap::Mean(errors), colmap::Median(errors));
//
//                if (status() == Status::stopping) return;
//
//            }
//
//            //} 
//
//
//            // RTK
//            // - Se añaden las camaras al ajuste de haces
//            //BundleAdjustmentConfig ba_config;
//            //for (const colmap::image_t image_id : reconstruction.RegImageIds()) {
//            //    ba_config.AddImage(image_id);
//            //    ba_config.setCameraPositionAccuracy(image_id, 0.01);
//            //}
//            // En OpenMVG utiliza la mediana del error calculado en la transformacíón de semejanza
//            // problem.AddResidualBlock(
//            //     cost_function,
//            //     new ceres::HuberLoss(
//            //         Square(pose_center_robust_fitting_error)),
//            //     &map_poses.at(prior->id_view)[0]);
//            // Aqui se obtiene con colmap::Median(errors)
//            // Habría que aplicarlo como
//            // ceres::CostFunction *control_point_cost_function = ControlPointCostFunction::create(point3D, weight_cp);
//            //problem_->AddResidualBlock(control_point_cost_function, new ceres::HuberLoss(Square(colmap::Median(errors))), gcp.point.data());
//
//            //TODO: Este if es redundante, ya se hace mas arriba
//            if (mOptions.isEnabled(Options::absolute_orientation)) {
//
//                /// Lectura de los puntos de control
//
//                std::vector<GCP> control_points_enu;
//                std::vector<GroundControlPoint> ground_control_points;
//                std::string ground_control_points_crs;
//
//                if (mOptions.isEnabled(Options::use_gcp)) {
//
//
//                    tl::Path gcp_file = mOutputPath;
//                    gcp_file.append("georef.xml");
//                    auto gcp_reader = GCPsReaderFactory::create("GRAPHOS");
//                    gcp_reader->read(gcp_file);
//                    ground_control_points = gcp_reader->gcps();
//                    ground_control_points_crs = gcp_reader->epsgCode();
//
//                    TL_ASSERT(!ground_control_points_crs.empty(), "Unknow CRS for ground control points");
//
//                    if (mEnuCrs.empty()) {
//
//
//                        /// Cálculo del centro
//                        tl::Point3d geographic_center;
//                        double i = 1.;
//                        for (const auto &gcp : ground_control_points) {
//                            auto coordinates = gcp;
//                            if (ground_control_points_crs != "EPSG:4326")
//                                mGeoTools->ptrCRSsTools()->crsOperation(ground_control_points_crs, "EPSG:4326", coordinates.x, coordinates.y, coordinates.z);
//                            geographic_center += coordinates / static_cast<double>(ground_control_points.size());
//                        }
//
//                        mEnuCrs = mGeoTools->ptrCRSsTools()->getCRSEnu("EPSG:4326", geographic_center.x, geographic_center.y, geographic_center.z);
//
//                    }
//
//                    std::vector<Eigen::Vector3d> src;
//                    std::vector<Eigen::Vector3d> dst;
//                    std::vector<std::string> gcp_name;
//
//                    colmap::IncrementalTriangulator::Options options;
//
//                    // Setup estimation options.
//                    colmap::EstimateTriangulationOptions tri_options;
//                    tri_options.min_tri_angle = colmap::DegToRad(options.min_angle);
//                    tri_options.residual_type = colmap::TriangulationEstimator::ResidualType::REPROJECTION_ERROR;
//                    tri_options.ransac_options.max_error = options.complete_max_reproj_error;
//                    tri_options.ransac_options.confidence = 0.9999;
//                    tri_options.ransac_options.min_inlier_ratio = 0.02;
//                    tri_options.ransac_options.max_num_trials = 10000;
//
//
//                    colmap::Database database;
//                    database.Open(mDatabase.toUtf8());
//
//                    for (auto &ground_control_point : ground_control_points) {
//
//                        std::vector<colmap::TriangulationEstimator::PointData> points_data;
//                        std::vector<colmap::TriangulationEstimator::PoseData> poses_data;
//
//                        for (auto &camera : reconstruction.Cameras()) {
//
//                            for (auto &image : reconstruction.Images()) {
//
//                                if (image.second.CameraId() == camera.second.CameraId()) {
//
//                                    auto &track = ground_control_point.track();
//
//                                    if (track.existPoint(image_ids_colmap_to_graphos[image.second.ImageId()])) {
//
//                                        tl::Point<double> point = track.point(image_ids_colmap_to_graphos[image.second.ImageId()]);
//
//                                        colmap::TriangulationEstimator::PointData point_data;
//                                        point_data.point = Eigen::Vector2d(point.x, point.y);
//                                        point_data.point_normalized = camera.second.ImageToWorld(point_data.point);
//                                        points_data.push_back(point_data);
//
//                                        colmap::TriangulationEstimator::PoseData pose_data;
//                                        pose_data.proj_matrix = image.second.ProjectionMatrix();
//                                        pose_data.proj_center = image.second.ProjectionCenter();
//                                        pose_data.camera = &camera.second;
//                                        poses_data.push_back(pose_data);
//                                    }
//
//                                }
//                            }
//                        }
//
//                        if (points_data.size() < 2) continue;
//
//                        Eigen::Vector3d xyz;
//                        std::vector<char> inlier_mask;
//                        if (colmap::EstimateTriangulation(tri_options, points_data, poses_data, &inlier_mask, &xyz)) {
//
//                            src.push_back(xyz);
//
//                            auto gcps_enu = ground_control_point;
//                            //if (ground_control_points_crs != "EPSG:4326")
//                                mGeoTools->ptrCRSsTools()->crsOperation(ground_control_points_crs, mEnuCrs, gcps_enu.x, gcps_enu.y, gcps_enu.z);
//
//                            GCP _gcp;
//                            _gcp.point = {gcps_enu.x, gcps_enu.y, gcps_enu.z};
//                            _gcp.track = ground_control_point.track();
//                            _gcp.name = ground_control_point.name();
//                            control_points_enu.push_back(_gcp);
//
//                            dst.emplace_back(gcps_enu.x, gcps_enu.y, gcps_enu.z);
//                            gcp_name.push_back(ground_control_point.name());
//                        }
//
//                    }
//
//                    /// Esto solo si no tenemos datos GNSS
//                    
//                    if (mOptions.isDisabled(Options::use_poses)) {
//
//                        TL_ASSERT(src.size() > 3, "Similarity transformation: Insufficient number of points");
//
//                        colmap::SimilarityTransform3 similarity_transform;
//                        similarity_transform.Estimate(src, dst);
//
//                        reconstruction.Transform(similarity_transform);
//
//                        std::vector<double> errors;
//                        errors.reserve(dst.size());
//
//                        for (size_t i = 0; i < dst.size(); ++i) {
//                            similarity_transform.TransformPoint(&src.at(i));
//                            errors.push_back((src[i] - dst[i]).norm());
//                        }
//
//                        //robust_fitting_error = colmap::Median(errors);
//
//                        tl::Message::info("Georeference error: {} (mean), {} (median)", colmap::Mean(errors), colmap::Median(errors));
//
//                    }
//                }
//
//                // Configure bundle adjustment.
//                BundleAdjustmentConfig ba_config;
//                ba_config.setRobustFittingError(robust_fitting_error);
//                for (const colmap::image_t image_id : reg_image_ids) {
//                    ba_config.AddImage(image_id);
//                    if (mOptions.isEnabled(Options::use_poses)){
//
//                        size_t image_graphos_id = image_ids_colmap_to_graphos[image_id];
//                        tl::Vector3d accuracy{10., 10., 10.};
//                        for (auto &image : mImages) {
//                            if (Image::id(image) == image_graphos_id) {
//                                auto camera_pose = image.cameraPose();
//                                if (!camera_pose.isEmpty()) {
//                                    if (mOptions.isEnabled(Options::use_rtk_positioning_accuracy)) {
//                                        accuracy = camera_pose.accuracy();
//                                    } else if (camera_pose.rtkFlag() == 50){
//                                        accuracy = {0.01, 0.01, 0.03};
//                                        //accuracy = {0.01, 0.01, 0.01};
//                                    } else if (camera_pose.rtkFlag() == 34){
//                                        accuracy = {0.2, 0.2, 0.5};
//                                    } else if (camera_pose.rtkFlag() == 16) {
//                                        accuracy = {10., 10., 10.};
//                                    }
//                                }
//                                break;
//                            }
//                        }
//
//                        ba_config.setCameraPositionAccuracy(image_id, cameras_enu[image_graphos_id], accuracy);
//
//                    }
//                }
//
//
//                if (mOptions.isEnabled(Options::use_gcp)) {
//                    TL_ASSERT(control_points_enu.size() > 3 || mOptions.isEnabled(Options::use_poses), "A minimum of 3 ground control points is required");
//                    if (control_points_enu.size() > 3) {
//                        ba_config.setGroundControlPoints(control_points_enu);
//                    }
//                }
//
//                ba_config.setImageIdsGraphosToColmap(image_ids_graphos_to_colmap); //Por ahora...
//                ba_config.setImageIdsColmapToGraphos(image_ids_colmap_to_graphos);
//
//                /// Configuración de OpenMVG
//
//                colmap::BundleAdjustmentOptions ba_options;
//                ba_options.solver_options.logging_type = ceres::LoggingType::SILENT;
//                //ba_options.solver_options.function_tolerance = 0.0;
////                ba_options.solver_options.gradient_tolerance = 1e-10;
////                ba_options.solver_options.parameter_tolerance = 1e-8;
//                ba_options.solver_options.minimizer_progress_to_stdout = false;
////                ba_options.solver_options.max_num_iterations = 50;
////                ba_options.solver_options.max_linear_solver_iterations = 500;
////                ba_options.solver_options.max_num_consecutive_invalid_steps = 10;
////                ba_options.solver_options.max_consecutive_nonmonotonic_steps = 10;
//                ba_options.solver_options.num_threads = 4; // 1;
////#if CERES_VERSION_MAJOR < 2
////                solver_options.num_linear_solver_threads = -1;
////#endif  // CERES_VERSION_MAJOR
//                //ba_options.loss_function_type = colmap::BundleAdjustmentOptions::LossFunctionType::CAUCHY;
//                //ba_options.loss_function_scale = 4.;
//                //ba_options.refine_focal_length = false;// true;
//                //ba_options.refine_principal_point = false;
//                //ba_options.refine_extra_params = false;// true;
//
//                ceres::Solver::Summary summary;
//                
//                for (size_t i = 0; i < 5; i++) {
//                    ba_terminate = false;
//
//                    BundleAdjuster bundle_adjuster(ba_options, ba_config);
//                    bundle_adjuster.solve(&reconstruction);
//                
//                    if (status() == Status::stopping) return;
//                
//                    ba_terminate = true;
//                    summary = bundle_adjuster.summary();
//
//                    if (summary.termination_type == ceres::CONVERGENCE) break;
//                }
//
//                // Calculo de los errores en el ajuste de haces
//
//                if (mOptions.isEnabled(Options::use_gcp)){
//
//                    std::vector<Eigen::Vector3d> src;
//                    std::vector<Eigen::Vector3d> dst;
//                    std::vector<std::string> gcp_name;
//
//                    colmap::IncrementalTriangulator::Options options;
//
//                    // Setup estimation options.
//                    colmap::EstimateTriangulationOptions tri_options;
//                    tri_options.min_tri_angle = colmap::DegToRad(options.min_angle);
//                    tri_options.residual_type = colmap::TriangulationEstimator::ResidualType::REPROJECTION_ERROR;
//                    tri_options.ransac_options.max_error = options.complete_max_reproj_error;
//                    tri_options.ransac_options.confidence = 0.9999;
//                    tri_options.ransac_options.min_inlier_ratio = 0.02;
//                    tri_options.ransac_options.max_num_trials = 10000;
//
//                    for (size_t i = 0; i < control_points_enu.size(); ++i) {
//
//                        std::vector<colmap::TriangulationEstimator::PointData> points_data;
//                        std::vector<colmap::TriangulationEstimator::PoseData> poses_data;
//
//                        for (auto &camera : reconstruction.Cameras()) {
//
//                            for (auto &image : reconstruction.Images()) {
//
//                                if (image.second.CameraId() == camera.second.CameraId()) {
//
//                                    auto &track = control_points_enu[i].track;
//
//                                    size_t graphos_image_id = image_ids_colmap_to_graphos[image.second.ImageId()];
//                                    if (track.existPoint(graphos_image_id)) {
//
//                                        tl::Point<double> point = track.point(graphos_image_id);
//
//                                        colmap::TriangulationEstimator::PointData point_data;
//                                        point_data.point = Eigen::Vector2d(point.x, point.y);
//                                        point_data.point_normalized = camera.second.ImageToWorld(point_data.point);
//                                        points_data.push_back(point_data);
//
//                                        colmap::TriangulationEstimator::PoseData pose_data;
//                                        pose_data.proj_matrix = image.second.ProjectionMatrix();
//                                        pose_data.proj_center = image.second.ProjectionCenter();
//                                        pose_data.camera = &camera.second;
//                                        poses_data.push_back(pose_data);
//
//                                        /// Residuals. Lo calculo en coordenadas ENU
//                                        Eigen::Vector3d gcp(control_points_enu[i].point.x(), control_points_enu[i].point.y(), control_points_enu[i].point.z());
//                                        auto rotate_point = image.second.RotationMatrix() * gcp + image.second.Tvec();
//                                        auto x = rotate_point.x() / rotate_point.z();
//                                        auto y = rotate_point.y() / rotate_point.z();
//                                        auto residuals = camera.second.WorldToImage({x, y});
//                                        residuals -= point_data.point;
//                                        ground_control_points[i].addErrorToTrack(graphos_image_id, {residuals.x(), residuals.y()});
//                                    }
//
//                                }
//                            }
//                        }
//
//                        if (points_data.size() < 2) continue;
//
//                        Eigen::Vector3d xyz;
//                        std::vector<char> inlier_mask;
//                        if (colmap::EstimateTriangulation(tri_options, points_data, poses_data, &inlier_mask, &xyz)) {
//                            src.push_back(xyz);
//                            dst.emplace_back(control_points_enu[i].point.x(), control_points_enu[i].point.y(), control_points_enu[i].point.z());
//                            gcp_name.push_back(control_points_enu[i].name);
//                        }
//
//                    }
//
//
//                    std::vector<double> errors;
//                    errors.reserve(dst.size());
//
//                    for (size_t i = 0; i < dst.size(); ++i) {
//                        auto error = (src[i] - dst[i]).norm();
//                        ground_control_points[i].setError(error);
//                        errors.push_back(error);
//
//                        tl::Message::info("Ground Control Point {}: Error -> {}", gcp_name[i], errors[i]);
//                    }
//
//                    tl::Path gcp_file = mOutputPath;
//                    gcp_file.append("georef.xml");
//                    auto gcp_writer = GCPsWriterFactory::create("GRAPHOS");
//                    gcp_writer->setEPSGCode(ground_control_points_crs);
//                    gcp_writer->setGCPs(ground_control_points);
//                    gcp_writer->write(gcp_file);
//
//                    mOrientationReport.alignmentErrorMean = colmap::Mean(errors);
//                    mOrientationReport.alignmentErrorMedian = colmap::Median(errors);
//
//                    tl::Message::info("Ground Control Points error: {} (mean), {} (median)",
//                                      mOrientationReport.alignmentErrorMean, mOrientationReport.alignmentErrorMedian);
//
//
//                } 
//
//                if (mOptions.isEnabled(Options::use_poses)){
//
//                    std::vector<double> errors;
//                    errors.reserve(mImages.size());
//                    std::vector<double> errors_x;
//                    std::vector<double> errors_y;
//                    std::vector<double> errors_z;
//                    errors_x.reserve(mImages.size());
//                    errors_y.reserve(mImages.size());
//                    errors_z.reserve(mImages.size());
//
//                    for (const auto &image : mImages)
//                    {
//                        size_t graphos_image_id = Image::id(image);
//
//                        if (reconstruction.ExistsImage(image_ids_graphos_to_colmap[graphos_image_id])) {
//
//                            const colmap::Image &colmap_image = reconstruction.Image(image_ids_graphos_to_colmap[graphos_image_id]);
//                            tl::Point3d position = image.cameraPose().position();
//                            Eigen::Vector3d pos_ini;
//                            pos_ini[0] = cameras_enu[graphos_image_id].x;
//                            pos_ini[1] = cameras_enu[graphos_image_id].y;
//                            pos_ini[2] = cameras_enu[graphos_image_id].z;
//                            std::string accuracy;
//                            if (mOptions.isDisabled(Options::use_rtk_positioning_accuracy)) {
//                                accuracy = "10.";
//                            } else {
//                                accuracy = std::to_string(image.cameraPose().accuracy().x())
//                                           .append(" ")
//                                           .append(std::to_string(image.cameraPose().accuracy().y()))
//                                           .append(" ")
//                                           .append(std::to_string(image.cameraPose().accuracy().z()));
//                            }
//                            auto pos_final = colmap_image.ProjectionCenter();
//                            auto error_xyz = pos_final - pos_ini;
//                            double error = (error_xyz).norm();
//                            double error_x = std::abs(error_xyz.x());
//                            double error_y = std::abs(error_xyz.y());
//                            double error_z = std::abs(error_xyz.z());
//                            //tl::Message::info("Camera pose error: {} -> Error:{}", image.name().toStdString(), error);
//
//                            std::cout << std::fixed << std::setprecision(6)
//                                << image.name().toStdString() << "\t"
//                                << pos_ini.x() << "\t" << pos_ini.y() << "\t" << pos_ini.z() << "\t"
//                                << accuracy << "\t" << error << "\t"
//                                << error_xyz.x() << "\t" << error_xyz.y() << "\t" << error_xyz.z() << "\t"
//                                << pos_final.x() << "\t" << pos_final.y() << "\t" << pos_final.z()
//                                << std::endl;
//
//                            errors.push_back(error);
//                            errors_x.push_back(error_x);
//                            errors_y.push_back(error_y);
//                            errors_z.push_back(error_z);
//                        }
//                    }
//
//                    mOrientationReport.alignmentErrorMean = colmap::Mean(errors);
//                    mOrientationReport.alignmentErrorMedian = colmap::Median(errors);
//
//                    tl::Message::info("Alignment error: {} (mean), {} (median)", mOrientationReport.alignmentErrorMean, mOrientationReport.alignmentErrorMedian);
//
//                    std::cout << std::fixed << "Error x,y,z: [" << colmap::Median(errors_x) << ", " << colmap::Median(errors_y) << ", " << colmap::Median(errors_z) << "]" << std::endl;
//                }
//
//                // Fin calculo de errores en el ajuste de haces
//                
//                
//
//                mOrientationReport.iterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
//                mOrientationReport.initialCost = std::sqrt(summary.initial_cost / summary.num_residuals_reduced);
//                mOrientationReport.finalCost = std::sqrt(summary.final_cost / summary.num_residuals_reduced);
//                mOrientationReport.termination = "CONVERGENCE";
//
//                TL_ASSERT(summary.termination_type == ceres::CONVERGENCE, "Bundle adjust: NO CONVERGENCE");
//
//                if (status() == Status::stopping) return;
//
//                tl::Message::success("Absolute orientation finished in {:.2} minutes", (this->time() - mOrientationReport.time) / 60.);
//            }
//
//
//        }
//
//
//        /// Write Ground points
//
//        ColmapReconstructionConvert convert(&reconstruction, mImages);
//
//        std::vector<GroundPoint> ground_points = convert.groundPoints();
//        auto gp_writer = GroundPointsWriterFactory::create("GRAPHOS");
//        gp_writer->setGroundPoints(ground_points);
//        tl::Path ground_points_path(mOutputPath);
//        ground_points_path.append("ground_points.bin");
//        gp_writer->write(ground_points_path);
//
//        // Write Camera Poses
//
//        auto camera_poses = convert.cameraPoses();
//        auto poses_writer = CameraPosesWriterFactory::create("GRAPHOS");
//        poses_writer->setCameraPoses(camera_poses);
//        tl::Path poses_path(mOutputPath);
//        poses_path.append("poses.bin");
//        poses_writer->write(poses_path);
//
//        for (auto &camera : mCameras) {
//
//            std::shared_ptr<Calibration> calibration = convert.readCalibration(camera.first);
//
//            if (calibration) {
//                camera.second.setCalibration(calibration);
//            }
//        }
//
//        if (status() == tl::Task::Status::stopping) return;
//
//        /// Write Sparse Cloud
//
//        tl::Path sparse_path(mOutputPath);
//        sparse_path.append("sparse.ply");
//        OrientationExport orientation_export(&reconstruction);
//        orientation_export.exportPLY(sparse_path);
//
//
//        if (progressBar) (*progressBar)();
//
//        mOrientationReport.time = this->time();
//
//        tl::Message::success("3D reconstruction finished in {:.2} minutes", mOrientationReport.time / 60.);
//
//    } catch (...) {
//        TL_THROW_EXCEPTION_WITH_NESTED("Relative Orientation error");
//    }
//}

} // namespace graphos