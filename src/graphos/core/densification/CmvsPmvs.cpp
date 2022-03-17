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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/core/densification/CmvsPmvs.h"

#include "graphos/core/camera/Calibration.h"
#include "graphos/core/camera/Colmap.h"
#include "graphos/core/camera/Utils.h"

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/process.h>
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


CmvsPmvsProperties::CmvsPmvsProperties()
  : mUseVisibilityInformation(true),
    mImagesPerCluster(100),
    mLevel(1),
    mCellSize(2),
    mThreshold(0.7),
    mWindowSize(7),
    mMinimunImageNumber(3)
{
}

CmvsPmvsProperties::CmvsPmvsProperties(const CmvsPmvsProperties &cmvsPmvs)
  : CmvsPmvs(cmvsPmvs),
    mUseVisibilityInformation(cmvsPmvs.mUseVisibilityInformation),
    mImagesPerCluster(cmvsPmvs.mImagesPerCluster),
    mLevel(cmvsPmvs.mLevel),
    mCellSize(cmvsPmvs.mCellSize),
    mThreshold(cmvsPmvs.mThreshold),
    mWindowSize(cmvsPmvs.mWindowSize),
    mMinimunImageNumber(cmvsPmvs.mMinimunImageNumber)
{
}

CmvsPmvsProperties::CmvsPmvsProperties(CmvsPmvsProperties &&cmvsPmvs) noexcept
  : CmvsPmvs(std::forward<CmvsPmvs>(cmvsPmvs)),
    mUseVisibilityInformation(cmvsPmvs.mUseVisibilityInformation),
    mImagesPerCluster(cmvsPmvs.mImagesPerCluster),
    mLevel(cmvsPmvs.mLevel),
    mCellSize(cmvsPmvs.mCellSize),
    mThreshold(cmvsPmvs.mThreshold),
    mWindowSize(cmvsPmvs.mWindowSize),
    mMinimunImageNumber(cmvsPmvs.mMinimunImageNumber)
{
}

CmvsPmvsProperties &CmvsPmvsProperties::operator =(const CmvsPmvsProperties &cmvsPmvs)
{
  if (this != &cmvsPmvs) {
    this->mUseVisibilityInformation = cmvsPmvs.mUseVisibilityInformation;
    this->mImagesPerCluster = cmvsPmvs.mImagesPerCluster;
    this->mLevel = cmvsPmvs.mLevel;
    this->mCellSize = cmvsPmvs.mCellSize;
    this->mThreshold = cmvsPmvs.mThreshold;
    this->mWindowSize = cmvsPmvs.mWindowSize;
    this->mMinimunImageNumber = cmvsPmvs.mMinimunImageNumber;
  }
  return *this;
}

CmvsPmvsProperties &CmvsPmvsProperties::operator =(CmvsPmvsProperties &&cmvsPmvs) noexcept
{
  if (this != &cmvsPmvs) {
    this->mUseVisibilityInformation = cmvsPmvs.mUseVisibilityInformation;
    this->mImagesPerCluster = cmvsPmvs.mImagesPerCluster;
    this->mLevel = cmvsPmvs.mLevel;
    this->mCellSize = cmvsPmvs.mCellSize;
    this->mThreshold = cmvsPmvs.mThreshold;
    this->mWindowSize = cmvsPmvs.mWindowSize;
    this->mMinimunImageNumber = cmvsPmvs.mMinimunImageNumber;
  }
  return *this;
}

bool CmvsPmvsProperties::useVisibilityInformation() const
{
  return mUseVisibilityInformation;
}

int CmvsPmvsProperties::imagesPerCluster() const
{
  return mImagesPerCluster;
}

int CmvsPmvsProperties::level() const
{
  return mLevel;
}

int CmvsPmvsProperties::cellSize() const
{
  return mCellSize;
}

double CmvsPmvsProperties::threshold() const
{
  return mThreshold;
}

int CmvsPmvsProperties::windowSize() const
{
  return mWindowSize;
}

int CmvsPmvsProperties::minimunImageNumber() const
{
  return mMinimunImageNumber;
}

void CmvsPmvsProperties::setUseVisibilityInformation(bool useVisibilityInformation)
{
  mUseVisibilityInformation = useVisibilityInformation;
}

void CmvsPmvsProperties::setImagesPerCluster(int imagesPerCluster)
{
  mImagesPerCluster = imagesPerCluster;
}

void CmvsPmvsProperties::setLevel(int level)
{
  mLevel = level;
}

void CmvsPmvsProperties::setCellSize(int cellSize)
{
  mCellSize = cellSize;
}

