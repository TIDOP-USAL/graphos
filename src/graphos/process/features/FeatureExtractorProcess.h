#ifndef GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H
#define GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/features/features.h"
#include "graphos/core/camera.h"
#include "graphos/core/image.h"

namespace graphos
{

class GRAPHOS_EXPORT FeatureExtractorProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  FeatureExtractorProcess(const std::vector<Image> &images,
                          const std::map<int, Camera> &cameras,
                          const QString &database,
                          int maxImageSize,
                          bool cuda,
                          const std::shared_ptr<FeatureExtractor> &featureExtractor);

  //Image image() const;
  //void setImage(const Image &image);
  //Camera camera() const;
  //void setCamera(const Camera &camera);
  //void setMaxDimension(int maxDimension);
  //bool useGPU() const;
  //void setUseGPU(bool useGPU);
  //std::shared_ptr<FeatureExtractor> featureExtractor() const;

signals:

  void featuresExtracted(QString, QString);

  // ProcessConcurrent interface

protected:

  void run() override;

protected:

  std::vector<Image> mImages;
  std::map<int, Camera> mCameras;
  QString mDatabase;
  int mMaxImageSize;
  bool bUseCuda;
  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
  bool bOpenCvRead;
};


//class GRAPHOS_EXPORT FeatureExtractorProcess
//  : public ProcessConcurrent
//{
//
//  Q_OBJECT
//
//public:
//
//  FeatureExtractorProcess(const Image &image,
//                          const Camera &camera,
//                          int maxDimension,
//                          const QString &featureFile,
//                          bool cuda,
//                          const std::shared_ptr<FeatureExtractor> &featureExtractor);
//
//  Image image() const;
//  void setImage(const Image &image);
//  Camera camera() const;
//  void setCamera(const Camera &camera);
//  void setMaxDimension(int maxDimension);
//  bool useGPU() const;
//  void setUseGPU(bool useGPU);
//  std::shared_ptr<FeatureExtractor> featureExtractor() const;
//
//signals:
//
//  void featuresExtracted(QString, QString);
//
//// ProcessConcurrent interface
//
//protected:
//
//  void run() override;
//
//protected:
//
//  Image mImage;
//  Camera mCamera;
//  int mMaxDimension;
//  QString mFeatureFile;
//  bool bUseCuda;
//  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
//  bool bOpenCvRead;
//};

} // graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H
