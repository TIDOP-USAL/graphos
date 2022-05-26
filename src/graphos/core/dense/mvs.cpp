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

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/core/app.h>
#include <tidop/img/imgreader.h>

// COLMAP
#include <colmap/base/reconstruction.h>
#include <colmap/base/undistortion.h>

// OPENCV
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#ifdef HAVE_CUDA
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>
#endif // HAVE_CUDA

#include <fstream>
#include <iomanip>

namespace graphos
{

namespace internal
{

class Reconstruction
{

public:

  Reconstruction(const std::string &path)
    : mReconstruction(new colmap::Reconstruction())
  {
    mReconstruction->ReadBinary(path);
  }

  ~Reconstruction()
  {
    if (mReconstruction) {
      delete mReconstruction;
      mReconstruction = nullptr;
    }
  }

  colmap::Reconstruction &colmap()
  {
    return *mReconstruction;
  }

private:

  colmap::Reconstruction *mReconstruction;
};

}

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


MvsDensifier::MvsDensifier()
  : bOpenCvRead(true),
    bCuda(false),
    mOutputPath(""),
    mReconstruction(nullptr),
    mCalibrationReader(nullptr)
{

}

MvsDensifier::MvsDensifier(int resolutionLevel,
                           int minResolution,
                           int maxResolution,
                           int numberViews,
                           int numberViewsFuse,
                           bool cuda)
  : bOpenCvRead(true),
    bCuda(cuda),
    mOutputPath(""),
    mReconstruction(nullptr),
    mCalibrationReader(nullptr)
{
  MvsProperties::setResolutionLevel(resolutionLevel);
  MvsProperties::setMinResolution(minResolution);
  MvsProperties::setMaxResolution(maxResolution);
  MvsProperties::setNumberViews(numberViews);
  MvsProperties::setNumberViewsFuse(numberViewsFuse);
}

MvsDensifier::~MvsDensifier()
{
  if (mReconstruction) {
    delete mReconstruction;
    mReconstruction = nullptr;
  }

  if (mCalibrationReader) {
    delete mCalibrationReader;
    mCalibrationReader = nullptr;
  }
}

void MvsDensifier::undistort(const QString &reconstructionPath,
                             const QString &outputPath)
{
  mReconstruction = new internal::Reconstruction(reconstructionPath.toStdString());
  mCalibrationReader = new ReadCalibration();
  mCalibrationReader->open(reconstructionPath);

  mOutputPath = outputPath.toStdString();
  mOutputPath.append("mvs");

  this->clearPreviousModel();
  this->createDirectories();
  this->writeNVMFile();
  this->undistortImages();
  this->exportToMVS();

}

void MvsDensifier::densify(const QString &undistortPath)
{
  try {

    tl::Path app_path = tl::App::instance().path();
    std::string cmd_mvs("\"");
    //cmd_mvs.append(app_path.parentPath().toString());
    cmd_mvs.append("E:\\ODM\\SuperBuild\\install\\bin\\DensifyPointCloud\" -w \"");
    cmd_mvs.append(mOutputPath.toString());
    cmd_mvs.append("\" -i model.mvs -o model_dense.mvs -v 0");
    cmd_mvs.append(" --resolution-level ").append(std::to_string(MvsProperties::resolutionLevel()));
    cmd_mvs.append(" --min-resolution ").append(std::to_string(MvsProperties::minResolution()));
    cmd_mvs.append(" --max-resolution ").append(std::to_string(MvsProperties::maxResolution()));
    cmd_mvs.append(" --number-views ").append(std::to_string(MvsProperties::numberViews()));
    cmd_mvs.append(" --number-views-fuse ").append(std::to_string(MvsProperties::numberViewsFuse()));
   
    msgInfo("Process: %s", cmd_mvs.c_str());
    tl::Process process(cmd_mvs);

    process.run();
   
    if (process.status() == tl::Process::Status::error) TL_THROW_EXCEPTION(cmd_mvs.c_str());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}

void MvsDensifier::enableCuda(bool enable)
{
  bCuda = enable;
}

void MvsDensifier::reset()
{
  MvsProperties::reset();
  bOpenCvRead = true;
}

void MvsDensifier::clearPreviousModel()
{
  mOutputPath.removeDirectory();
}

void MvsDensifier::createDirectories()
{
  mOutputPath.createDirectory();
  createDirectory("undistort");
  createDirectory("models");
  //createDirectory(std::string(mOutputPath).append("/temp"));
}

void MvsDensifier::createDirectory(const std::string &dir)
{
  tl::Path path(mOutputPath);
  path.append(dir);
  if (!path.exists() && !path.createDirectories()) {
    std::string err = "The output directory cannot be created: ";
    err.append(path.toString());
    throw std::runtime_error(err);
  }
}

void MvsDensifier::writeNVMFile()
{
  try {

    TL_ASSERT(mReconstruction, "There is not a valid reconstruction");

    colmap::Reconstruction &reconstruction = mReconstruction->colmap();

    tl::Path nvm_path(mOutputPath);
    //nvm_path.append("models");
    nvm_path.append("model.nvm");

    std::ofstream stream(nvm_path.toString(), std::ios::trunc);

    if (stream.is_open()) {

      int camera_count = reconstruction.Images().size();
      int feature_count = reconstruction.NumPoints3D();

      stream << "NVM_V3 \n\n" << camera_count << "\n";
      
      stream << std::fixed << std::setprecision(12);

      const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

      for (auto &camera : reconstruction.Cameras()) {

        std::shared_ptr<Calibration> calibration = mCalibrationReader->calibration(static_cast<int>(camera.first));

        float focal_x{};
        float focal_y{};
        if (calibration->existParameter(Calibration::Parameters::focal)) {
          focal_x = static_cast<float>(calibration->parameter(Calibration::Parameters::focal));
          focal_y = static_cast<float>(calibration->parameter(Calibration::Parameters::focal));
        } else {
          focal_x = static_cast<float>(calibration->parameter(Calibration::Parameters::focalx));
          focal_y = static_cast<float>(calibration->parameter(Calibration::Parameters::focaly));
        }

        float ppx = static_cast<float>(calibration->parameter(Calibration::Parameters::cx));
        float ppy = static_cast<float>(calibration->parameter(Calibration::Parameters::cy));

        cv::Size imageSize(static_cast<int>(camera.second.Width()),
                           static_cast<int>(camera.second.Height()));
        std::array<std::array<float, 3>, 3> camera_matrix_data = {focal_x, 0.f, ppx,
                                                                  0.f, focal_y, ppy,
                                                                  0.f, 0.f, 1.f};
        cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32F, camera_matrix_data.data());
        cv::Mat distCoeffs = cv::Mat::zeros(1, 5, CV_32F);
        distCoeffs.at<float>(0) = calibration->existParameter(Calibration::Parameters::k1) ?
          static_cast<float>(calibration->parameter(Calibration::Parameters::k1)) : 0.f;
        distCoeffs.at<float>(1) = calibration->existParameter(Calibration::Parameters::k2) ?
          static_cast<float>(calibration->parameter(Calibration::Parameters::k2)) : 0.f;
        distCoeffs.at<float>(2) = calibration->existParameter(Calibration::Parameters::p1) ?
          static_cast<float>(calibration->parameter(Calibration::Parameters::p1)) : 0.f;
        distCoeffs.at<float>(3) = calibration->existParameter(Calibration::Parameters::p2) ?
          static_cast<float>(calibration->parameter(Calibration::Parameters::p2)) : 0.f;
        distCoeffs.at<float>(4) = calibration->existParameter(Calibration::Parameters::k3) ?
          static_cast<float>(calibration->parameter(Calibration::Parameters::k3)) : 0.f;

        cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr, true);
        float new_focal = (optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.f;

        for (size_t i = 0; i < reg_image_ids.size(); ++i) {
          colmap::image_t image_id = reg_image_ids[i];
          colmap::Image &image = reconstruction.Image(image_id);
          if (image.CameraId() == camera.second.CameraId()) {

            Eigen::Vector3d projection_center = image.ProjectionCenter();

            stream << "undistort/" << tl::Path(image.Name()).fileName() << " ";
            stream << new_focal << " ";
            stream << image.Qvec(0) << " ";
            stream << image.Qvec(1) << " ";
            stream << image.Qvec(2) << " ";
            stream << image.Qvec(3) << " ";
            stream << projection_center(0) << " ";
            stream << projection_center(1) << " ";
            stream << projection_center(2) << " ";
            stream << 0 << " ";
            stream << 0 << std::endl;

          }
        }
      }

      stream << "\n" << feature_count << std::endl;

      for (auto &points_3d : reconstruction.Points3D()) {

        stream << points_3d.second.X() << " "
          << points_3d.second.Y() << " "
          << points_3d.second.Z() << " "
          << static_cast<int>(points_3d.second.Color(0)) << " "
          << static_cast<int>(points_3d.second.Color(1)) << " "
          << static_cast<int>(points_3d.second.Color(2)) << " ";



        colmap::Track track = points_3d.second.Track();

        std::map<int, int> track_ids_not_repeat;
        for (auto &element : track.Elements()) {
          track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
        }

        stream << track_ids_not_repeat.size();

        for (auto &map : track_ids_not_repeat) {

          colmap::image_t image_id = map.first + 1;
          const colmap::Image &image = reconstruction.Image(image_id);
          const colmap::Camera &camera = reconstruction.Camera(image.CameraId());

          const colmap::Point2D &point2D = image.Point2D(map.second);

          stream << " " << static_cast<int>(image_id - 1)
            << " " << map.second
            << " " << point2D.X()
            << " " << point2D.Y();
        }

        stream << std::endl;
      }

      stream.close();
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Densification error");
  }
}

void MvsDensifier::undistortImages()
{
  colmap::Reconstruction &reconstruction = mReconstruction->colmap();
  const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

  tl::Path output_images_path(mOutputPath);
  output_images_path.append("undistort");

  for (auto &camera : reconstruction.Cameras()) {

    std::shared_ptr<Calibration> calibration = mCalibrationReader->calibration(static_cast<int>(camera.first));
    
    cv::Mat cameraMatrix = openCVCameraMatrix(*calibration);
    cv::Mat distCoeffs = openCVDistortionCoefficients(*calibration);

    cv::Mat map1;
    cv::Mat map2;
    cv::Mat optCameraMat;
    cv::Size imageSize(static_cast<int>(camera.second.Width()),
                       static_cast<int>(camera.second.Height()));
    bool b_fisheye = calibration->checkCameraType(Calibration::CameraType::fisheye);

    if (!b_fisheye) {
      optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr);
      cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), optCameraMat, imageSize, CV_32FC1, map1, map2);
    } else {
      cv::fisheye::estimateNewCameraMatrixForUndistortRectify(cameraMatrix, distCoeffs, imageSize, cv::Mat(), optCameraMat, 0.0, imageSize);
      cv::fisheye::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), optCameraMat, imageSize, CV_32FC1, map1, map2);
    }

    TL_TODO("undistortImage()")

    for (size_t i = 0; i < reg_image_ids.size(); ++i) {

      colmap::image_t image_id = reg_image_ids[i];
      colmap::Image &image = reconstruction.Image(image_id);

      if (image.CameraId() == camera.second.CameraId()) {

        std::string image_file = image.Name();

        cv::Mat img;
        cv::Mat img_original;

        if (bOpenCvRead) {
          img = cv::imread(image_file, cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);

          if (img.empty()) {
            bOpenCvRead = false;
          }
        }

        if (!bOpenCvRead) {
          std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image_file);
          imageReader->open();
          if (imageReader->isOpen()) {
            img = imageReader->read();
            if (imageReader->depth() != 8) {

              TL_TODO("Codigo duplicado en FeatureExtractorProcess")

#ifdef HAVE_CUDA
                if (bCuda) {
                  cv::cuda::GpuMat gImgIn(img);
                  cv::cuda::GpuMat gImgOut;
                  cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
                  gImgOut.download(img);
                } else {
#endif
                  cv::normalize(img, img, 0., 255., cv::NORM_MINMAX, CV_8U);
#ifdef HAVE_CUDA
                }
#endif
            }

            imageReader->close();
          }
        }

        TL_TODO("Las imágenes en escala de grises con canal alfa no estan comprobadas")
          if (img.channels() == 1) {
            cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
          }

        cv::Mat img_undistort;
#ifdef HAVE_CUDA
        if (bCuda) {
          TL_TODO("comprobar versión driver cuda");
          cv::cuda::GpuMat gImgOut(img);
          img.release();
          cv::cuda::GpuMat gImgUndistort;

          cv::cuda::GpuMat gMap1(map1);
          cv::cuda::GpuMat gMap2(map2);
          cv::cuda::remap(gImgOut, gImgUndistort, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar()/*, stream1*/);
          gImgUndistort.download(img_undistort);

        } else {
#endif
          cv::remap(img, img_undistort, map1, map2, cv::INTER_LINEAR);
          img.release();

#ifdef HAVE_CUDA
        }
#endif
        tl::Path image_path(image_file);

        msgInfo("Undistort image: %s", image_path.fileName().toString().c_str());

        tl::Path undistort_image_path(output_images_path);
        undistort_image_path.append(image_path.fileName());

        cv::imwrite(undistort_image_path.toString(), img_undistort);


      }
    }

  }
}

