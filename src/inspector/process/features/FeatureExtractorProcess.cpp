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
  try {
    tl::Chrono chrono;
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

    cv::Mat mat = cv::imread(mImage.path().toStdString(), cv::IMREAD_IGNORE_ORIENTATION |cv::IMREAD_GRAYSCALE);

    double max_dimension;
    if (mat.cols > mat.rows){
      max_dimension = mat.cols;
    } else {
      max_dimension = mat.rows;
    }

    double scale = 1;
    if (mMaxDimension < max_dimension){
      cv::Size size(mat.cols, mat.rows);
      scale = max_dimension / mMaxDimension;
      size.width /= scale;
      size.height /= scale;
      cv::resize(mat, mat, size);
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
    emit statusChangedNext();

    uint64_t time = chrono.stop();
    msgInfo("%i features extracted [Time: %f seconds]", featureKeypoints.size(), time/1000.);

  } catch (std::exception &e) {
    emit error(0, "Keypoint Detector/descriptor error");
    msgError(e.what());
  }
}

} // inspector
