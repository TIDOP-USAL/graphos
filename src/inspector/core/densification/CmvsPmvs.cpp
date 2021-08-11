#include "CmvsPmvs.h"

// TIDOP LIB
#include <tidop/core/messages.h>
#include <tidop/core/process.h>
#include <tidop/core/path.h>
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

namespace inspector
{

namespace internal
{

class Reconstruction
{
public:
  Reconstruction(const std::string &path)
    :  mReconstruction(new colmap::Reconstruction())
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
  //CHECK_EQ(camera.ModelId(), colmap::PinholeCameraModel::model_id);

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
    mReconstruction(nullptr)
{

}

CmvsPmvsDensifier::CmvsPmvsDensifier(bool useVisibilityInformation,
                                     int imagesPerCluster,
                                     int level,
                                     int cellSize,
                                     double threshold,
                                     int windowSize,
                                     int minimunImageNumber)
  : bOpenCvRead(true),
    bCuda(false),
    mOutputPath(""),
    mReconstruction(nullptr)
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
}

bool CmvsPmvsDensifier::undistort(const QString &reconstructionPath,
                                  const QString &outputPath)
{
  mReconstruction = new internal::Reconstruction(reconstructionPath.toStdString());

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

  tl::Path app_path(tl::getRunfile());
  //std::string cmd_cmvs("/c \"\"");
  //cmd_cmvs.append(app_path.parentPath().toString());
  //cmd_cmvs.append("\\pmvs2\" \"");
  //cmd_cmvs.append(undistortPath.toStdString());
  //cmd_cmvs.append("/pmvs/\" option-all\"");
  std::string cmd_cmvs("\"");
  cmd_cmvs.append(app_path.parentPath().toString());
  cmd_cmvs.append("\\pmvs2\" \"");
  cmd_cmvs.append(undistortPath.toStdString());
  cmd_cmvs.append("/pmvs/\" option-all");
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
  //createDirectory(std::string(mOutputPath).append("/original"));
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
  std::string bundler_path = mOutputPath + "/bundle.rd.out";
  std::string bundler_path_list = bundler_path + ".list.txt";
  //std::string bundler_path_list_original = bundler_path + ".original.list.txt";

  if (mReconstruction) {

    colmap::Reconstruction &reconstruction = mReconstruction->colmap();

    std::ofstream stream(bundler_path, std::ios::trunc);
    std::ofstream stream_image_list(bundler_path_list, std::ios::trunc);
    //std::ofstream stream_image_list_original(bundler_path_list_original, std::ios::trunc);
    if (stream.is_open() && stream_image_list.is_open() /*&& stream_image_list_original.is_open()*/) {

      int camera_count = reconstruction.Images().size();
      int feature_count = reconstruction.NumPoints3D();

      stream << "# Bundle file v0.3" << std::endl;
      stream << camera_count << " " << feature_count << std::endl;

      const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

      for (auto &camera : reconstruction.Cameras()) {

        //double sensor_width_px = std::max(camera.second.Width(), camera.second.Height());
        float focal = static_cast<float>(camera.second.FocalLength());
        float ppx = static_cast<float>(camera.second.PrincipalPointX());
        float ppy = static_cast<float>(camera.second.PrincipalPointY());
        int model_id = camera.second.ModelId();

        cv::Size imageSize(static_cast<int>(camera.second.Width()),
                           static_cast<int>(camera.second.Height()));
        std::array<std::array<float, 3>, 3> camera_matrix_data = {focal, 0.f, ppx,
                                                                  0.f, focal, ppy,
                                                                  0.f, 0.f, 1.f};
        cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32F, camera_matrix_data.data());
        cv::Mat distCoeffs = cv::Mat::zeros(1, 5, CV_32F);
        std::vector<double> params = camera.second.Params();
        distCoeffs.at<float>(0) = (model_id == 0 ? 0.f : static_cast<float>(params[3]));
        distCoeffs.at<float>(1) = (model_id == 3 || model_id == 50 ? static_cast<float>(params[4]) : 0.0f);
        distCoeffs.at<float>(2) = (model_id == 50 ? static_cast<float>(params[6]) : 0.0f);
        distCoeffs.at<float>(3) = (model_id == 50 ? static_cast<float>(params[7]) : 0.0f);
        distCoeffs.at<float>(4) = (model_id == 50 ? static_cast<float>(params[5]) : 0.0f);

        cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr);
        float new_focal = optCameraMat.at<float>(0, 0);

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
            
            //std::string output_image_original_path = image.Name(); //colmap::StringPrintf("%08d.tif", i);
            //stream_image_list_original << output_image_original_path << std::endl;
            
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
      //stream_image_list_original.close();
    }
  } else 
    msgError("There is not a valid reconstruction");
}

void CmvsPmvsDensifier::undistortImages()
{
  colmap::Reconstruction &reconstruction = mReconstruction->colmap();
  const std::vector<colmap::image_t> &reg_image_ids = reconstruction.RegImageIds();

  for (auto &camera : reconstruction.Cameras()) {

    //double sensor_width_px = std::max(camera.second.Width(), camera.second.Height());
    float focal = static_cast<float>(camera.second.FocalLength());
    float ppx = static_cast<float>(camera.second.PrincipalPointX());
    float ppy = static_cast<float>(camera.second.PrincipalPointY());
    int model_id = camera.second.ModelId();

    cv::Size imageSize(static_cast<int>(camera.second.Width()),
                       static_cast<int>(camera.second.Height()));
    std::array<std::array<float, 3>, 3> camera_matrix_data = {focal, 0.f, ppx,
                                                              0.f, focal, ppy,
                                                              0.f, 0.f, 1.f};
    cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32F, camera_matrix_data.data());
    cv::Mat distCoeffs = cv::Mat::zeros(1, 5, CV_32F);
    std::vector<double> params = camera.second.Params();
    distCoeffs.at<float>(0) = (model_id == 0 ? 0.f : static_cast<float>(params[3]));
    distCoeffs.at<float>(1) = (model_id == 3 || model_id == 50 ? static_cast<float>(params[4]) : 0.0f);
    distCoeffs.at<float>(2) = (model_id == 50 ? static_cast<float>(params[6]) : 0.0f);
    distCoeffs.at<float>(3) = (model_id == 50 ? static_cast<float>(params[7]) : 0.0f);
    distCoeffs.at<float>(4) = (model_id == 50 ? static_cast<float>(params[5]) : 0.0f);

    cv::Mat map1, map2;
    cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr);
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), optCameraMat, imageSize, CV_32FC1, map1, map2);
    
