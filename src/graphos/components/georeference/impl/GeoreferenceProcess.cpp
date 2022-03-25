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

#include "GeoreferenceProcess.h"

#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

#include <colmap/estimators/triangulation.h>
#include <colmap/sfm/incremental_triangulator.h>
#include <colmap/base/similarity_transform.h>
#include <colmap/util/misc.h>

#include <QFileInfo>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace graphos
{

GeoreferenceProcess::GeoreferenceProcess(const QString &inputPath,
                                         const QString &outputPath,
                                         const std::vector<GroundControlPoint> &groundControlPoints)
  : tl::ProcessBase(),
    mInputPath(inputPath),
    mOutputPath(outputPath),
    mGroundControlPoints(groundControlPoints)
{

}

GeoreferenceProcess::~GeoreferenceProcess()
{
}

void GeoreferenceProcess::execute(tl::Progress *progressBar)
{
  try {

    msgInfo("Starting Georeference");

    tl::Chrono chrono;
    chrono.run();

    //std::map<std::string, Eigen::Vector3d> triangulate_points;
    std::vector<Eigen::Vector3d> src;
    std::vector<Eigen::Vector3d> dst;
    std::vector<std::string> gcp_name;

    colmap::IncrementalTriangulator::Options options;
    colmap::Reconstruction reconstruction;
    reconstruction.ReadBinary(mInputPath.toStdString());

    // Setup estimation options.
    colmap::EstimateTriangulationOptions tri_options;
    tri_options.min_tri_angle = colmap::DegToRad(options.min_angle);
    tri_options.residual_type =
      colmap::TriangulationEstimator::ResidualType::REPROJECTION_ERROR;
    tri_options.ransac_options.max_error = options.complete_max_reproj_error;
    tri_options.ransac_options.confidence = 0.9999;
    tri_options.ransac_options.min_inlier_ratio = 0.02;
    tri_options.ransac_options.max_num_trials = 10000;

    for (size_t i = 0; i < mGroundControlPoints.size(); i++) {

      GroundControlPoint ground_control_point = mGroundControlPoints[i];

      std::vector<colmap::TriangulationEstimator::PointData> points_data;
      std::vector<colmap::TriangulationEstimator::PoseData> poses_data;

      for (auto &camera : reconstruction.Cameras()) {

        for (auto &image : reconstruction.Images()) {

          if (image.second.CameraId() == camera.second.CameraId()) {

            QString base_name = QFileInfo(image.second.Name().c_str()).baseName();
            if (ground_control_point.existImagePoint(base_name.toStdString())) {
              tl::PointD point = ground_control_point.imagePoint(base_name.toStdString());

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
        dst.push_back(Eigen::Vector3d(ground_control_point.point().x, 
                                      ground_control_point.point().y, 
                                      ground_control_point.point().z));
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

      std::string path = mOutputPath.toStdString();
      fs::path dir(path);

      if (!fs::exists(dir)) {
        if (!fs::create_directories(dir)) {
          std::string err = "The output directory cannot be created: ";
          err.append(path);
          throw std::runtime_error(err);
        }
      }

      std::string offset_file = std::string(path).append("\\offset.txt");
      std::ofstream stream(offset_file, std::ios::trunc);
      if (stream.is_open()) {
        stream << QString::number(offset.x(), 'f', 3).toStdString() << " "
          << QString::number(offset.y(), 'f', 3).toStdString() << " "
          << QString::number(offset.z(), 'f', 3).toStdString() << std::endl;
      }

      colmap::SimilarityTransform3 similarity_transform;
      similarity_transform.Estimate(src, dst);
      
      reconstruction.Transform(similarity_transform);
      reconstruction.Write(path);

      OrientationExport orientationExport(&reconstruction);
      orientationExport.exportPLY(QString(path.c_str()).append("\\sparse.ply"));


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

    uint64_t time = chrono.stop();
    msgInfo("Georeference process finished [Time: %f seconds]", time / 1000.);

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
  }
}

} // namespace graphos
