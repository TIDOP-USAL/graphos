/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "ImportOrientationProcess.h"

#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"
#include "graphos/core/camera/colmap.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/core/path.h>
#include <tidop/math/algebra/rotation_matrix.h>
#include <tidop/math/algebra/rotation_convert.h>

///TODO: Mover a 'core'
#include <colmap/util/option_manager.h>
#include <colmap/base/reconstruction.h>
#include <colmap/base/database.h>
#include <colmap/base/database_cache.h>
#include <colmap/controllers/incremental_mapper.h>

namespace graphos
{

ImportOrientationProcess::ImportOrientationProcess(const std::vector<Image> &images,
                                                   const std::map<int, tl::Camera> &cameras,
                                                   const QString &projectPath,
                                                   const QString &database,
                                                   bool fixCalibration,
                                                   bool fixPoses)
  : ProcessConcurrent(),
    mImages(images),
    mCameras(cameras),
    mProjectPath(projectPath), 
    mDatabase(database),
    mFixCalibration(fixCalibration),
    mFixPoses(fixPoses)
{

}

ImportOrientationProcess::~ImportOrientationProcess()
{
}

void ImportOrientationProcess::run()
{
  try {

    msgInfo("Starting Import Orientation");

    tl::Chrono chrono("Import Orientation finished");
    chrono.run();


    tl::TemporalDir temp_dir;
    tl::Path input_path = temp_dir.path();
    
    /// Reconstrucción temporal con orientaciones importadas
    
    bool bLocalCoord = true;
    tl::Point3D offset(0., 0., 0.);

    /// 1 - Escritura images.txt
    {

      tl::Path images_path(input_path);
      images_path.append("images.txt");

      std::ofstream ofs;
      ofs.open(images_path.toString(), std::ofstream::out | std::ofstream::trunc);

      if (!ofs.is_open()) throw std::runtime_error(std::string("Open fail: images.txt"));


      auto it = mImages.begin();
      if (it != mImages.end()) {
        CameraPose camera_pose = it->cameraPose();
        if (camera_pose.crs() != "") bLocalCoord = false;
      }

      int i = 1;
      for (const auto &image : mImages){
        CameraPose camera_pose = image.cameraPose();
        if (camera_pose.isEmpty()) continue;
        offset += (camera_pose.position() - offset) / i;
        i++;
      }

      size_t id = 1;

      //if (enu) {

      //  std::shared_ptr<Crs> epsgGeographic(new Crs("EPSG:4258"));
      //  std::shared_ptr<Crs> epsgGeocentric(new Crs("EPSG:4936"));
      //  std::shared_ptr<Crs> epsgUTM(new Crs("EPSG:25830"));
      //  CrsTransform<Point3D> crsTransformUtmToGeographic(epsgUTM, epsgGeographic);
      //  CrsTransform<Point3D> crsTransformUtmToGeocentric(epsgUTM, epsgGeocentric);

      //  /// Paso a coordenadas geograficas

      //  Point3D center_geographic = crsTransformUtmToGeographic.transform(offset);

      //  /// Paso a coordenadas geocentricas

      //  Point3D center_geocentric = crsTransformUtmToGeocentric.transform(offset);

      //  math::RotationMatrix<double> rotation_enu = rotationMatrixEnu(center_geographic.y, center_geographic.x);


      //  for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {

      //    CameraPosition cameraPosition = image->cameraPosition();
      //    if (cameraPosition.isEmpty()) {
      //      id++;
      //      continue; /// Se saltan las imagenes no orientadas
      //    }

      //    math::Quaternion<double> quaternion = cameraPosition.quaternion();
      //    /// Colmap da problemas con el formato texto si la ruta del fichero tiene espacios...
      //    std::string file_name = image->path().toStdString();//Path(image->path().toStdString()).fileName();
      //    tl::Point3D position(cameraPosition.x(), cameraPosition.y(), cameraPosition.z());

      //    /// Paso a coordenadas geocentricas

      //    Point3D point_geocentric = crsTransformUtmToGeocentric.transform(position);

      //    /// Paso coodenadas geocentricas (ECEF) a coordenadas ENU

      //    math::Vector<double, 3> vector_camera_position = ecefToEnu(center_geocentric, point_geocentric, rotation_enu);

      //    math::RotationMatrix<double> rotation_matrix;
      //    math::RotationConverter<double>::convert(quaternion, rotation_matrix);

      //    math::RotationMatrix<double> r_ip_ic = math::RotationMatrix<double>::identity();
      //    //r_ip_ic.at(0, 0) = -1;
      //    r_ip_ic.at(1, 1) = -1;
      //    r_ip_ic.at(2, 2) = -1;

      //    math::RotationMatrix<double> rotation = r_ip_ic * rotation_matrix.transpose();
      //    vector_camera_position = rotation * -vector_camera_position;
      //    tl::math::RotationConverter<double>::convert(rotation, quaternion);
      //    quaternion.normalize();

      //    ofs << std::fixed << id++ << " " << QString::number(quaternion.w, 'g', 10).toStdString() << " "
      //      << QString::number(quaternion.x, 'g', 10).toStdString() << " "
      //      << QString::number(quaternion.y, 'g', 10).toStdString() << " "
      //      << QString::number(quaternion.z, 'g', 10).toStdString() << " "
      //      << QString::number(vector_camera_position[0], 'g', 10).toStdString() << " "
      //      << QString::number(vector_camera_position[1], 'g', 10).toStdString() << " "
      //      << QString::number(vector_camera_position[2], 'g', 10).toStdString() << " 1 " << file_name << std::endl;
      //    ofs << std::endl;

      //  }

      //} else {

      for (const auto &image : mImages) {

          CameraPose camera_pose = image.cameraPose();
          if (camera_pose.isEmpty()) {
            id++;
            continue; /// Se saltan las imagenes no orientadas
          }

          tl::math::Quaternion<double> quaternion = camera_pose.quaternion();
          std::string file_name = image.path().toStdString();
          tl::Point3D position = camera_pose.position();

          if (!bLocalCoord) {
            position -= offset;
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
      //}

      ofs.close();
    }

    /// 2 - Escritura de fichero cameras.txt

    {
      tl::Path cameras_path(input_path);
      cameras_path.append("cameras.txt");

      std::ofstream ofs;
      ofs.open(cameras_path.toString(), std::ofstream::out | std::ofstream::trunc);

      if (!ofs.is_open()) throw std::runtime_error(std::string("Open fail: cameras.txt"));

      ofs << "# Camera list with one line of data per camera: \n";
      ofs << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n";
      ofs << "# Number of cameras: " << mCameras.size() << "\n";

      for (auto it = mCameras.begin(); it != mCameras.end(); it++) {
        tl::Camera camera = it->second;
        std::string camera_type = cameraToColmapType(camera).toStdString();
        std::shared_ptr<tl::Calibration> calibration = camera.calibration();

        if (calibration) {

          ofs << it->first << " " << camera_type << " " << camera.width() << " " << camera.height() << " ";

          if (camera_type == "SIMPLE_RADIAL" || 
              camera_type == "RADIAL" || 
              camera_type == "FULL_RADIAL" || 
              camera_type == "SIMPLE_RADIAL_FISHEYE" ||
              camera_type == "RADIAL_FISHEYE") {
            double focal = calibration->existParameter(tl::Calibration::Parameters::focal) ?
              calibration->parameter(tl::Calibration::Parameters::focal) :
              std::min(camera.width(), camera.height());
            ofs << QString::number(focal, 'g', 10).toStdString() << " ";
          } else {
            double focal_x = calibration->existParameter(tl::Calibration::Parameters::focalx) ?
              calibration->parameter(tl::Calibration::Parameters::focalx) :
              std::min(camera.width(), camera.height());
            double focal_y = calibration->existParameter(tl::Calibration::Parameters::focaly) ?
              calibration->parameter(tl::Calibration::Parameters::focaly) :
              std::min(camera.width(), camera.height());
            ofs << QString::number(focal_x, 'g', 10).toStdString() << " " << QString::number(focal_y, 'g', 10).toStdString() << " ";
          }

          double cx = calibration->existParameter(tl::Calibration::Parameters::cx) ? 
                      calibration->parameter(tl::Calibration::Parameters::cx) : 
                      camera.width() / 2.;
          double cy = calibration->existParameter(tl::Calibration::Parameters::cy) ? 
                      calibration->parameter(tl::Calibration::Parameters::cy) :
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
            double k1 = calibration->existParameter(tl::Calibration::Parameters::k1) ?
              calibration->parameter(tl::Calibration::Parameters::k1) : 0.0;
            ofs << " " << QString::number(k1, 'g', 10).toStdString();
          }

          if (camera_type == "RADIAL" ||
              camera_type == "FULL_RADIAL" ||
              camera_type == "OPENCV" ||
              camera_type == "OPENCV_FISHEYE" ||
              camera_type == "FULL_OPENCV" ||
              camera_type == "RADIAL_FISHEYE" ||
              camera_type == "THIN_PRISM_FISHEYE") {
            double k2 = calibration->existParameter(tl::Calibration::Parameters::k2) ?
              calibration->parameter(tl::Calibration::Parameters::k2) : 0.0;
            ofs << " " << QString::number(k2, 'g', 10).toStdString();
          }

          if (camera_type == "OPENCV" ||
              camera_type == "FULL_OPENCV" ||
              camera_type == "THIN_PRISM_FISHEYE" ||
              camera_type == "FULL_RADIAL") {
            double p1 = calibration->existParameter(tl::Calibration::Parameters::p1) ?
              calibration->parameter(tl::Calibration::Parameters::p1) : 0.0;
            double p2 = calibration->existParameter(tl::Calibration::Parameters::p2) ?
              calibration->parameter(tl::Calibration::Parameters::p2) : 0.0;
            ofs << " " << QString::number(p1, 'g', 10).toStdString()
                << " " << QString::number(p2, 'g', 10).toStdString();
          }

          if (camera_type == "OPENCV_FISHEYE" ||
              camera_type == "FULL_OPENCV" ||
              camera_type == "THIN_PRISM_FISHEYE" ||
              camera_type == "FULL_RADIAL") {
            double k3 = calibration->existParameter(tl::Calibration::Parameters::k3) ?
              calibration->parameter(tl::Calibration::Parameters::k3) : 0.0;
            ofs << " " << QString::number(k3, 'g', 10).toStdString();
          }

          if (camera_type == "OPENCV_FISHEYE" ||
              camera_type == "FULL_OPENCV" ||
              camera_type == "THIN_PRISM_FISHEYE") {
            double k4 = calibration->existParameter(tl::Calibration::Parameters::k4) ?
              calibration->parameter(tl::Calibration::Parameters::k4) : 0.0;
            ofs << " " << QString::number(k4, 'g', 10).toStdString();
          }

          if (camera_type == "FULL_OPENCV") {
            double k5 = calibration->existParameter(tl::Calibration::Parameters::k5) ?
              calibration->parameter(tl::Calibration::Parameters::k5) : 0.0;
            double k6 = calibration->existParameter(tl::Calibration::Parameters::k6) ?
              calibration->parameter(tl::Calibration::Parameters::k6) : 0.0;
            ofs << " " << QString::number(k5, 'g', 10).toStdString() 
                << " " << QString::number(k6, 'g', 10).toStdString();
          }

        } else {

          double focal = std::min(camera.width(), camera.height());
          double cx = camera.width() / 2.;
          double cy = camera.height() / 2.;

          ofs << it->first << " " << camera_type << " " << camera.width() << " " << camera.height() << " ";

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

    /// 3 - Escritura de fichero points3D.txt (En blanco)
    {

      tl::Path points3d_path(input_path);
      points3d_path.append("points3D.txt");

      std::ofstream ofs;
      ofs.open(points3d_path.toString(), std::ofstream::out | std::ofstream::trunc);
      ofs.close();
    }
    
    /// Triangulación y ajuste de haces
    {
      tl::Path reconstruction_path(mProjectPath.toStdWString());

      if (bLocalCoord) {
        reconstruction_path.append("ori").append("relative");
      } else {
        reconstruction_path.append("ori").append("absolute");
      }
      reconstruction_path.createDirectories();

      bool clear_points = false;

      colmap::OptionManager options;
      options.AddDatabaseOptions();
      options.AddImageOptions();
      options.AddRequiredOption("input_path", &input_path.toString());
      options.AddRequiredOption("reconstruction_path", &reconstruction_path.toString());
      options.AddDefaultOption(
        "clear_points", &clear_points,
        "Whether to clear all existing points and observations");
      options.AddMapperOptions();

      if (!reconstruction_path.exists()) {
        throw std::runtime_error(std::string("ERROR: 'reconstruction_path' is not a directory"));
      }

      auto mapper_options = options.mapper;

      msgInfo("Loading model");

      colmap::Reconstruction reconstruction;
      reconstruction.Read(input_path.toString());

      msgInfo("Loading database");

      colmap::DatabaseCache database_cache;

      {
        tl::Chrono timer("Elapsed time:");
        timer.run();

        colmap::Database database(mDatabase.toStdString());

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

      TL_ASSERT(reconstruction.NumRegImages() >= 2, "Need at least two images for triangulation")

      colmap::IncrementalMapper mapper(&database_cache);
      mapper.BeginReconstruction(&reconstruction);

      //////////////////////////////////////////////////////////////////////////////
      // Triangulation
      //////////////////////////////////////////////////////////////////////////////

      auto triangulation_options = mapper_options->Triangulation();

      const auto &reg_image_ids = reconstruction.RegImageIds();

      for (size_t i = 0; i < reg_image_ids.size(); ++i) {
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

      msgInfo("Extracting colors");
      reconstruction.ExtractColorsForAllImages("");

      const bool kDiscardReconstruction = false;
      mapper.EndReconstruction(kDiscardReconstruction);

      reconstruction.Write(reconstruction_path.toString());
      reconstruction.WriteText(reconstruction_path.toString());
      reconstruction.ExportPLY(tl::Path(reconstruction_path).append("sparse.ply").toString());

      std::ofstream stream(tl::Path(reconstruction_path).append("offset.txt").toString(), std::ios::trunc);
      if (stream.is_open()) {
        stream << QString::number(offset.x, 'f', 6).toStdString() << " "
               << QString::number(offset.y, 'f', 6).toStdString() << " "
               << QString::number(offset.z, 'f', 6).toStdString() << std::endl;

        msgInfo("Camera offset: %lf,%lf,%lf", offset.x, offset.y, offset.z);

        stream.close();
      }

    }

    emit importOrientationFinished();

    chrono.stop();

  } catch (std::exception &e) {
    emit error(0, "Import Orientation error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Import Orientation error");
    msgError("Unknow Exception");
  }
}

} // namespace graphos
