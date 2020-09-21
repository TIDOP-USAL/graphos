#include "FeatureExtractorProcess.h"

#include "inspector/core/features/featio.h"
#include "inspector/core/features/sift.h"

#include <tidop/core/messages.h>
#include <tidop/img/imgreader.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>

#ifdef HAVE_CUDA
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>
#endif // HAVE_CUDA

#include <QFileInfo>

namespace inspector
{
	
FeatureExtractorProcess::FeatureExtractorProcess(const Image &image,
                                                 const Camera &camera,
                                                 int maxDimension,
                                                 const QString &featureFile,
                                                 const std::shared_ptr<FeatureExtractor> &featureExtractor)
  : ProcessConcurrent(),
    mImage(image),
    mCamera(camera),
    mMaxDimension(maxDimension),
    mFeatureFile(featureFile),
    mFeatureExtractor(featureExtractor),
    bOpenCvRead(true)
{
}

Image FeatureExtractorProcess::image() const
{
  return mImage;
}

void FeatureExtractorProcess::setImage(const Image &image)
{
  mImage = image;
}

Camera FeatureExtractorProcess::camera() const
{
  return mCamera;
}

void FeatureExtractorProcess::setCamera(const Camera &camera)
{
  mCamera = camera;
}

std::shared_ptr<FeatureExtractor> FeatureExtractorProcess::featureExtractor() const
{
  return mFeatureExtractor;
}

void FeatureExtractorProcess::run()
{
  try {
    tl::Chrono chrono("", false);
    chrono.run();

    QByteArray ba = mImage.path().toLocal8Bit();
    const char *img_file = ba.data();

    if (mFeatureExtractor == nullptr) {
      emit error(0, "Keypoint Detector/descriptor error");
      return;
    }

    msgInfo("Extracting features for image %s", img_file);

    QFileInfo file_info(mImage.path());
    std::string image_name = file_info.fileName().toStdString();

    QStringList list = mFeatureFile.split("@");
    colmap::Database database(list[1].toStdString());

    colmap::FeatureKeypoints featureKeypoints;
    colmap::FeatureDescriptors featureDescriptors;
    colmap::image_t image_id;

    if (!database.ExistsImageWithName(image_name)){

      colmap::Camera camera_colmap;
      colmap::camera_t camera_id = static_cast<colmap::camera_t>(mImage.cameraId());
      if (!database.ExistsCamera(camera_id)){
        QString colmap_camera_type = cameraToColmapType(mCamera);
        int camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
        if (camera_model_id == -1) throw std::runtime_error("Camera model unknow");
        double focal_length = mCamera.focal();
        size_t width = static_cast<size_t>(mCamera.width());
        size_t height = static_cast<size_t>(mCamera.height());
        if (round(1.2 * std::max(width, height)) == round(focal_length)){
          camera_colmap.SetPriorFocalLength(false);
        } else {
          camera_colmap.SetPriorFocalLength(true);
        }
        camera_colmap.InitializeWithId(camera_model_id, focal_length, width, height);
        camera_id = database.WriteCamera(camera_colmap);
        camera_colmap.SetCameraId(camera_id);
      }

      colmap::Image image_colmap;
      image_colmap.SetName(image_name);
      image_colmap.SetCameraId(camera_id);

      image_id = database.WriteImage(image_colmap, false);
    } else {
      colmap::Image image_colmap = database.ReadImageWithName(image_name);
      image_id = image_colmap.ImageId();
    }

    cv::Mat mat;
    double scale = 1.;

    if (bOpenCvRead) {

      mat = cv::imread(mImage.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION | cv::IMREAD_GRAYSCALE);

      if (mat.empty()) {
        bOpenCvRead = false;
      } else {
        double max_dimension;
        if (mat.cols > mat.rows){
          max_dimension = mat.cols;
        } else {
          max_dimension = mat.rows;
        }

        if (mMaxDimension < max_dimension){
          cv::Size size(mat.cols, mat.rows);
          scale = max_dimension / mMaxDimension;
          size.width /= scale;
          size.height /= scale;
#ifdef HAVE_CUDA
          cv::cuda::GpuMat gImgIn(mat);
          cv::cuda::GpuMat gImgResize;
          cv::cuda::resize(gImgIn, gImgResize, size);
          gImgResize.download(mat);
#else
          cv::resize(mat, mat, size);
#endif
        }
      }

    }

    if (!bOpenCvRead) {
      std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(mImage.path().toStdString());
      imageReader->open();
      if (imageReader->isOpen()) {
        int w = imageReader->cols();
        int h = imageReader->rows();
        double max_dimension;
        if (w > h){
          max_dimension = w;
        } else {
          max_dimension = h;
        }

        
        if (mMaxDimension < max_dimension) {
          scale = mMaxDimension / max_dimension;
          mat = imageReader->read(scale, scale);
          scale = 1. / scale;
        } else {
          mat = imageReader->read();
        }
        
        if (imageReader->depth() != 8) {
#ifdef HAVE_CUDA
          cv::cuda::GpuMat gImgIn(mat);
          cv::cuda::GpuMat gImgOut;
          cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U);
          gImgOut.download(mat);
#else
          cv::normalize(mat, mat, 0., 255., cv::NORM_MINMAX, CV_8U);
#endif
        }

        if (mat.channels() >= 3) {
#ifdef HAVE_CUDA
          cv::cuda::GpuMat gImgIn(mat);
          cv::cuda::GpuMat gImgGray;
          cv::cuda::cvtColor(gImgIn, gImgGray, cv::COLOR_BGR2GRAY);
          gImgGray.download(mat);
#else
          cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
#endif
        }



        imageReader->close();
      }
    }

    mFeatureExtractor->run(mat, featureKeypoints, featureDescriptors);

    if (scale > 1) {
      for (auto & featureKeypoint : featureKeypoints){
        featureKeypoint.Rescale(scale, scale);
      }
    }

    // Escritura en la base de datos
//    if (database.ExistsKeypoints(image_id)){
//      database.
//    } else {
      database.WriteKeypoints(image_id, featureKeypoints);
      database.WriteDescriptors(image_id, featureDescriptors);
//    }
    database.Close();

    emit featuresExtracted(mImage.name(), mFeatureFile);

    uint64_t time = chrono.stop();
    msgInfo("%i features extracted [Time: %f seconds]", featureKeypoints.size(), time/1000.);

  } catch (std::exception &e) {
    //emit error(0, "Keypoint Detector/descriptor error");
    msgError(e.what());
  } catch (...) {
    //emit error(0, "Keypoint Detector/descriptor error");
    msgError("Keypoint Detector/descriptor unknown exception");
  }
  emit statusChangedNext();
}

} // inspector
