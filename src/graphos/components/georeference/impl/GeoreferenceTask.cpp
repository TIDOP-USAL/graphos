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

#include "GeoreferenceTask.h"

#include "graphos/core/sfm/orientationexport.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Colmap.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/math/algebra/rotation_convert.h>

#include <colmap/estimators/triangulation.h>
#include <colmap/sfm/incremental_triangulator.h>
#include <colmap/base/similarity_transform.h>
#include <colmap/util/misc.h>
#include <colmap/base/database.h>

#include <QFileInfo>

#include <iomanip>

namespace graphos
{

void exportToColmap(const tl::Path &databasePath,
                    const std::unordered_map<size_t, Image> &images,
                    const std::map<int, Camera> &cameras,
                    const std::unordered_map<size_t, CameraPose> &poses,
                    const std::vector<GroundPoint> &groundPoints,
                    const std::vector<GroundControlPoint> &groundControlPoints,
                    colmap::Reconstruction &reconstruction)
{
  try {

    colmap::Database database;
    database.Open(databasePath.toString());

    //colmap::DatabaseCache database_cache;
    //database_cache.Load(database, 15, false, std::unordered_set<std::string>{});

    //colmap::Reconstruction reconstruction;
    //reconstruction.Load(database_cache);

    //colmap::Camera colmap_camera;
    //reconstruction.AddCamera(colmap_camera);
    const auto &colmap_images = database.ReadAllImages();

    std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;

    for (const auto &image : images) {

      tl::Path image_path(image.second.path().toStdString());

      for (const auto &colmap_image : colmap_images) {
        tl::Path colmap_image_path(colmap_image.Name());

        if (image_path.equivalent(colmap_image_path)) {
          graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
          break;
        }
      }

    }

    //std::map<int, Undistort> undistort_map;

    //for (auto &camera : cameras()) {
    //  undistort_map[camera.first] = Undistort(camera.second);
    //}


    tl::TemporalDir temp_dir;
    tl::Path colmap_sparse_path = temp_dir.path();
    //colmap_sparse_path.append("temp");
    colmap_sparse_path.append("sparse");
    colmap_sparse_path.createDirectories();

    TL_TODO("Extraer la exportación a colmap")
      // cameras.txt
    {
      tl::Path colmap_cameras(colmap_sparse_path);
      colmap_cameras.append("cameras.txt");

      std::ofstream ofs;
      ofs.open(colmap_cameras.toString(), std::ofstream::out | std::ofstream::trunc);

      TL_ASSERT(ofs.is_open(), "Open fail: cameras.txt");

      ofs << "# Camera list with one line of data per camera: \n";
      ofs << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n";
      ofs << "# Number of cameras: " << cameras.size() << "\n";

      ofs << std::fixed << std::setprecision(12);

      for (auto &camera_pair : cameras) {

        size_t camera_id = camera_pair.first;
        Camera camera = camera_pair.second;
        std::string camera_type = cameraToColmapType(camera).toStdString();
        auto calibration = camera.calibration();

        TL_ASSERT(calibration, "Camera calibration not found");

        ofs << camera_id << " " << camera_type << " " << camera.width() << " " << camera.height() << " ";

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
                    camera.width() / 2.;
        double cy = calibration->existParameter(Calibration::Parameters::cy) ?
                    calibration->parameter(Calibration::Parameters::cy) :
                    camera.height() / 2.;

        ofs << cx << " " << cy;

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
                      calibration->parameter(Calibration::Parameters::k1) : 
                      0.0;

          ofs << " " << k1;

        }

        if (camera_type == "RADIAL" ||
            camera_type == "FULL_RADIAL" ||
            camera_type == "OPENCV" ||
            camera_type == "OPENCV_FISHEYE" ||
            camera_type == "FULL_OPENCV" ||
            camera_type == "RADIAL_FISHEYE" ||
            camera_type == "THIN_PRISM_FISHEYE") {

          double k2 = calibration->existParameter(Calibration::Parameters::k2) ?
                      calibration->parameter(Calibration::Parameters::k2) : 
                      0.0;
                      
          ofs << " " << k2;

        }

        if (camera_type == "OPENCV" ||
            camera_type == "FULL_OPENCV" ||
            camera_type == "THIN_PRISM_FISHEYE" ||
            camera_type == "FULL_RADIAL") {

          double p1 = calibration->existParameter(Calibration::Parameters::p1) ?
                      calibration->parameter(Calibration::Parameters::p1) : 0.0;
          double p2 = calibration->existParameter(Calibration::Parameters::p2) ?
                      calibration->parameter(Calibration::Parameters::p2) : 0.0;
          
          ofs << " " << p1 << " " << p2;

        }

        if (camera_type == "OPENCV_FISHEYE" ||
            camera_type == "FULL_OPENCV" ||
            camera_type == "THIN_PRISM_FISHEYE" ||
            camera_type == "FULL_RADIAL") {

          double k3 = calibration->existParameter(Calibration::Parameters::k3) ?
                      calibration->parameter(Calibration::Parameters::k3) : 
                      0.0;

          ofs << " " << k3;

        }

        if (camera_type == "OPENCV_FISHEYE" ||
            camera_type == "FULL_OPENCV" ||
            camera_type == "THIN_PRISM_FISHEYE") {

          double k4 = calibration->existParameter(Calibration::Parameters::k4) ?
                      calibration->parameter(Calibration::Parameters::k4) :
                      0.0;

          ofs << " " << k4;

        }

        if (camera_type == "FULL_OPENCV") {

          double k5 = calibration->existParameter(Calibration::Parameters::k5) ?
                      calibration->parameter(Calibration::Parameters::k5) : 0.0;
          double k6 = calibration->existParameter(Calibration::Parameters::k6) ?
                      calibration->parameter(Calibration::Parameters::k6) : 0.0;

          ofs << " " << k5 << " " << k6;
        }

        ofs << std::endl;

        ofs.close();

      }

    }

    /// Escritura en colmap::Reconstruction
    //{

    //  for (auto &camera_pair : cameras) {
    //    
    //    Camera graphos_camera = camera_pair.second;
    //    auto calibration = graphos_camera.calibration();

    //    colmap::Camera &colmap_camera = reconstruction.Camera(static_cast<colmap::image_t>(camera_pair.first));
    //    std::vector<double> &params = colmap_camera.Params();

    //    switch (calibration->cameraModel()) {
    //      case graphos::Calibration::CameraModel::radial1:
    //        params[0] = calibration->parameter(Calibration::Parameters::focal);
    //        params[1] = calibration->parameter(Calibration::Parameters::cx);
    //        params[2] = calibration->parameter(Calibration::Parameters::cy);
    //        params[3] = calibration->parameter(Calibration::Parameters::k1);
    //        break;    
    //      case graphos::Calibration::CameraModel::radial2:
    //        params[0] = calibration->parameter(Calibration::Parameters::focal);
    //        params[1] = calibration->parameter(Calibration::Parameters::cx);
    //        params[2] = calibration->parameter(Calibration::Parameters::cy);
    //        params[3] = calibration->parameter(Calibration::Parameters::k1);
    //        params[4] = calibration->parameter(Calibration::Parameters::k2);
    //        break;
    //      case graphos::Calibration::CameraModel::radial3:
    //        params[0] = calibration->parameter(Calibration::Parameters::focal);
    //        params[1] = calibration->parameter(Calibration::Parameters::cx);
    //        params[2] = calibration->parameter(Calibration::Parameters::cy);
    //        params[3] = calibration->parameter(Calibration::Parameters::k1);
    //        params[4] = calibration->parameter(Calibration::Parameters::k2);
    //        params[5] = calibration->parameter(Calibration::Parameters::k3);
    //        params[6] = calibration->parameter(Calibration::Parameters::p1);
    //        params[7] = calibration->parameter(Calibration::Parameters::p2);
    //        break;
    //      case graphos::Calibration::CameraModel::simple_radial_fisheye:
    //        params[0] = calibration->parameter(Calibration::Parameters::focal);
    //        params[1] = calibration->parameter(Calibration::Parameters::cx);
    //        params[2] = calibration->parameter(Calibration::Parameters::cy);
    //        params[3] = calibration->parameter(Calibration::Parameters::k1);
    //        break;
    //      case graphos::Calibration::CameraModel::radial_fisheye:
    //        params[0] = calibration->parameter(Calibration::Parameters::focal);
    //        params[1] = calibration->parameter(Calibration::Parameters::cx);
    //        params[2] = calibration->parameter(Calibration::Parameters::cy);
    //        params[3] = calibration->parameter(Calibration::Parameters::k1);
    //        params[4] = calibration->parameter(Calibration::Parameters::k2);
    //        break;
    //      case graphos::Calibration::CameraModel::opencv:
    //        params[0] = calibration->parameter(Calibration::Parameters::focalx);
    //        params[1] = calibration->parameter(Calibration::Parameters::focaly);
    //        params[2] = calibration->parameter(Calibration::Parameters::cx);
    //        params[3] = calibration->parameter(Calibration::Parameters::cy);
    //        params[4] = calibration->parameter(Calibration::Parameters::k1);
    //        params[5] = calibration->parameter(Calibration::Parameters::k2);
    //        params[6] = calibration->parameter(Calibration::Parameters::p1);
    //        params[7] = calibration->parameter(Calibration::Parameters::p2);
    //        break;
    //      case graphos::Calibration::CameraModel::opencv_fisheye:
    //        params[0] = calibration->parameter(Calibration::Parameters::focalx);
    //        params[1] = calibration->parameter(Calibration::Parameters::focaly);
    //        params[2] = calibration->parameter(Calibration::Parameters::cx);
    //        params[3] = calibration->parameter(Calibration::Parameters::cy);
    //        params[4] = calibration->parameter(Calibration::Parameters::k1);
    //        params[5] = calibration->parameter(Calibration::Parameters::k2);
    //        params[6] = calibration->parameter(Calibration::Parameters::k3);
    //        params[7] = calibration->parameter(Calibration::Parameters::k4);
    //        break;
    //      case graphos::Calibration::CameraModel::opencv_full:
    //        params[0] = calibration->parameter(Calibration::Parameters::focalx);
    //        params[1] = calibration->parameter(Calibration::Parameters::focaly);
    //        params[2] = calibration->parameter(Calibration::Parameters::cx);
    //        params[3] = calibration->parameter(Calibration::Parameters::cy);
    //        params[4] = calibration->parameter(Calibration::Parameters::k1);
    //        params[5] = calibration->parameter(Calibration::Parameters::k2);
    //        params[6] = calibration->parameter(Calibration::Parameters::p1);
    //        params[7] = calibration->parameter(Calibration::Parameters::p2);
    //        params[8] = calibration->parameter(Calibration::Parameters::k3);
    //        params[9] = calibration->parameter(Calibration::Parameters::k4);
    //        params[10] = calibration->parameter(Calibration::Parameters::k5);
    //        params[11] = calibration->parameter(Calibration::Parameters::k6);
    //        break;
    //      case graphos::Calibration::CameraModel::simple_pinhole:
    //        params[0] = calibration->parameter(Calibration::Parameters::focal);
    //        params[1] = calibration->parameter(Calibration::Parameters::cx);
    //        params[2] = calibration->parameter(Calibration::Parameters::cy);
    //        break;
    //      case graphos::Calibration::CameraModel::pinhole:
    //        params[0] = calibration->parameter(Calibration::Parameters::focalx);
    //        params[1] = calibration->parameter(Calibration::Parameters::focaly);
    //        params[2] = calibration->parameter(Calibration::Parameters::cx);
    //        params[3] = calibration->parameter(Calibration::Parameters::cy);
    //        break;
    //      default:
    //        break;
    //    }
    //    
    //  }
    //}

    //std::unordered_map<size_t, size_t> graphos_to_mvs_ids;

    // images.txt
    {
      tl::Path colmap_images(colmap_sparse_path);
      colmap_images.append("images.txt");

      std::ofstream ofs;
      ofs.open(colmap_images.toString(), std::ofstream::out | std::ofstream::trunc);

      TL_ASSERT(ofs.is_open(), "Open fail: images.txt");

      ofs << std::fixed << std::setprecision(12);



      for (const auto &pose : poses) {

        size_t image_id = pose.first;
        auto &image_pose = pose.second;

        //size_t mvs_id = graphos_to_mvs_ids.size();
        //graphos_to_mvs_ids[image_id] = mvs_id;

        const auto &image = images.at(image_id);

        auto projection_center = image_pose.position();
        auto rotation_matrix = image_pose.rotationMatrix();

        tl::math::RotationMatrix<double> transform_to_colmap = tl::math::RotationMatrix<double>::identity();
        transform_to_colmap.at(1, 1) = -1;
        transform_to_colmap.at(2, 2) = -1;

        tl::math::RotationMatrix<double> rotation = transform_to_colmap * rotation_matrix.transpose();

        tl::math::Quaternion<double> quaternion;
        tl::math::RotationConverter<double>::convert(rotation, quaternion);
        quaternion.normalize();

        auto xyx = rotation * -projection_center.vector();

        ofs << graphos_to_colmap_image_ids[image_id] << " " << quaternion.w << " " << quaternion.x << " " << quaternion.y << " " << quaternion.z << " "
          << xyx[0] << " " << xyx[1] << " " << xyx[2] << " " << image.cameraId() << " " << image.name().toStdString() << std::endl;

        /// #   POINTS2D[] as (X, Y, POINT3D_ID)

        for (size_t i = 0; i < groundPoints.size(); i++) {

          auto &track = groundPoints[i].track();

          if (track.existPair(image_id)) {

            for (auto &map : track.pairs()) {

              if (map.first == image_id) {
                size_t point_id = map.second;
                auto keypoints = database.ReadKeypoints(graphos_to_colmap_image_ids.at(image_id));
                
                ofs << keypoints[point_id].x << " " << keypoints[point_id].y << " " << i+1 << " ";

              }

            }
          }
        }

        ofs << std::endl;

      }

      ofs.close();
    }

    /// colmap::Reconstruction images
    //{

    //  for (const auto &pose : poses) {

    //    size_t image_id = pose.first;

    //    colmap::Image colmap_image = reconstruction.Image(graphos_to_colmap_image_ids[image_id]);
    //    colmap_image.Po
    //  }

    //}

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
      for (auto &points_3d : groundPoints) {

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

    reconstruction.ReadText(colmap_sparse_path.toString());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Densification error");
  }
}


GeoreferenceProcess::GeoreferenceProcess(const std::unordered_map<size_t, Image> &images,
                                         const std::map<int, Camera> &cameras,
                                         const std::unordered_map<size_t, CameraPose> &poses,
                                         const std::vector<GroundPoint> &groundPoints,
                                         const std::vector<GroundControlPoint> &groundControlPoints,
                                         const tl::Path &database)
  : tl::TaskBase(),
    mImages(images),
    mCameras(cameras),
    mPoses(poses),
    mGroundPoints(groundPoints),
    mGroundControlPoints(groundControlPoints),
    mDatabase(database)
{

}

GeoreferenceProcess::~GeoreferenceProcess()
{
}

void GeoreferenceProcess::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Georeference process finished");
    chrono.run();

