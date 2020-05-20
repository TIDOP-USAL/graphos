#include "FeatureExtractorProcess.h"

#include "inspector/core/features/featio.h"
#include "inspector/core/features/sift.h"

#include <tidop/core/messages.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>

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
    mFeatureExtractor(featureExtractor)
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
  tl::Chrono chrono;
  chrono.run();

  QByteArray ba = mImage.path().toLocal8Bit();
  const char *img_file = ba.data();
//  cv::Mat img = cv::imread(img_file, cv::IMREAD_IGNORE_ORIENTATION);
//  if (img.empty()) {
//    emit error(0, QString("Could not load image :").append(mImage));
//    return;
//  }

  colmap::Bitmap bitmap;
  msgInfo("Read image: %s", img_file);
  if (!bitmap.Read(img_file, false)) {
    msgError("Failed to read image file");
    return;
  }



  if (mFeatureExtractor == nullptr) {
    emit error(0, "Keypoint Detector/descriptor error");
    return;
  }

  msgInfo("Extracting features for image %s", img_file);

  colmap::Database database(mFeatureFile.toStdString());
  colmap::Camera camera_colmap;

  colmap::camera_t camera_id = static_cast<colmap::camera_t>(mImage.cameraId());
  if (!database.ExistsCamera(camera_id)){
    int camera_model_id = colmap::CameraModelNameToId(mCamera.type().toStdString());
    double focal_length = mCamera.focal();
    size_t width = static_cast<size_t>(mCamera.width());
    size_t height = static_cast<size_t>(mCamera.height());
    if (round(1.2 * std::max(width, height)) == round(focal_length)){
      camera_colmap.SetPriorFocalLength(false);
    } else {
      camera_colmap.SetPriorFocalLength(true);
    }
    camera_colmap.InitializeWithId(camera_model_id, focal_length, width, height);
    camera_colmap.SetCameraId(database.WriteCamera(camera_colmap));
  }

  QFileInfo file_info(mImage.path());
  std::string image_name = file_info.fileName().toStdString();

  colmap::Image image_colmap;
  image_colmap.SetName(image_name);
  image_colmap.SetCameraId(camera_id);

  colmap::image_t image_id = database.WriteImage(image_colmap, false);


  cv::Mat mat = cv::imread(mImage.path().toStdString(), cv::IMREAD_GRAYSCALE);

  double max_dimension;
  if (mat.cols > mat.rows){
    max_dimension = mat.cols;
  } else {
    max_dimension = mat.rows;
  }

  double scale = 1;
  if (mMaxDimension < max_dimension){
    cv::Size size(mat.cols, mat.rows);
    scale = mMaxDimension / max_dimension;
    size.width *= scale;
    size.height *= scale;
    cv::resize(mat, mat, size);
  }

  colmap::FeatureKeypoints featureKeypoints;
  colmap::FeatureDescriptors featureDescriptors;
  mFeatureExtractor->run(mat, featureKeypoints, featureDescriptors);

  if (scale < 1) {
    for (auto & featureKeypoint : featureKeypoints){
      featureKeypoint.Rescale(scale, scale);
    }
  }

  // Escritura en la base de datos

  database.WriteKeypoints(image_id, featureKeypoints);
  database.WriteDescriptors(image_id, featureDescriptors);


//  ba = mFeatures.toLocal8Bit();
//  const char *cfeat = ba.data();
//  msgInfo("Write features at: %s", cfeat);
//  emit featuresExtracted(mFeatures);
  emit statusChangedNext();
}

} // inspector
