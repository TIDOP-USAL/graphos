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

#include "graphos/core/densification/Smvs.h"

#include "graphos/core/orientation/orientationexport.h"
#include "graphos/core/utils.h"
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


SmvsProperties::SmvsProperties()
  : mInputImageScale(1),
    mOutputDepthScale(2),
    mShadingBasedOptimization(false),
    mSemiGlobalMatching(true),
    mSurfaceSmoothingFactor(1.0)
{
}

SmvsProperties::SmvsProperties(const SmvsProperties &smvs)
  : Smvs(smvs),
    mInputImageScale(smvs.mInputImageScale),
    mOutputDepthScale(smvs.mOutputDepthScale),
    mShadingBasedOptimization(smvs.mShadingBasedOptimization),
    mSemiGlobalMatching(smvs.mSemiGlobalMatching),
    mSurfaceSmoothingFactor(smvs.mSurfaceSmoothingFactor)
{
}

SmvsProperties::SmvsProperties(SmvsProperties &&smvs) noexcept
  : mInputImageScale(smvs.mInputImageScale),
    mOutputDepthScale(smvs.mOutputDepthScale),
    mShadingBasedOptimization(smvs.mShadingBasedOptimization),
    mSemiGlobalMatching(smvs.mSemiGlobalMatching),
    mSurfaceSmoothingFactor(smvs.mSurfaceSmoothingFactor)
{
}

SmvsProperties &SmvsProperties::operator =(const SmvsProperties &smvs)
{
  if (this != &smvs) {
    this->mInputImageScale = smvs.mInputImageScale;
    this->mOutputDepthScale = smvs.mOutputDepthScale;
    this->mShadingBasedOptimization = smvs.mShadingBasedOptimization;
    this->mSemiGlobalMatching = smvs.mSemiGlobalMatching;
    this->mSurfaceSmoothingFactor = smvs.mSurfaceSmoothingFactor;
  }
  return *this;
}

SmvsProperties &SmvsProperties::operator =(SmvsProperties &&smvs) noexcept
{
  if (this != &smvs) {
    this->mInputImageScale = smvs.mInputImageScale;
    this->mOutputDepthScale = smvs.mOutputDepthScale;
    this->mShadingBasedOptimization = smvs.mShadingBasedOptimization;
    this->mSemiGlobalMatching = smvs.mSemiGlobalMatching;
    this->mSurfaceSmoothingFactor = smvs.mSurfaceSmoothingFactor;
  }
  return *this;
}

int SmvsProperties::inputImageScale() const
{
  return mInputImageScale;
}

int SmvsProperties::outputDepthScale() const
{
  return mOutputDepthScale;
}

bool SmvsProperties::shadingBasedOptimization() const
{
  return mShadingBasedOptimization;
}

bool SmvsProperties::semiGlobalMatching() const
{
  return mSemiGlobalMatching;
}

double SmvsProperties::surfaceSmoothingFactor() const
{
  return mSurfaceSmoothingFactor;
}

void SmvsProperties::setInputImageScale(int inputImageScale)
{
  mInputImageScale = inputImageScale;
}

void SmvsProperties::setOutputDepthScale(int outputDepthScale)
{
  mOutputDepthScale = outputDepthScale;
}

void SmvsProperties::setShadingBasedOptimization(bool shadingBasedOptimization)
{
  mShadingBasedOptimization = shadingBasedOptimization;
}

void SmvsProperties::setSemiGlobalMatching(bool semiGlobalMatching)
{
  mSemiGlobalMatching = semiGlobalMatching;
}

void SmvsProperties::setSurfaceSmoothingFactor(double surfaceSmoothingFactor)
{
  mSurfaceSmoothingFactor = surfaceSmoothingFactor;
}

void SmvsProperties::reset()
{
  mInputImageScale = 1;
  mOutputDepthScale = 2;
  mShadingBasedOptimization = false;
  mSemiGlobalMatching = true;
  mSurfaceSmoothingFactor = 1.0;
}

QString SmvsProperties::name() const
{
  return QString("SMVS");
}


/*----------------------------------------------------------------*/


SmvsDensifier::SmvsDensifier()
  : bOpenCvRead(true),
    bCuda(false),
    mOutputPath(""),
    mReconstruction(nullptr),
    mCalibrationReader(nullptr)
{
}

