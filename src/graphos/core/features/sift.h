#ifndef GRAPHOS_SIFT_DETECTOR_DESCRIPTOR_H
#define GRAPHOS_SIFT_DETECTOR_DESCRIPTOR_H

#include "graphos/graphos_global.h"

#include "graphos/core/features/features.h"

#include <opencv2/features2d.hpp>
#if defined OPENCV_ENABLE_NONFREE && defined HAVE_OPENCV_XFEATURES2D
#include <opencv2/xfeatures2d.hpp>
#endif // HAVE_OPENCV_XFEATURES2D

#include "SiftGPU/SiftGPU.h"
#include <colmap/feature/sift.h>
#include "VLFeat/sift.h"

#include <QString>

namespace graphos
{


/*!
 * \brief SIFT detector/descriptor properties class
 */
class GRAPHOS_EXPORT SiftProperties
  : public Sift
{

public:

  SiftProperties();
  SiftProperties(const SiftProperties &siftProperties);
  ~SiftProperties() override = default;

// Sift interface

public:

  int featuresNumber() const override;
  int octaveLayers() const override;
  bool constrastThresholdAuto() const override;
  double contrastThreshold() const override;
  double edgeThreshold() const override;

  void setFeaturesNumber(int featuresNumber) override;
  void setOctaveLayers(int octaveLayers) override;
  void setContrastThresholdAuto(bool active) override;
  void setContrastThreshold(double contrastThreshold) override;
  void setEdgeThreshold(double edgeThreshold) override;


// Feature interface

public:

  void reset() override;
  QString name() const final;

private:

  int mFeaturesNumber;
  int mOctaveLayers;
  bool mContrastThresholdAuto;
  double mContrastThreshold;
  double mEdgeThreshold;
  //double mSigma;
};


/*----------------------------------------------------------------*/


class SiftDetectorDescriptor
  : public SiftProperties,
    public FeatureExtractor
{

public:

  SiftDetectorDescriptor();
  SiftDetectorDescriptor(const SiftDetectorDescriptor &siftDetectorDescriptor);
  SiftDetectorDescriptor(int featuresNumber,
                         int octaveLayers,
                         double edgeThreshold,
                         double contrastThreshold = 0.);
  ~SiftDetectorDescriptor() override;

private:

  void update();

// FeatureExtractor interface

public:

  void run(const colmap::Bitmap &bitmap,
           colmap::FeatureKeypoints &keyPoints,
           colmap::FeatureDescriptors &descriptors) override;

  void run(const cv::Mat &bitmap,
           colmap::FeatureKeypoints &keyPoints,
           colmap::FeatureDescriptors &descriptors) override;

// Sift interface

public:

  void setFeaturesNumber(int featuresNumber) override;
  void setOctaveLayers(int octaveLayers) override;
  void setContrastThreshold(double contrastThreshold) override;
  void setEdgeThreshold(double edgeThreshold) override;

// Feature interface

public:

  void reset() override;

protected:

  VlSiftFilt *mSiftCpu;
  colmap::SiftExtractionOptions mSiftExtractionOptions;
  std::mutex mMutex;
};


/*----------------------------------------------------------------*/


class SiftCPUDetectorDescriptor
  : public SiftProperties,
  public FeatureExtractor
{

public:

  SiftCPUDetectorDescriptor();
  SiftCPUDetectorDescriptor(const SiftCPUDetectorDescriptor &siftDetectorDescriptor);
  SiftCPUDetectorDescriptor(int featuresNumber,
                         int octaveLayers,
                         double edgeThreshold,
                         double contrastThreshold = 0.);
  ~SiftCPUDetectorDescriptor() override;

private:

  void update();

  // FeatureExtractor interface

public:

  void run(const colmap::Bitmap &bitmap,
           colmap::FeatureKeypoints &keyPoints,
           colmap::FeatureDescriptors &descriptors) override;

  void run(const cv::Mat &bitmap,
           colmap::FeatureKeypoints &keyPoints,
           colmap::FeatureDescriptors &descriptors) override;

  // Sift interface

public:

  void setFeaturesNumber(int featuresNumber) override;
  void setOctaveLayers(int octaveLayers) override;
  void setContrastThreshold(double contrastThreshold) override;
  void setEdgeThreshold(double edgeThreshold) override;

  // Feature interface

public:

  void reset() override;

protected:

#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4))
  cv::Ptr <cv::SIFT>  mSift;
#elif defined OPENCV_ENABLE_NONFREE && defined HAVE_OPENCV_XFEATURES2D
  cv::Ptr<cv::xfeatures2d::SIFT> mSift;
#endif // HAVE_OPENCV_XFEATURES2D
  colmap::SiftExtractionOptions mSiftExtractionOptions;
  std::mutex mMutex;
};


/*----------------------------------------------------------------*/


class SiftCudaDetectorDescriptor
  : public SiftProperties,
    public FeatureExtractor
{

public:

  SiftCudaDetectorDescriptor();
  SiftCudaDetectorDescriptor(const SiftCudaDetectorDescriptor &siftDetectorDescriptor);
  SiftCudaDetectorDescriptor(int featuresNumber,
                             int octaveLayers,
                             double edgeThreshold,
                             double contrastThreshold = 0.);
  ~SiftCudaDetectorDescriptor() override;

private:

  void update();

// FeatureExtractor interface

public:

  void run(const colmap::Bitmap &bitmap,
           colmap::FeatureKeypoints &keyPoints,
           colmap::FeatureDescriptors &descriptors) override;

  void run(const cv::Mat &bitmap,
           colmap::FeatureKeypoints &keyPoints,
           colmap::FeatureDescriptors &descriptors) override;

// Sift interface

public:

  void setFeaturesNumber(int featuresNumber) override;
  void setOctaveLayers(int octaveLayers) override;
  void setContrastThreshold(double contrastThreshold) override;
  void setEdgeThreshold(double edgeThreshold) override;

// Feature interface

public:

  void reset() override;

protected:

  std::unique_ptr<SiftGPU> mSiftGpu;
  colmap::SiftExtractionOptions mSiftExtractionOptions;
  std::mutex mMutex;
};


} // namespace graphos

#endif // GRAPHOS_SIFT_DETECTOR_DESCRIPTOR_H