void CmvsPmvsProperties::setThreshold(double threshold)
{
  mThreshold = threshold;
}

void CmvsPmvsProperties::setWindowSize(int windowSize)
{
  mWindowSize = windowSize;
}

void CmvsPmvsProperties::setMinimunImageNumber(int minimunImageNumber)
{
  mMinimunImageNumber = minimunImageNumber;
}

void CmvsPmvsProperties::reset()
{
  mUseVisibilityInformation = true;
  mImagesPerCluster = 100;
  mLevel = 1;
  mCellSize = 2;
  mThreshold = 0.7;
  mWindowSize = 7;
  mMinimunImageNumber = 3;
}


QString CmvsPmvsProperties::name() const
{
  return QString("CMVS/PMVS");
}




/*----------------------------------------------------------------*/

template <typename Derived>
void WriteMatrix(const Eigen::MatrixBase<Derived>& matrix,
                 std::ofstream* file) {
  typedef typename Eigen::MatrixBase<Derived>::Index index_t;
  for (index_t r = 0; r < matrix.rows(); ++r) {
    for (index_t c = 0; c < matrix.cols() - 1; ++c) {
      *file << matrix(r, c) << " ";
    }
    *file << matrix(r, matrix.cols() - 1) << std::endl;
  }
}

// Write projection matrix P = K * [R t] to file and prepend given header.
void WriteProjectionMatrix(const std::string& path, const colmap::Camera& camera,
                           const colmap::Image& image, const std::string& header) {

  std::ofstream file(path, std::ios::trunc);
  CHECK(file.is_open()) << path;

  Eigen::Matrix3d calib_matrix = Eigen::Matrix3d::Identity();
  calib_matrix(0, 0) = camera.FocalLengthX();
  calib_matrix(1, 1) = camera.FocalLengthY();
  calib_matrix(0, 2) = camera.PrincipalPointX();
  calib_matrix(1, 2) = camera.PrincipalPointY();

  const Eigen::Matrix3x4d proj_matrix = calib_matrix * image.ProjectionMatrix();

  if (!header.empty()) {
    file << header << std::endl;
  }

  WriteMatrix(proj_matrix, &file);
}




CmvsPmvsDensifier::CmvsPmvsDensifier()
  : bOpenCvRead(true),
    bCuda(false),
    mOutputPath(""),
    mReconstruction(nullptr),
    mCalibrationReader(nullptr)
{

}

CmvsPmvsDensifier::CmvsPmvsDensifier(bool useVisibilityInformation,
                                     int imagesPerCluster,
                                     int level,
                                     int cellSize,
                                     double threshold,
                                     int windowSize,
                                     int minimunImageNumber,
                                     bool cuda)
  : bOpenCvRead(true),
    bCuda(cuda),
    mOutputPath(""),
    mReconstruction(nullptr),
    mCalibrationReader(nullptr)
{
  CmvsPmvsProperties::setUseVisibilityInformation(useVisibilityInformation);
  CmvsPmvsProperties::setImagesPerCluster(imagesPerCluster);
  CmvsPmvsProperties::setLevel(level);
  CmvsPmvsProperties::setCellSize(cellSize);
  CmvsPmvsProperties::setThreshold(threshold);
  CmvsPmvsProperties::setWindowSize(windowSize);
  CmvsPmvsProperties::setMinimunImageNumber(minimunImageNumber);
}

CmvsPmvsDensifier::~CmvsPmvsDensifier()
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

bool CmvsPmvsDensifier::undistort(const QString &reconstructionPath,
                                  const QString &outputPath)
{
  mReconstruction = new internal::Reconstruction(reconstructionPath.toStdString());
  mCalibrationReader = new ReadCalibration();
  mCalibrationReader->open(reconstructionPath);

  mOutputPath = outputPath.toStdString() + "/pmvs";

  this->createDirectories();
  this->writeBundleFile(); // Realmente no es necesario crearlo ya que no se usa cmvs.exe ni genOption.exe
  this->undistortImages();
  this->writeVisibility();
  this->writeOptions();

  return false;
}

bool CmvsPmvsDensifier::densify(const QString &undistortPath)
{
  tl::Path app_path = tl::App::instance().path();
  std::string cmd_cmvs("\"");
  cmd_cmvs.append(app_path.parentPath().toString());
  cmd_cmvs.append("\\pmvs2\" \"");
  cmd_cmvs.append(undistortPath.toStdString());
  cmd_cmvs.append("/pmvs/\" option-all");

  msgInfo("Process: %s", cmd_cmvs.c_str());
  tl::ExternalProcess process(cmd_cmvs);
  process.run();

  return false;
}