SmvsDensifier::SmvsDensifier(int inputImageScale,
                             int outputDepthScale,
                             bool shadingBasedOptimization,
                             bool semiGlobalMatching,
                             double surfaceSmoothingFactor,
                             bool cuda)
  : bOpenCvRead(true),
    bCuda(cuda),
    mOutputPath(""),
    mReconstruction(nullptr),
    mCalibrationReader(nullptr)
{
  SmvsDensifier::setInputImageScale(inputImageScale);
  SmvsDensifier::setOutputDepthScale(outputDepthScale);
  SmvsDensifier::setShadingBasedOptimization(shadingBasedOptimization);
  SmvsDensifier::setSemiGlobalMatching(semiGlobalMatching);
  SmvsDensifier::setSurfaceSmoothingFactor(surfaceSmoothingFactor);
}

SmvsDensifier::~SmvsDensifier()
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

void SmvsDensifier::reset()
{
  SmvsProperties::reset();
  bOpenCvRead = true;
}

bool SmvsDensifier::undistort(const QString &reconstructionPath,
                              const QString &outputPath)
{
  
  mReconstruction = new internal::Reconstruction(reconstructionPath.toStdString());
  mCalibrationReader = new ReadCalibration();
  mCalibrationReader->open(reconstructionPath);

  mOutputPath = outputPath.toStdString();

  this->createDirectories();
  this->writeMVEFile();
  this->undistortImages();


  return false;
}

void SmvsDensifier::createDirectories()
{
  createDirectory(mOutputPath);
}

void SmvsDensifier::createDirectory(const std::string &path)
{
  tl::Path dir(path);

  if (!dir.exists() && !dir.createDirectories()) {
    std::string err = "The output directory cannot be created: ";
    err.append(path);
    throw std::runtime_error(err);
  }
}

void SmvsDensifier::writeMVEFile()
{
  std::string mve_path = mOutputPath + "/synth_0.out";

  std::ofstream stream(mve_path, std::ios::trunc);
  if (stream.is_open()) {

    colmap::Reconstruction &reconstruction = mReconstruction->colmap();

    int camera_count = static_cast<int>(reconstruction.Images().size());
    int feature_count = static_cast<int>(reconstruction.NumPoints3D());

    stream << "drews 1.0\n";
    stream << camera_count << " " << feature_count << "\n";

    const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

    for (auto &camera : reconstruction.Cameras()) {

      std::shared_ptr<Calibration> calibration = mCalibrationReader->calibration(static_cast<int>(camera.first));

      cv::Mat cameraMatrix = openCVCameraMatrix(*calibration);
      cv::Mat distCoeffs = openCVDistortionCoefficients(*calibration);

      cv::Mat optCameraMat;
      cv::Size imageSize(static_cast<int>(camera.second.Width()),
                         static_cast<int>(camera.second.Height()));
      bool b_fisheye = calibration->checkCameraType(Calibration::CameraType::fisheye);

      if (!b_fisheye) {
        optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr);
      } else {
        cv::fisheye::estimateNewCameraMatrixForUndistortRectify(cameraMatrix, distCoeffs, imageSize, cv::Mat(), optCameraMat, 0.0, imageSize);
      }

      double new_focal = ((optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.f) / std::max(camera.second.Width(), camera.second.Height());
      double new_ppx = optCameraMat.at<float>(0, 2) / camera.second.Width();
      double new_ppy = optCameraMat.at<float>(1, 2) / camera.second.Height();

      for (auto &image : reconstruction.Images()) {
          if (image.second.CameraId() == camera.second.CameraId()) {

          Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
          Eigen::Vector3d translation = image.second.Tvec();

          std::string ini_file(mOutputPath);
          ini_file.append(colmap::StringPrintf("\\views\\view_%04d.mve", image.second.ImageId() - 1));
          createDirectory(ini_file);

          ini_file.append("\\meta.ini");
          std::ofstream stream_ini(ini_file, std::ios::trunc);

          if (stream_ini.is_open()) {
            stream_ini << "# MVE view meta data is stored in INI-file syntax.\n";
            stream_ini << "# This file is generated, formatting will get lost.\n\n";
            stream_ini << "[camera]\n";
            stream_ini << "focal_length = " << new_focal << "\n";
            stream_ini << "pixel_aspect = " << 1. << "\n";
            stream_ini << "principal_point = " << new_ppx << " " << new_ppy << "\n";
            stream_ini << "rotation = " << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << " "
                       << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << " "
                       << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << "\n";
            stream_ini << "translation = " << translation[0] << " " << translation[1] << " " << translation[2] << "\n\n";
            stream_ini << "[view]\n";
            stream_ini << "id = " << image.second.ImageId() - 1 << "\n";
            stream_ini << "name = " << image.second.Name().c_str() << std::endl;

            stream_ini.close();
          }
          
          TL_TODO("El formato bundler r10, r11, r12, r20, r21, r22, T1 y T2 se invierte el signo!!! Aqui supongo que habría que hacerlo igual")
          stream << new_focal << " " << "0" << " " << "0" << "\n";
          stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << "\n";
          stream << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << "\n";
          stream << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << "\n";
          stream << translation[0] << " " << translation[1] << " " << translation[2] << std::endl;

        }
      }
    }

    for (auto &points_3d : reconstruction.Points3D()) {

      Eigen::Vector3ub color = points_3d.second.Color();
      stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << "\n";

      stream << static_cast<int>(color[0]) << " " <<
                static_cast<int>(color[1]) << " " <<
                static_cast<int>(color[2]) << "\n";

      colmap::Track track = points_3d.second.Track();

      std::map<int, int> track_ids_not_repeat;
      for (auto &element : track.Elements()) {
        track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
      }

      stream << track_ids_not_repeat.size();

      for (auto &map : track_ids_not_repeat) {
        stream << " " << map.first << " " << map.second << " 0";
      }


      stream << std::endl;
    }

    stream.close();

  }
}

