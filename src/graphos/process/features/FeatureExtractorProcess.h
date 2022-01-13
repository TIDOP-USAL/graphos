#ifndef GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H
#define GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H

#include <tidop/geospatial/camera.h>

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/features/features.h"
#include "graphos/core/image.h"

namespace graphos
{

class GRAPHOS_EXPORT FeatureExtractorProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  FeatureExtractorProcess(const std::vector<Image> &images,
                          const std::map<int, tl::Camera> &cameras,
                          const QString &database,
                          int maxImageSize,
                          bool cuda,
                          const std::shared_ptr<FeatureExtractor> &featureExtractor);

signals:

  void featuresExtracted(QString, QString);

  // ProcessConcurrent interface

protected:

  void run() override;

protected:

  std::vector<Image> mImages;
  std::map<int, tl::Camera> mCameras;
  QString mDatabase;
  int mMaxImageSize;
  bool bUseCuda;
  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
  //bool bOpenCvRead;
};


} // graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H