void CmvsPmvsDensifier::enableCuda(bool enable)
{
  bCuda = enable;
}

void CmvsPmvsDensifier::reset()
{
  CmvsPmvsProperties::reset();
  bOpenCvRead = true;
}

void CmvsPmvsDensifier::createDirectories()
{
  createDirectory(mOutputPath);
  createDirectory(std::string(mOutputPath).append("/txt"));
  createDirectory(std::string(mOutputPath).append("/visualize"));
  createDirectory(std::string(mOutputPath).append("/models"));
}

void CmvsPmvsDensifier::createDirectory(const std::string &path)
{
  tl::Path dir(path);
  if (!dir.exists() && !dir.createDirectories()) {
      std::string err = "The output directory cannot be created: ";
      err.append(path);
      throw std::runtime_error(err);
  }
}

void CmvsPmvsDensifier::writeBundleFile()
{

  if (mReconstruction) {

    colmap::Reconstruction &reconstruction = mReconstruction->colmap();

    tl::Path bundler_path(mOutputPath);
    bundler_path.append("bundle.rd.out");
    tl::Path bundler_path_list(mOutputPath);
    bundler_path_list.append("bundle.rd.out.list.txt");

    std::ofstream stream(bundler_path.toString(), std::ios::trunc);
    std::ofstream stream_image_list(bundler_path_list.toString(), std::ios::trunc);
    std::ofstream stream_image_list_original;

    if (stream.is_open() && stream_image_list.is_open()) {

      int camera_count = reconstruction.Images().size();
      int feature_count = reconstruction.NumPoints3D();

      stream << "# Bundle file v0.3" << std::endl;
      stream << camera_count << " " << feature_count << std::endl;

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

        cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr);
        float new_focal = (optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.f;

        for (size_t i = 0; i < reg_image_ids.size(); ++i) {
          colmap::image_t image_id = reg_image_ids[i];
          colmap::Image &image = reconstruction.Image(image_id);
          if (image.CameraId() == camera.second.CameraId()) {

            Eigen::Matrix3d rotation_matrix = image.RotationMatrix();
            Eigen::Vector3d translation = image.Tvec();
            // En el formato bundler r10, r11, r12, r20, r21, r22, T1 y T2 se invierte el signo
            stream << new_focal << " 0 0 "  << std::endl;
            stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << std::endl;
            stream << -rotation_matrix(1, 0) << " " << -rotation_matrix(1, 1) << " " << -rotation_matrix(1, 2) << std::endl;
            stream << -rotation_matrix(2, 0) << " " << -rotation_matrix(2, 1) << " " << -rotation_matrix(2, 2) << std::endl;
            stream << translation[0] << " " << -translation[1] << " " << -translation[2] << std::endl;

            // Undistorted images
            std::string output_image_path = colmap::StringPrintf("%08d.jpg", i);
            stream_image_list << output_image_path << std::endl;
            
          }
        }
      }


      for (auto &points_3d : reconstruction.Points3D()) {

        Eigen::Vector3ub color = points_3d.second.Color();
        stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << std::endl;

        stream << static_cast<int>(color[0]) << " " <<
                  static_cast<int>(color[1]) << " " <<
                  static_cast<int>(color[2]) << std::endl;

        colmap::Track track = points_3d.second.Track();

        std::map<int, int> track_ids_not_repeat;
        for (auto &element : track.Elements()) {
          track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
        }

        stream << track_ids_not_repeat.size();

        for (auto &map : track_ids_not_repeat) {

          colmap::image_t image_id = map.first+1;
          const colmap::Image &image = reconstruction.Image(image_id);
          const colmap::Camera &camera = reconstruction.Camera(image.CameraId());

          const colmap::Point2D &point2D = image.Point2D(map.second);

          stream << " " << static_cast<int>(image_id) << " " << map.second << " ";
          stream << point2D.X() - camera.PrincipalPointX() << " ";
          stream << camera.PrincipalPointY() - point2D.Y() << " ";
        }

        stream << std::endl;
      }
      stream.close();
      stream_image_list.close();
      stream_image_list_original.close();
    }
  } else 
    msgError("There is not a valid reconstruction");
}