void SmvsDensifier::undistortImages()
{
  colmap::Reconstruction &reconstruction = mReconstruction->colmap();
  const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

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

    try {

      for (auto &image : reconstruction.Images()) {

        if (image.second.CameraId() == camera.second.CameraId()) {

          std::string image_file = image.second.Name();

          cv::Mat img;

          if (bOpenCvRead) {
            img = cv::imread(image_file.c_str(), cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);
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
                   
          if (img.channels() == 1) {
            cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
          }

          cv::Mat img_undistort;
#ifdef HAVE_CUDA
          if (bCuda) {
            cv::cuda::GpuMat gImgOut(img);
            img.release();
            cv::cuda::GpuMat gImgUndistort;

            cv::cuda::Stream stream;
            cv::cuda::GpuMat gMap1(map1);
            cv::cuda::GpuMat gMap2(map2);
            cv::cuda::remap(gImgOut, gImgUndistort, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar(), stream);
            gImgUndistort.download(img_undistort);
          } else {
#endif
            cv::remap(img, img_undistort, map1, map2, cv::INTER_LINEAR);
            img.release();
#ifdef HAVE_CUDA
          }
#endif
          msgInfo("Undistort image: %s", image_file.c_str());

          std::string image_file_undistort = mOutputPath;
          image_file_undistort.append(colmap::StringPrintf("\\views\\view_%04d.mve\\", image.second.ImageId() - 1));
          image_file_undistort.append("undistorted.png");
          cv::imwrite(image_file_undistort.c_str(), img_undistort);

        }
      }
    } catch (const std::exception &e) {
      msgError(e.what());
    }

  }
}

bool SmvsDensifier::densify(const QString &undistortPath)
{

  tl::Path app_path = tl::App::instance().path();

  std::string cmd("\"");
  cmd.append(app_path.parentPath().toString());
  cmd.append("\\smvsrecon_SSE41.exe\" ");
  cmd.append("--scale=").append(std::to_string(SmvsProperties::inputImageScale()));
  cmd.append(" --output-scale=").append(std::to_string(SmvsProperties::outputDepthScale()));
  cmd.append(" --alpha=").append(std::to_string(SmvsProperties::surfaceSmoothingFactor()));
  cmd.append(" --force ");
  if (!SmvsProperties::semiGlobalMatching())
    cmd.append(" --no-sgm ");
  if (SmvsProperties::shadingBasedOptimization())
    cmd.append(" --shading ");
  cmd.append("\"").append(undistortPath.toStdString()).append("\"");
  tl::Process process(cmd);
  process.run();

  return false;
}

void SmvsDensifier::enableCuda(bool enable)
{
  bCuda = enable;
}

} // namespace graphos
