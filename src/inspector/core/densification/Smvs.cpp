#include "Smvs.h"

//#include <inspector/process/reconstruct_export.h>

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/process.h>

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
#endif // HAVE_CUDA

// BOOST
#include <boost/algorithm/string.hpp>
using namespace inspector;
using namespace tl;
#if (__cplusplus >= 201703L)
namespace fs = std::filesystem;
#else
namespace fs = boost::filesystem;
#endif

namespace inspector
{


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


SmvsProcess::SmvsProcess()
{
}

SmvsProcess::SmvsProcess(const SmvsProcess &smvs)
  : SmvsProperties(smvs)
{
}

SmvsProcess::SmvsProcess(SmvsProcess &&agastDetector) noexcept
  : SmvsProperties(std::forward<SmvsProperties>(agastDetector))
{
}

SmvsProcess::SmvsProcess(int inputImageScale,
                         int outputDepthScale,
                         bool shadingBasedOptimization,
                         bool semiGlobalMatching,
                         double surfaceSmoothingFactor)
{
  SmvsProcess::setInputImageScale(inputImageScale);
  SmvsProcess::setOutputDepthScale(outputDepthScale);
  SmvsProcess::setShadingBasedOptimization(shadingBasedOptimization);
  SmvsProcess::setSemiGlobalMatching(semiGlobalMatching);
  SmvsProcess::setSurfaceSmoothingFactor(surfaceSmoothingFactor);
}

SmvsProcess &SmvsProcess::operator =(const SmvsProcess &smvs)
{
  if (this != &smvs){
    SmvsProperties::operator=(smvs);

  }
  return *this;
}

SmvsProcess &SmvsProcess::operator =(SmvsProcess &&smvs) noexcept
{
  if (this != &smvs){
    SmvsProperties::operator=(std::forward<SmvsProperties>(smvs));

  }
  return *this;
}

void SmvsProcess::reset()
{
  SmvsProperties::reset();
}

bool SmvsProcess::undistort(const std::string &reconstructionPath, 
                            const std::string &imagesPath, 
                            const std::string &outputPath)
{
  
//  colmap::Reconstruction reconstruction;
//#ifdef _DEBUG
//  //Lectura como texto
//  reconstruction.ReadText(reconstructionPath);
//#else
//  //Lectura como binario
//  reconstruction.ReadBinary(reconstructionPath);
//#endif

//  ReconstructionExport reconstructionExport;
//  reconstructionExport.setReconstruction(&reconstruction);
//  reconstructionExport.exportMVE(outputPath);

//  for (auto &camera : reconstruction.Cameras()) {

//    double sensor_width_px = std::max(camera.second.Width(), camera.second.Height());
//    float focal = static_cast<float>(camera.second.FocalLength() /*/ sensor_width_px*/);
//    float ppx = static_cast<float>(camera.second.PrincipalPointX() /*/ sensor_width_px*/);
//    float ppy = static_cast<float>(camera.second.PrincipalPointY() /*/ sensor_width_px*/);
//    int model_id = camera.second.ModelId();

//    cv::Size imageSize(camera.second.Width(), camera.second.Height());
//    std::array<std::array<float, 3>, 3> camera_matrix_data = {focal, 0, ppx,
//                                  0.f, focal, ppy,
//                                  0.f, 0.f, 1.f};
//    cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32F, camera_matrix_data.data());
//    cv::Mat distCoeffs = cv::Mat::zeros(1, 5, CV_32F);
//    std::vector<double> params = camera.second.Params();
//    distCoeffs.at<float>(0) = (model_id == 0 ? 0.f : static_cast<float>(params[3]));
//    distCoeffs.at<float>(1) = (model_id == 3 || model_id == 50 ? static_cast<float>(params[4]) : 0.0f);
//    distCoeffs.at<float>(2) = (model_id == 50 ? static_cast<float>(params[6]) : 0.0f);
//    distCoeffs.at<float>(3) = (model_id == 50 ? static_cast<float>(params[7]) : 0.0f);
//    distCoeffs.at<float>(4) = (model_id == 50 ? static_cast<float>(params[5]) : 0.0f);

//    cv::Mat map1, map2;
//    cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0);
//    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), optCameraMat, imageSize, CV_32FC1, map1, map2);

//#ifdef HAVE_CUDA
//    cv::cuda::GpuMat gMap1(map1);
//    cv::cuda::GpuMat gMap2(map2);
//#endif

//    try {
//      for (auto &image : reconstruction.Images()) {
//        if (image.second.CameraId() == camera.second.CameraId()) {


//          std::string image_file = imagesPath;
//          image_file.append("/").append(image.second.Name());
//          cv::Mat img = cv::imread(image_file.c_str(), cv::IMREAD_COLOR);
//          cv::Mat img_undistort;
//#ifdef HAVE_CUDA
//          cv::cuda::GpuMat gImgOut(img);
//          img.release();
//          cv::cuda::GpuMat gImgUndistort;

//          cv::cuda::Stream stream;
//          cv::cuda::remap(gImgOut, gImgUndistort, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar(), stream);
//          gImgUndistort.download(img_undistort);
//#else
//          cv::remap(img, img_undistort, map1, map2, cv::INTER_LINEAR);
//#endif


//          std::string image_file_undistort = outputPath;
//          image_file_undistort.append("\\views\\view_000").append(std::to_string(image.second.ImageId() - 1)).append(".mve\\").append("undistorted.png");
//          cv::imwrite(image_file_undistort.c_str(), img_undistort);

//        }
//      }
//    } catch (const std::exception &e) {
//      msgError(e.what());
//      return true;
//    }

//  }

  return false;
}

bool SmvsProcess::densify(const std::string &undistortPath)
{

//  /// smvsrecon_SSE41.exe  --scale=2 --output-scale=3 --alpha=1.000000 --force C:\Users\esteban\Documents\Inspector\Projects\planta solar\out"
  
//  fs::path app_path(tl::getRunfile());

//  std::string cmd("/c \"");
//  cmd.append(app_path.parent_path().string());
//  cmd.append("\\smvsrecon_SSE41.exe\" ");
//  cmd.append("--scale=").append(std::to_string(SmvsProperties::inputImageScale()));
//  cmd.append(" --output-scale=").append(std::to_string(SmvsProperties::outputDepthScale()));
//  cmd.append(" --alpha=").append(std::to_string(SmvsProperties::surfaceSmoothingFactor()));
//  cmd.append(" --force ");
//  if (!SmvsProperties::semiGlobalMatching())
//    cmd.append(" --no-sgm ");
//  if (SmvsProperties::shadingBasedOptimization())
//    cmd.append(" --shading ");
//  cmd.append(undistortPath);
//  tl::CmdProcess process(cmd);
//  if (process.run() == tl::Process::Status::error) {
//    return true;
//  }

  return false;
}

} // namespace inspector