void CmvsPmvsDensifier::undistortImages()
{
  colmap::Reconstruction &reconstruction = mReconstruction->colmap();
  const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

  tl::Path output_images_path(mOutputPath);
  output_images_path.append("visualize");
  tl::Path output_proj_matrix_path(mOutputPath);
  output_proj_matrix_path.append("txt");

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

        std::string output_image_path = mOutputPath + colmap::StringPrintf("/visualize/%08d.jpg", i);

        cv::imwrite(output_image_path, img_undistort);

        std::string proj_matrix_path = mOutputPath + colmap::StringPrintf("/txt/%08d.txt", i);
        std::ofstream file(proj_matrix_path, std::ios::trunc);
        CHECK(file.is_open()) << proj_matrix_path;

        Eigen::Matrix3d calib_matrix = Eigen::Matrix3d::Identity();
        calib_matrix(0, 0) = optCameraMat.at<float>(0, 0);
        calib_matrix(1, 1) = optCameraMat.at<float>(1, 1);
        calib_matrix(0, 2) = optCameraMat.at<float>(0, 2);
        calib_matrix(1, 2) = optCameraMat.at<float>(1, 2);

        const Eigen::Matrix3x4d proj_matrix = calib_matrix * image.ProjectionMatrix();

        file << "CONTOUR" << std::endl;

        WriteMatrix(proj_matrix, &file);
      }
    }

  }
}

void CmvsPmvsDensifier::undistortImage()
{
}

void CmvsPmvsDensifier::writeVisibility()
{
  colmap::Reconstruction &reconstruction = mReconstruction->colmap();

  std::string visibility_path = mOutputPath + "/vis.dat";
  std::ofstream file(visibility_path, std::ios::trunc);
  CHECK(file.is_open()) << visibility_path;

  file << "VISDATA" << std::endl;
  file << reconstruction.NumRegImages() << std::endl;

  const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

  for (size_t i = 0; i < reg_image_ids.size(); ++i) {
    colmap::image_t image_id = reg_image_ids[i];
    colmap::Image &image = reconstruction.Image(image_id);
    std::unordered_set<colmap::image_t> visible_image_ids;
    for (colmap::point2D_t point2D_idx = 0; point2D_idx < image.NumPoints2D();
         ++point2D_idx) {
      colmap::Point2D &point2D = image.Point2D(point2D_idx);
      if (point2D.HasPoint3D()) {
        colmap::Point3D& point3D = reconstruction.Point3D(point2D.Point3DId());
        for (const colmap::TrackElement &track_el : point3D.Track().Elements()) {
          if (track_el.image_id != image_id) {
            for (size_t j = 0; j < reg_image_ids.size(); ++j) {
              colmap::image_t image_id_2 = reg_image_ids[j];
              if (image_id_2 == track_el.image_id){
                visible_image_ids.insert(j);
                break;
              }
            }
          }
        }
      }
    }

    std::vector<colmap::image_t> sorted_visible_image_ids(visible_image_ids.begin(),
                                                          visible_image_ids.end());
    std::sort(sorted_visible_image_ids.begin(), sorted_visible_image_ids.end());

    file << i << " " << visible_image_ids.size();
    for (const colmap::image_t visible_image_id : sorted_visible_image_ids) {
      file << " " << visible_image_id;
    }
    file << std::endl;
  }
}

void CmvsPmvsDensifier::writeOptions()
{
  colmap::Reconstruction &reconstruction = mReconstruction->colmap();

  /// options

  std::string options_path = mOutputPath + "/option-all";
  std::ofstream file_options(options_path, std::ios::trunc);
  CHECK(file_options.is_open()) << options_path;

  TL_TODO("Si hay muchas imagenes separar en clusters pero con solape ya que los generados por cmvs dejan huecos al fusionar")

  file_options << "# Generated by Graphos - all images, no clustering.\n";

  file_options << "level " << CmvsPmvsProperties::level() << "\n";
  file_options << "csize " << CmvsPmvsProperties::cellSize() << "\n";
  file_options << "threshold " << CmvsPmvsProperties::threshold() << "\n";
  file_options << "wsize " << CmvsPmvsProperties::windowSize() << "\n";
  file_options << "minImageNum " << CmvsPmvsProperties::minimunImageNumber() << "\n";
  file_options << "CPU " << std::thread::hardware_concurrency() << "\n";
  file_options << "setEdge 0" << "\n";
  file_options << "useBound 0" << "\n";
  file_options << "useVisData " << (CmvsPmvsProperties::useVisibilityInformation() ? 1 : 0) << "\n";
  file_options << "sequence -1" << "\n";
  file_options << "maxAngle 10" << "\n";
  file_options << "quad 2.0" << "\n";

  file_options << "timages " << reconstruction.NumRegImages();
  for (size_t i = 0; i < reconstruction.NumRegImages(); ++i) {
    file_options << " " << i;
  }
  file_options << "\n";

  file_options << "oimages 0" << std::endl;
}

} // namespace graphos
