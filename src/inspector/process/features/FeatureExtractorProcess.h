#ifndef INSPECTOR_FEATURE_EXTRACTOR_PROCESS_H
#define INSPECTOR_FEATURE_EXTRACTOR_PROCESS_H

#include "inspector/process/ProcessConcurrent.h"
#include "inspector/core/features/features.h"
#include "inspector/core/camera.h"
#include "inspector/core/image.h"

namespace inspector
{

class INSPECTOR_EXPORT FeatureExtractorProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  FeatureExtractorProcess(const Image &image,
                          const Camera &camera,
                          int maxDimension,
                          const QString &featureFile,
                          const std::shared_ptr<FeatureExtractor> &featureExtractor);

  Image image() const;
  void setImage(const Image &image);
  Camera camera() const;
  void setCamera(const Camera &camera);
  void setMaxDimension(int maxDimension);

  std::shared_ptr<FeatureExtractor> featureExtractor() const;

signals:

  void featuresExtracted(QString, QString);

// ProcessConcurrent interface

protected:

  void run() override;

protected:

  Image mImage;
  Camera mCamera;
  int mMaxDimension;
  QString mFeatureFile;
  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
};

} // inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_PROCESS_H