#ifdef HAVE_CUDA
    cv::cuda::GpuMat gMap1(map1);
    cv::cuda::GpuMat gMap2(map2);
#endif
    

    //try {
      
      TL_TODO("undistortImage()")

      for (size_t i = 0; i < reg_image_ids.size(); ++i) {
        colmap::image_t image_id = reg_image_ids[i];
        colmap::Image &image = reconstruction.Image(image_id);
        if (image.CameraId() == camera.second.CameraId()) {

          std::string image_file = image.Name();
          
          cv::Mat img;
          //cv::Mat img_original;

          if (bOpenCvRead) {
            img = cv::imread(image_file, cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);
            //img_original = cv::imread(image_file, cv::IMREAD_ANYDEPTH | cv::IMREAD_IGNORE_ORIENTATION);
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

                //img_original = img;
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
            //if (img_original.empty()) img_original = img;
            cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
          }

          cv::Mat img_undistort;
          cv::Mat img_undistort_original;
#ifdef HAVE_CUDA
          if (bCuda) {
            TL_TODO("comprobar versión driver cuda");
            cv::cuda::GpuMat gImgOut(img);
            img.release();
            cv::cuda::GpuMat gImgUndistort;

            //cv::cuda::GpuMat gImgOutOriginal(img_original);
            //img_original.release();
            //cv::cuda::GpuMat gImgUndistortOriginal;

            //cv::cuda::Stream stream1;
            cv::cuda::remap(gImgOut, gImgUndistort, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar()/*, stream1*/);
            gImgUndistort.download(img_undistort);

            //cv::cuda::Stream stream2;
            //cv::cuda::remap(gImgOutOriginal, gImgUndistortOriginal, gMap1, gMap2, cv::INTER_LINEAR, 0, cv::Scalar(), stream2);
            //gImgUndistortOriginal.download(img_undistort_original);

            //stream1.waitForCompletion();
            //stream2.waitForCompletion();

          } else {
#endif
            cv::remap(img, img_undistort_original, map1, map2, cv::INTER_LINEAR);
            img.release();
            //cv::remap(img_original, img_undistort_original, map1, map2, cv::INTER_LINEAR);
            //img_original.release();

#ifdef HAVE_CUDA
          }
#endif
          msgInfo("Undistort image: %s", image_file.c_str());

          std::string output_image_path = mOutputPath + colmap::StringPrintf("/visualize/%08d.jpg", i);
          cv::imwrite(output_image_path, img_undistort);
          //std::string output_image_original_path = mOutputPath + "/" + image.Name();// colmap::StringPrintf("/original/%08d.tif", i);
          //cv::imwrite(output_image_original_path, img_undistort_original);

          std::string proj_matrix_path = mOutputPath + colmap::StringPrintf("/txt/%08d.txt", i);
          std::ofstream file(proj_matrix_path, std::ios::trunc);
          CHECK(file.is_open()) << proj_matrix_path;

          Eigen::Matrix3d calib_matrix = Eigen::Matrix3d::Identity();
          calib_matrix(0, 0) = optCameraMat.at<float>(0,0);
          calib_matrix(1, 1) = optCameraMat.at<float>(1,1);
          calib_matrix(0, 2) = optCameraMat.at<float>(0,2);
          calib_matrix(1, 2) = optCameraMat.at<float>(1,2);

          const Eigen::Matrix3x4d proj_matrix = calib_matrix * image.ProjectionMatrix();

          file << "CONTOUR" << std::endl;

          WriteMatrix(proj_matrix, &file);
        }
      }
    //} catch (const std::exception &e) {
    //  msgError(e.what());
    //}

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

  file_options << "# Generated by Inspector Image - all images, no clustering." << std::endl;

  file_options << "level " << CmvsPmvsProperties::level() << std::endl;
  file_options << "csize " << CmvsPmvsProperties::cellSize() << std::endl;
  file_options << "threshold " << CmvsPmvsProperties::threshold() << std::endl;
  file_options << "wsize " << CmvsPmvsProperties::windowSize() << std::endl;
  file_options << "minImageNum " << CmvsPmvsProperties::minimunImageNumber() << std::endl;
  file_options << "CPU " << std::thread::hardware_concurrency() << std::endl;
  file_options << "setEdge 0" << std::endl;
  file_options << "useBound 0" << std::endl;
  file_options << "useVisData " << (CmvsPmvsProperties::useVisibilityInformation() ? 1 : 0) << std::endl;
  file_options << "sequence -1" << std::endl;
  file_options << "maxAngle 10" << std::endl;
  file_options << "quad 2.0" << std::endl;

  file_options << "timages " << reconstruction.NumRegImages();
  for (size_t i = 0; i < reconstruction.NumRegImages(); ++i) {
    file_options << " " << i;
  }
  file_options << std::endl;

  file_options << "oimages 0" << std::endl;
}

} // namespace inspector