    colmap::Reconstruction reconstruction;
    exportToColmap(mDatabase,
                   mImages,
                   mCameras,
                   mPoses,
                   mGroundPoints,
                   mGroundControlPoints,
                   reconstruction);

    std::vector<Eigen::Vector3d> src;
    std::vector<Eigen::Vector3d> dst;
    std::vector<std::string> gcp_name;

    colmap::IncrementalTriangulator::Options options;
    //colmap::Reconstruction reconstruction;
    ////reconstruction.ReadBinary(mInputPath.toStdString());

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

    for (size_t i = 0; i < mGroundControlPoints.size(); i++) {

      GroundControlPoint ground_control_point = mGroundControlPoints[i];

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
        //triangulate_points[ground_control_point.name()] = xyz;
        src.push_back(xyz);
        dst.push_back(Eigen::Vector3d(ground_control_point.x, 
                                      ground_control_point.y, 
                                      ground_control_point.z));
        gcp_name.push_back(ground_control_point.name());
      }

    }

    if (src.size() > 3) {

      Eigen::Vector3d offset(0., 0., 0.);
      for (size_t i = 0; i < dst.size(); i++) {
        offset += (dst[i] - offset) / (i + 1);
      }

      for (size_t i = 0; i < dst.size(); i++) {
        dst[i] -= offset;
      }

      //std::string path = mOutputPath.toStdString();
      tl::Path offset_path(mDatabase);
      offset_path.replaceFileName("offset.txt");
      //if (!fs::exists(dir)) {
      //  if (!fs::create_directories(dir)) {
      //    std::string err = "The output directory cannot be created: ";
      //    err.append(path);
      //    throw std::runtime_error(err);
      //  }
      //}

      //std::string offset_file = std::string(path).append("\\offset.txt");
      std::ofstream stream(offset_path.toString(), std::ios::trunc);
      if (stream.is_open()) {
        stream << QString::number(offset.x(), 'f', 3).toStdString() << " "
          << QString::number(offset.y(), 'f', 3).toStdString() << " "
          << QString::number(offset.z(), 'f', 3).toStdString() << std::endl;
      }

      colmap::SimilarityTransform3 similarity_transform;
      similarity_transform.Estimate(src, dst);
      
      reconstruction.Transform(similarity_transform);
      //reconstruction.Write(path);

      OrientationExport orientationExport(&reconstruction);
      //orientationExport.exportPLY(QString(path.c_str()).append("\\sparse.ply"));


      std::vector<double> errors;
      errors.reserve(dst.size());

      for (size_t i = 0; i < dst.size(); ++i) {
        similarity_transform.TransformPoint(&src.at(i));
        errors.push_back((src[i] - dst[i]).norm());
        
        msgInfo("Ground Control Point %s: Error -> %f", gcp_name[i].c_str(), errors[i]);
      }

      msgInfo("Georeference error: %f (mean), %f (median)",
              colmap::Mean(errors), colmap::Median(errors));

    }

    emit georeferenceFinished();

    chrono.stop();

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Georeference error");
  }
}

} // namespace graphos
