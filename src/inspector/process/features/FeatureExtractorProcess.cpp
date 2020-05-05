#include "FeatureExtractorProcess.h"

#include <tidop/core/messages.h>

#include "inspector/core/features/featio.h"

namespace inspector
{
	
FeatureExtractorProcess::FeatureExtractorProcess(const QString &image,
                                                 const QString &features,
                                                 double imageScale,
                                                 const std::shared_ptr<FeatureExtractor> &featureExtractor)
  : ProcessConcurrent(),
    mImage(image),
    mFeatures(features),
    mImageScale(imageScale),
    mFeatureExtractor(featureExtractor)
{
}

QString FeatureExtractorProcess::image() const
{
  return mImage;
}

void FeatureExtractorProcess::setImage(const QString &image)
{
  mImage = image;
}

QString FeatureExtractorProcess::features() const
{
  return mFeatures;
}

void FeatureExtractorProcess::setFeatures(const QString &features)
{
  mFeatures = features;
}

std::shared_ptr<FeatureExtractor> FeatureExtractorProcess::featureExtractor() const
{
  return mFeatureExtractor;
}

void FeatureExtractorProcess::run()
{
/*   QByteArray ba = mImage.toLocal8Bit();
  const char *img_file = ba.data();
  cv::Mat img = cv::imread(img_file, cv::IMREAD_IGNORE_ORIENTATION);

  if (img.empty()) {
    emit error(0, QString("Could not load image :").append(mImage));
    return;
  }

  if (mKeypointDetector == nullptr) {
    emit error(0, "Keypoint Detector error");
    return;
  }

  msgInfo("Searching Keypoints for image %s", img_file);

  tl::Chrono chrono;
  chrono.run();

  std::vector<cv::KeyPoint> key_points;
  if (bool _error = mKeypointDetector->detect(img, key_points)){
    emit error(0, "Keypoint Detector error");
    return;
  }

  uint64_t time = chrono.stop();
  msgInfo("%i Keypoints detected in image %s [Time: %f seconds]", key_points.size(), img_file, time/1000.);

  /// Filtrado de puntos
  for(auto &filter : mKeyPointsFiltersProcess){
    filter->filter(key_points, key_points);
  }

  if (mDescriptorExtractor == nullptr) {
    emit error(0, "Descriptor Extractor Error");
    return;
  }

  msgInfo("Computing keypoints descriptors for image %s", img_file);

  chrono.reset();
  chrono.run();

  cv::Mat descriptors;
  if (bool _error = mDescriptorExtractor->extract(img, key_points, descriptors)){
    emit error(0, "Descriptor Extractor Error");
    return;
  }

  time = chrono.stop();
  msgInfo("Descriptors computed for image %s [Time: %f seconds]", img_file, time/1000.);

  for (size_t i = 0; i < key_points.size(); i++){
    key_points[i].pt *= mImageScale;
    key_points[i].size *= static_cast<float>(mImageScale);
  }

  std::unique_ptr<FeaturesWriter> writer = FeaturesWriterFactory::createWriter(mFeatures);
  writer->setKeyPoints(key_points);
  writer->setDescriptors(descriptors);
  writer->write();

  ba = mFeatures.toLocal8Bit();
  const char *cfeat = ba.data();
  msgInfo("Write features at: %s", cfeat);
  emit featuresExtracted(mFeatures);
  emit statusChangedNext(); */
}

} // inspector
