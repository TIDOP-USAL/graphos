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

#include "graphos/core/dense/mvs.h"

#include "graphos/core/camera/Calibration.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/groundpoint.h"

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/core/app.h>
#include <tidop/img/imgreader.h>
#include <tidop/core/progress.h>
#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/core/chrono.h>

// COLMAP
#include <colmap/base/database.h>

#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace graphos
{

constexpr auto mvsDefaultResolutionLevel = 1;
constexpr auto mvsDefaultMinResolution = 640;
constexpr auto mvsDefaultMaxResolution = 3200;
constexpr auto mvsDefaultNumberViews = 5;
constexpr auto mvsDefaultNumberViewsFuse = 3;

MvsProperties::MvsProperties()
  : mResolutionLevel(mvsDefaultResolutionLevel),
    mMinResolution(mvsDefaultMinResolution),
    mMaxResolution(mvsDefaultMaxResolution),
    mNumberViews(mvsDefaultNumberViews),
    mNumberViewsFuse(mvsDefaultNumberViewsFuse)
{
}

MvsProperties::MvsProperties(int resolutionLevel,
                             int minResolution,
                             int maxResolution,
                             int numberViews,
                             int numberViewsFuse)
  : mResolutionLevel(resolutionLevel),
    mMinResolution(minResolution),
    mMaxResolution(maxResolution),
    mNumberViews(numberViews),
    mNumberViewsFuse(numberViewsFuse)
{
}

MvsProperties::MvsProperties(const MvsProperties &mvs)
  : Mvs(mvs),
    mResolutionLevel(mvs.mResolutionLevel),
    mMinResolution(mvs.mMinResolution),
    mMaxResolution(mvs.mMaxResolution),
    mNumberViews(mvs.mNumberViews),
    mNumberViewsFuse(mvs.mNumberViewsFuse)
{
}

MvsProperties::MvsProperties(MvsProperties &&mvs) noexcept
  : Mvs(std::forward<Mvs>(mvs)),
    mResolutionLevel(mvs.mResolutionLevel),
    mMinResolution(mvs.mMinResolution),
    mMaxResolution(mvs.mMaxResolution),
    mNumberViews(mvs.mNumberViews),
    mNumberViewsFuse(mvs.mNumberViewsFuse)
{
}

MvsProperties &MvsProperties::operator =(const MvsProperties &mvs)
{
  if (this != &mvs) {
    mResolutionLevel = mvs.mResolutionLevel;
    mMinResolution = mvs.mMinResolution;
    mMaxResolution = mvs.mMaxResolution;
    mNumberViews = mvs.mNumberViews;
    mNumberViewsFuse = mvs.mNumberViewsFuse;
  }

  return *this;
}

MvsProperties &MvsProperties::operator =(MvsProperties &&mvs) noexcept
{
  if (this != &mvs) {
    mResolutionLevel = mvs.mResolutionLevel;
    mMinResolution = mvs.mMinResolution;
    mMaxResolution = mvs.mMaxResolution;
    mNumberViews = mvs.mNumberViews;
    mNumberViewsFuse = mvs.mNumberViewsFuse;
  }

  return *this;
}

int MvsProperties::resolutionLevel() const
{
  return mResolutionLevel;
}

int MvsProperties::minResolution() const
{
  return mMinResolution;
}

int MvsProperties::maxResolution() const
{
  return mMaxResolution;
}

int MvsProperties::numberViews() const
{
  return mNumberViews;
}

int MvsProperties::numberViewsFuse() const
{
  return mNumberViewsFuse;
}

void MvsProperties::setResolutionLevel(int resolutionLevel)
{
  mResolutionLevel = resolutionLevel;
}

void MvsProperties::setMinResolution(int minResolution)
{
  mMinResolution = minResolution;
}

void MvsProperties::setMaxResolution(int maxResolution)
{
  mMaxResolution = maxResolution;
}

void MvsProperties::setNumberViews(int numberViews)
{
  mNumberViews = numberViews;
}

void MvsProperties::setNumberViewsFuse(int numberViewsFuse)
{
  mNumberViewsFuse = numberViewsFuse;
}

void MvsProperties::reset()
{
  mResolutionLevel = mvsDefaultResolutionLevel;
  mMinResolution = mvsDefaultMinResolution;
  mMaxResolution = mvsDefaultMaxResolution;
  mNumberViews = mvsDefaultNumberViews;
  mNumberViewsFuse = mvsDefaultNumberViewsFuse;
}


QString MvsProperties::name() const
{
  return QString("MVS");
}




/*----------------------------------------------------------------*/



MvsDensifier::MvsDensifier(const std::unordered_map<size_t, Image> &images,
                           const std::map<int, Camera> &cameras,
                           const std::unordered_map<size_t, CameraPose> &poses,
                           const std::vector<GroundPoint> &groundPoints, 
                           const QString &outputPath,
                           const QString &database, 
                           bool cuda)
  : DensifierBase(images, cameras, poses, groundPoints, outputPath),
    mDatabase(database)
{
  enableCuda(cuda);
  setUndistortImagesFormat(UndistortImages::Format::tiff);
}

MvsDensifier::~MvsDensifier()
{
}

void MvsDensifier::clearPreviousModel()
{
  outputPath().removeDirectory();
}

void MvsDensifier::exportToComap()
{
  //try {

  //  colmap::Database database;
  //  database.Open(mDatabase.toStdString());
  //  const auto &colmap_images = database.ReadAllImages();

  //  std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;

  //  for (const auto &image : images()) {

  //    tl::Path image_path(image.second.path().toStdString());

  //    for (const auto &colmap_image : colmap_images) {
  //      tl::Path colmap_image_path(colmap_image.Name());

  //      if (image_path.equivalent(colmap_image_path)) {
  //        graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
  //        break;
  //      }
  //    }

  //  }
  //  std::map<int, Undistort> undistort_map;

  //  for (auto &camera : cameras()) {
  //    undistort_map[camera.first] = Undistort(camera.second);
  //  }


  //  tl::TemporalDir temp_dir;
  //  tl::Path colmap_sparse_path = temp_dir.path();
  //  colmap_sparse_path.append("temp");
  //  colmap_sparse_path.append("sparse");
  //  colmap_sparse_path.createDirectories();

  //  TL_TODO("Extraer la exportación a colmap")
  //  // cameras.txt
  //  {
  //    tl::Path colmap_cameras(colmap_sparse_path);
  //    colmap_cameras.append("cameras.txt");

  //    std::ofstream ofs;
  //    ofs.open(colmap_cameras.toString(), std::ofstream::out | std::ofstream::trunc);

  //    TL_ASSERT(ofs.is_open(), "Open fail: cameras.txt");
  //    
  //    ofs << "# Camera list with one line of data per camera: \n";
  //    ofs << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n";
  //    ofs << "# Number of cameras: " << cameras().size() << "\n";

  //    ofs << std::fixed << std::setprecision(12);

  //    for (auto &undistort_pair : undistort_map) {
  //      
  //      size_t camera_id = undistort_pair.first;
  //      Undistort undistort = undistort_pair.second;
  //      Camera undistort_camera = undistort.undistortCamera();
  //      auto calibration = undistort_camera.calibration();

  //      TL_ASSERT(calibration, "Camera calibration not found");

  //      /// La cámara tiene que ser PINHOLE para InterfaceCOLMAP
  //      ofs << camera_id << " PINHOLE " << undistort_camera.width() << " " << undistort_camera.height() << " ";

  //      double focal_x = 0.0;
  //      double focal_y = 0.0;

  //      if (calibration->existParameter(Calibration::Parameters::focal)) {
  //        focal_x = focal_y = calibration->parameter(Calibration::Parameters::focal);
  //      } else {
  //        focal_x = calibration->parameter(Calibration::Parameters::focalx);
  //        focal_y = calibration->parameter(Calibration::Parameters::focaly);
  //      }

  //      ofs << focal_x << " " << focal_y << " ";

  //      double cx = calibration->existParameter(Calibration::Parameters::cx) ?
  //                  calibration->parameter(Calibration::Parameters::cx) :
  //                   undistort_camera.width() / 2.;
  //      double cy = calibration->existParameter(Calibration::Parameters::cy) ?
  //                  calibration->parameter(Calibration::Parameters::cy) :
  //                  undistort_camera.height() / 2.;

  //      ofs << cx << " " << cy << std::endl;

  //    }

  //  }

  //  // images.txt
  //  {
  //    tl::Path colmap_images(colmap_sparse_path);
  //    colmap_images.append("images.txt");

  //    std::ofstream ofs;
  //    ofs.open(colmap_images.toString(), std::ofstream::out | std::ofstream::trunc);

  //    TL_ASSERT(ofs.is_open(), "Open fail: images.txt");

  //    ofs << std::fixed << std::setprecision(12);

  //    std::unordered_map<size_t, size_t> graphos_to_mvs_ids;

  //    for (const auto &pose : poses()) {
  //    
  //      size_t image_id = pose.first;
  //      auto &image_pose = pose.second;

  //      size_t mvs_id = graphos_to_mvs_ids.size();
  //      graphos_to_mvs_ids[image_id] = mvs_id;

  //      const auto &image = images().at(image_id);

  //      auto projection_center = image_pose.position();
  //      auto rotation_matrix = image_pose.rotationMatrix();

  //      tl::math::RotationMatrix<double> transform_to_colmap = tl::math::RotationMatrix<double>::identity();
  //      transform_to_colmap.at(1, 1) = -1;
  //      transform_to_colmap.at(2, 2) = -1;

  //      tl::math::RotationMatrix<double> rotation = transform_to_colmap * rotation_matrix.transpose();

  //      tl::math::Quaternion<double> quaternion;
  //      tl::math::RotationConverter<double>::convert(rotation, quaternion);
  //      quaternion.normalize();

  //      auto xyx = rotation * -projection_center.vector();

  //      ofs << mvs_id << " " << quaternion.w << " " << quaternion.x << " " << quaternion.y << " " << quaternion.z << " "
  //          << xyx[0] << " " << xyx[1] << " " << xyx[2] << " " << image.cameraId() << " " << image.name().toStdString() << std::endl;
  //      ofs << std::endl;
  //    }

  //  }

  //  // points3D.txt
  //  {
  //    tl::Path colmap_points_3d(colmap_sparse_path);
  //    colmap_points_3d.append("points3D.txt");
  //  }

  //} catch (...) {
  //  TL_THROW_EXCEPTION_WITH_NESTED("Densification error");
  //}
}

void MvsDensifier::writeNVMFile()
{
  try {
  
    colmap::Database database;
    database.Open(mDatabase.toStdString());
    const auto &colmap_images = database.ReadAllImages();

    std::unordered_map<size_t, colmap::image_t> graphos_to_colmap_image_ids;

    for (const auto &image : images()) {

      tl::Path image_path(image.second.path().toStdString());

      for (const auto &colmap_image : colmap_images) {
        tl::Path colmap_image_path(colmap_image.Name());

        if (image_path.equivalent(colmap_image_path)) {
          graphos_to_colmap_image_ids[image.first] = colmap_image.ImageId();
          break;
        }
      }

    }


    tl::Path nvm_path(outputPath());
    nvm_path.append("model.nvm");

    std::map<int, Undistort> undistort;

    for (auto &camera : cameras()) {
      undistort[camera.first] = Undistort(camera.second);
    }

    std::ofstream stream(nvm_path.toString(), std::ios::trunc);

    if (stream.is_open()) {

      int camera_count = poses().size();
      int ground_points_count = groundPoints().size();

      stream << "NVM_V3 \n\n" << camera_count << "\n";
      //stream << "NVM_V3_R9T \n\n" << camera_count << "\n";

      stream << std::fixed << std::setprecision(12);

      std::unordered_map<size_t, size_t> graphos_to_mvs_ids;

      for (const auto &pose : poses()) {

        size_t image_id = pose.first;
        size_t mvs_id = graphos_to_mvs_ids.size();
        graphos_to_mvs_ids[image_id] = mvs_id;

        const auto &image = images().at(image_id);
        size_t camera_id = image.cameraId();
        float new_focal = undistort.at(camera_id).undistortCamera().focal();

        auto projection_center = pose.second.position();
        auto quaternion = pose.second.quaternion();
        //auto rotation_matrix = pose.second.rotationMatrix();
        //auto transform_to_colmap = tl::math::RotationMatrix<double>::identity();
        //transform_to_colmap.at(1, 1) = -1;
        //transform_to_colmap.at(2, 2) = -1;

        //tl::math::RotationMatrix<double> rotation = transform_to_colmap * pose.second.rotationMatrix();

        //tl::math::Quaternion<double> quaternion;
        //tl::math::RotationConverter<double>::convert(rotation, quaternion);
        //quaternion.normalize();

        tl::Path undistort_image(image.path().toStdString());
        undistort_image.replaceExtension(".tif");

        stream << "undistort/" << undistort_image.fileName().toString() << " ";
        stream << new_focal << " ";
        stream << quaternion.w << " ";
        stream << quaternion.x << " ";
        stream << quaternion.y << " ";
        stream << quaternion.z << " ";
        //stream << rotation_matrix[0][0] << " ";
        //stream << rotation_matrix[0][1] << " ";
        //stream << rotation_matrix[0][2] << " ";
        //stream << rotation_matrix[1][0] << " ";
        //stream << rotation_matrix[1][1] << " ";
        //stream << rotation_matrix[1][2] << " ";
        //stream << rotation_matrix[2][0] << " ";
        //stream << rotation_matrix[2][1] << " ";
        //stream << rotation_matrix[2][2] << " ";
        stream << projection_center.x << " ";
        stream << projection_center.y << " ";
        stream << projection_center.z << " ";
        stream << 0 << " ";
        stream << 0 << std::endl;

      }

      stream << "\n" << ground_points_count << std::endl;

      for (auto &points_3d : groundPoints()) {

        stream << points_3d.x << " "
               << points_3d.y << " "
               << points_3d.z << " "
               << points_3d.color().red() << " "
               << points_3d.color().green() << " "
               << points_3d.color().blue() << " ";

        auto &track = points_3d.track();

        stream << track.size();
        
        for (auto &map : track.pairs()) {

          size_t image_id = map.first;
          size_t point_id = map.second;
          
          auto keypoints = database.ReadKeypoints(graphos_to_colmap_image_ids.at(image_id));
          auto _undistort = undistort.at(images().at(image_id).cameraId());

          tl::Point<float> undistort_point = _undistort.undistortPoint(tl::Point<float>(keypoints[point_id].x, keypoints[point_id].y));

          stream << " " << static_cast<int>(graphos_to_mvs_ids.at(image_id))
                 << " " << point_id
                 << " " << undistort_point.x
                 << " " << undistort_point.y;
        }

        stream << std::endl;
      }

      stream.close();
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Densification error");
  }

}

void MvsDensifier::exportToMVS()
{
  try {

    tl::Path app_path = tl::App::instance().path();
    std::string cmd_mvs("\"");
    cmd_mvs.append(app_path.parentPath().toString());
    //cmd_mvs.append("E:\\ODM\\SuperBuild\\install\\bin\\InterfaceVisualSFM\" -w \"");
    cmd_mvs.append("\\InterfaceVisualSFM\" -w \"");
    cmd_mvs.append(outputPath().toString());
    cmd_mvs.append("\" -i model.nvm -o model.mvs -v 0");

    msgInfo("Process: %s", cmd_mvs.c_str());
    tl::Process process(cmd_mvs);

    process.run();

    TL_ASSERT(process.status() == tl::Process::Status::finalized, "MVS error");

    std::this_thread::sleep_for(std::chrono::nanoseconds(1000));

    tl::Path model = outputPath();
    model.append("model.mvs");

    TL_ASSERT(model.exists(), "model.mvs not found");

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}

void MvsDensifier::densify()
{
  try {

    tl::Path app_path = tl::App::instance().path();
    std::string cmd_mvs("\"");
    cmd_mvs.append(app_path.parentPath().toString());
    cmd_mvs.append("\\DensifyPointCloud\" -w \"");
    //cmd_mvs.append("E:\\ODM\\SuperBuild\\install\\bin\\DensifyPointCloud\" -w \"");
    cmd_mvs.append(outputPath().toString());
    cmd_mvs.append("\" -i model.mvs -o model_dense.mvs -v 0");
    cmd_mvs.append(" --resolution-level ").append(std::to_string(MvsProperties::resolutionLevel()));
    cmd_mvs.append(" --min-resolution ").append(std::to_string(MvsProperties::minResolution()));
    cmd_mvs.append(" --max-resolution ").append(std::to_string(MvsProperties::maxResolution()));
    cmd_mvs.append(" --number-views ").append(std::to_string(MvsProperties::numberViews()));
    cmd_mvs.append(" --number-views-fuse ").append(std::to_string(MvsProperties::numberViewsFuse()));

    msgInfo("Process: %s", cmd_mvs.c_str());
    tl::Process process(cmd_mvs);

    process.run();

    TL_ASSERT(process.status() == tl::Process::Status::finalized, "Densify Point Cloud error");

    std::this_thread::sleep_for(std::chrono::nanoseconds(5000));

    tl::Path dense_model = outputPath();
    dense_model.append("model_dense.ply");
    
    TL_ASSERT(dense_model.exists(), "Densify Point Cloud error");

    setDenseModel(QString::fromStdWString(dense_model.toWString()));

    msgInfo("Dense model write at: %s", dense_model.toString().c_str());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}

void MvsDensifier::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Densification finished");
    chrono.run();

    this->clearPreviousModel();

    tl::Path undistort_path(outputPath());
    undistort_path.append("undistort");
    undistort_path.createDirectories();

    tl::Path models_path(outputPath());
    models_path.append("models");
    models_path.createDirectory();

    /// Da problemas al escribir el fichero nvm e importarlo
    /// Intentar exportar directamente. Por ahora exportar a Colmap  utilizar InterfaceColmap para convertirlo a OpenMVS
    this->exportToComap();
    this->writeNVMFile(); 
    
    if (status() == tl::Task::Status::stopping) return;

    this->undistort(QString::fromStdWString(undistort_path.toWString()));
    
    if (status() == tl::Task::Status::stopping) return;

    this->exportToMVS();
    this->densify();

    chrono.stop();

    if (progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("MVS error");
  }
}

} // namespace graphos