void MvsDensifier::undistortImage()
{

}

void MvsDensifier::exportToMVS()
{
  // InterfaceVisualSFM.exe -w "C:\Users\Tido\OneDrive - Universidad de Salamanca\Documents\graphos\Projects\ZufreFotogrametria\dense\pmvs\visualize" 
  //                        -i "C:\Users\Tido\OneDrive - Universidad de Salamanca\Documents\graphos\Projects\ZufreFotogrametria\dense\mvs\model.nvm" 
  //                        -o "C:\Users\Tido\OneDrive - Universidad de Salamanca\Documents\graphos\Projects\ZufreFotogrametria\dense\mvs\model.nvs" -v 0

  try {

    tl::Path app_path = tl::App::instance().path();
    std::string cmd_mvs("\"");
    //cmd_mvs.append(app_path.parentPath().toString());
    cmd_mvs.append("E:\\ODM\\SuperBuild\\install\\bin\\InterfaceVisualSFM\" -w \"");
    //cmd_mvs.append("C:\\OpenMVS\\InterfaceCOLMAP.exe\" - w \"");
    cmd_mvs.append(mOutputPath.toString());
    cmd_mvs.append("\" -i model.nvm -o model.mvs -v 0");

    msgInfo("Process: %s", cmd_mvs.c_str());
    tl::Process process(cmd_mvs);
   
    process.run();
  
    if (process.status() == tl::Process::Status::error) TL_THROW_EXCEPTION(cmd_mvs.c_str());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}


} // namespace graphos
