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
                                       bool fixCalibration,
                                       bool absoluteOrientation,
                                       bool gps,
                                       bool controlPoints)
  : mFixCalibration(fixCalibration),
    mAbsoluteOrientation(absoluteOrientation),
    mGPS(gps),
    mControlPoints(controlPoints),
    mMinCommonImages(absolute_orientation_min_common_images),
    mRobustAlignment(absolute_orientation_robust_alignment),
    mRobustAlignmentMaxError(absolute_orientation_robust_alignment_max_error),
    mDatabase(std::move(database)),
    mOutputPath(std::move(outputPath)),
    mImages(images),
    mCameras(cameras),
    mIncrementalMapper(new colmap::IncrementalMapperOptions),
    mMapper(nullptr),
    mReconstructionManager(new colmap::ReconstructionManager)
{

}

ReconstructionTask::~ReconstructionTask()
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
            throw std::runtime_error(std::string("Directory couldn't be created: ").append(mOutputPath.toString()));
        }

        if (mMapper) {
            delete mMapper;
            mMapper = nullptr;
        }

        /// ¿Si se establece como fija la calibración?
        mIncrementalMapper->ba_refine_focal_length = !mFixCalibration;
        mIncrementalMapper->ba_refine_principal_point = false;
        mIncrementalMapper->ba_refine_extra_params = !mFixCalibration;

        mMapper = new colmap::IncrementalMapperController(mIncrementalMapper, "", mDatabase.toString(), mReconstructionManager.get());

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

        mMapper->Start();
        mMapper->Wait();

        if (status() == Status::stopping) return;

        TL_ASSERT(mReconstructionManager->Size() > 0, "Reconstruction fail");

        colmap::OptionManager option_manager;
        option_manager.bundle_adjustment->refine_focal_length = !mFixCalibration;
        option_manager.bundle_adjustment->refine_principal_point = !mFixCalibration;
        option_manager.bundle_adjustment->refine_extra_params = !mFixCalibration;

        colmap::Reconstruction &reconstruction = mReconstructionManager->Get(0);

        const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

        TL_ASSERT(reg_image_ids.size() >= 2, "Need at least two views.");

        // Avoid degeneracies in bundle adjustment.
        reconstruction.FilterObservationsWithNegativeDepth();

        colmap::BundleAdjustmentOptions ba_options = *option_manager.bundle_adjustment;
        ba_options.solver_options.minimizer_progress_to_stdout = true;
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

        {
            ba_terminate = false;

            BundleAdjuster bundle_adjuster(ba_options, ba_config);
            bundle_adjuster.solve(&reconstruction);

            if (status() == Status::stopping) return;

            ba_terminate = true;
            summary = bundle_adjuster.summary();
        }



        if (summary.termination_type == ceres::NO_CONVERGENCE) {

            ba_terminate = false;

            // Se vuelve a intentar el ajuste de haces
            /// Reemplazar por graphos::BundleAdjuster
            BundleAdjuster bundle_adjuster(ba_options, ba_config);
            bundle_adjuster.solve(&reconstruction);

            if (status() == Status::stopping) return;

            ba_terminate = true;
            summary = bundle_adjuster.summary();
        }

        mOrientationReport.iterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
        mOrientationReport.initialCost = std::sqrt(summary.initial_cost / summary.num_residuals_reduced);
        mOrientationReport.finalCost = std::sqrt(summary.final_cost / summary.num_residuals_reduced);
        mOrientationReport.termination = "CONVERGENCE";
        mOrientationReport.time = this->time();

        TL_ASSERT(summary.termination_type == ceres::CONVERGENCE, "Bundle adjust: NO CONVERGENCE");

        if (status() == Status::stopping) return;

        tl::Message::success("Relative orientation finished in {:.2} minutes", mOrientationReport.time / 60.);


        if (mAbsoluteOrientation) {

            /// Si se dispone de coordenadas GPS se hace una transformación de semejanza
            if (mGPS) {

                /// Transformación de semejanza
                bool robust_alignment = mRobustAlignment;
                colmap::RANSACOptions ransac_options;
                ransac_options.max_error = mRobustAlignmentMaxError;

                if (robust_alignment && ransac_options.max_error <= 0) {
                    throw std::runtime_error("ERROR: You must provide a maximum alignment error > 0");
                }

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

                for (auto &pos : ref_locations) {
                    pos -= offset;
                }

                if (status() == Status::stopping) return;

                bool alignment_success;
                if (robust_alignment) {
                    alignment_success = reconstruction.AlignRobust(ref_image_names, ref_locations, mMinCommonImages, ransac_options);
                } else {
                    alignment_success = reconstruction.Align(ref_image_names, ref_locations, mMinCommonImages);
                }

                if (status() == Status::stopping) return;

                TL_ASSERT(alignment_success, "Absolute Orientation failed");


                std::vector<double> errors;
                errors.reserve(ref_image_names.size());

                for (size_t i = 0; i < ref_image_names.size(); ++i) {
                    const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
                    if (image != nullptr) {
                        double error = (image->ProjectionCenter() - ref_locations[i]).norm();
                        errors.push_back(error);
                    }
                }

                tl::Message::info("Alignment error: {} (mean), {} (median)", colmap::Mean(errors), colmap::Median(errors));

                if (status() == Status::stopping) return;

                //tl::Message::success("Absolute orientation finished in {:.2} minutes", (this->time() - mOrientationReport.time) / 60.);

                /// writeOffset

                tl::Path offset_path = mOutputPath;
                offset_path.append("offset.txt");
                offsetWrite(offset_path, tl::Point3<double>(offset[0], offset[1], offset[2]));
                tl::Message::info("Camera offset: {},{},{}", offset[0], offset[1], offset[2]);
            } 

            if (mControlPoints) {


                // Si no se dispone de datos GPS de las imágenes se hace una trans 
                if (!mGPS) {

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
                    database.Open(mDatabase.toString());

                    const auto &colmap_images = database.ReadAllImages();

                    std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;
                    std::unordered_map<colmap::image_t, size_t> colmap_to_graphos_image_ids;
                    for (const auto &image : mImages) {

                        tl::Path image_path(image.second.path().toStdString());

                        for (const auto &colmap_image : colmap_images) {
                            tl::Path colmap_image_path(colmap_image.Name());

                            if (image_path.equivalent(colmap_image_path)) {
                                graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
                                colmap_to_graphos_image_ids[colmap_image.ImageId()] = image.first;
                                break;
                            }
                        }

                    }

                    for (auto ground_control_point : mGroundControlPoints) {

                        std::vector<colmap::TriangulationEstimator::PointData> points_data;
                        std::vector<colmap::TriangulationEstimator::PoseData> poses_data;

                        for (auto &camera : reconstruction.Cameras()) {

                            for (auto &image : reconstruction.Images()) {

                                if (image.second.CameraId() == camera.second.CameraId()) {

                                    QString base_name = QFileInfo(image.second.Name().c_str()).baseName();
                                    auto &track = ground_control_point.track();

                                    if (track.existPoint(colmap_to_graphos_image_ids[image.second.ImageId()])) {
                                        tl::Point<double> point = track.point(colmap_to_graphos_image_ids[image.second.ImageId()]);

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
                            dst.emplace_back(ground_control_point.x,
                                ground_control_point.y,
                                ground_control_point.z);
                            gcp_name.push_back(ground_control_point.name());
                        }

                    }

                    TL_ASSERT(src.size() > 3, "Insufficient number of points");

                    Eigen::Vector3d offset(0., 0., 0.);
                    for (size_t i = 0; i < dst.size(); i++) {
                        offset += (dst[i] - offset) / (i + 1);
                    }

                    for (auto &i : dst) {
                        i -= offset;
                    }

                    tl::Path offset_path(mPath);
                    offset_path.append("offset.txt");

                    offsetWrite(offset_path, tl::Point3<double>(offset.x(), offset.y(), offset.z()));

                    colmap::SimilarityTransform3 similarity_transform;
                    similarity_transform.Estimate(src, dst);

                }


                // Se hace un ajuste mas fino con los puntos de control

                /// Por ahora paso la conversión de 
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

                /// Lectura de los puntos de control
                tl::Path gcp_file = mOutputPath;
                gcp_file.append("georef.xml");
                auto gcp_reader = GCPsReaderFactory::create("GRAPHOS");
                gcp_reader->read(gcp_file);
                auto gcps = gcp_reader->gcps();
                /// Se modifica la reconstrucción añadiendo los puntos de control
                /// Se guardan los ids para eliminarlos al final
                //for (auto &gcp : gcps) {
                //    //point3D_t AddPoint3D(
                //    //    const Eigen::Vector3d & xyz, const Track & track,
                //    //    const Eigen::Vector3ub & color = Eigen::Vector3ub::Zero());
                //    colmap::Track track;
                //    for (auto &pair : gcp.track().points()) {

                //        auto image_id = pair.first;
                //        auto point = pair.second;

                //        auto colmap_image_id = image_ids_graphos_to_colmap[image_id];
                //        /// No hay forma de añadir un unico punto 2D...
                //        //reconstruction.Image(colmap_image_id).SetPoints2D()
                //        //track.AddElement(colmap_image_id, );
                //        //inline void AddElement(const image_t image_id, const point2D_t point2D_idx);
                //    }
                //    Eigen::Vector3ub color = Eigen::Vector3ub::Zero();
                //    auto id = reconstruction.AddPoint3D(Eigen::Vector3d(gcp.x, gcp.y, gcp.z), track, color);

                //    
                //}


                // Configure bundle adjustment.
                BundleAdjustmentConfig ba_config;
                for (const colmap::image_t image_id : reg_image_ids) {
                    ba_config.AddImage(image_id);
                }
                ba_config.SetConstantPose(reg_image_ids[0]);
                ba_config.SetConstantTvec(reg_image_ids[1], {0});
                ba_config.setGroundControlPoints(gcps);

                ceres::Solver::Summary summary;

                {
                    ba_terminate = false;

                    BundleAdjuster bundle_adjuster(ba_options, ba_config);
                    bundle_adjuster.solve(&reconstruction);

                    if (status() == Status::stopping) return;

                    ba_terminate = true;
                    summary = bundle_adjuster.summary();
                }



                if (summary.termination_type == ceres::NO_CONVERGENCE) {

                    ba_terminate = false;

                    // Se vuelve a intentar el ajuste de haces
                    /// Reemplazar por graphos::BundleAdjuster
                    BundleAdjuster bundle_adjuster(ba_options, ba_config);
                    bundle_adjuster.solve(&reconstruction);

                    if (status() == Status::stopping) return;

                    ba_terminate = true;
                    summary = bundle_adjuster.summary();
                }

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