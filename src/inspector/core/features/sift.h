#ifndef INSPECTOR_SIFT_DETECTOR_DESCRIPTOR_H
#define INSPECTOR_SIFT_DETECTOR_DESCRIPTOR_H

#include "inspector/inspector_global.h"

#include "inspector/core/features/features.h"

#include <opencv2/xfeatures2d.hpp>

#include "SiftGPU/SiftGPU.h"
#include <colmap/feature/sift.h>

#include <QString>


namespace inspector
{


/*!
 * \brief SIFT detector/descriptor properties class
 */
class INSPECTOR_EXPORT SiftProperties
  : public Sift
{

public:

  SiftProperties();
  SiftProperties(const SiftProperties &siftProperties);
  ~SiftProperties() override = default;

// ISift interface

public:

  int featuresNumber() const override;
  int octaveLayers() const override;
  double contrastThreshold() const override;
  double edgeThreshold() const override;
  double sigma() const override;
  void setFeaturesNumber(int featuresNumber) override;
  void setOctaveLayers(int octaveLayers) override;
  void setContrastThreshold(double contrastThreshold) override;
  void setEdgeThreshold(double edgeThreshold) override;
  void setSigma(double sigma) override;

// Feature interface

public:

  void reset() override;
  QString name() const final;

private:

  int mFeaturesNumber;
  int mOctaveLayers;
  double mContrastThreshold;
  double mEdgeThreshold;
  double mSigma;
};


/*----------------------------------------------------------------*/

//#ifdef OPENCV_ENABLE_NONFREE

//class INSPECTOR_EXPORT SiftDetectorDescriptor
//  : public SiftProperties,
//    public FeatureExtractor
//{

//public:

//  SiftDetectorDescriptor();
//  SiftDetectorDescriptor(const SiftDetectorDescriptor &siftDetectorDescriptor);
//  SiftDetectorDescriptor(int featuresNumber,
//                         int octaveLayers,
//                         double contrastThreshold,
//                         double edgeThreshold,
//                         double sigma);
//  ~SiftDetectorDescriptor() override = default;

//private:

//  void update();

//// FeatureExtractor interface

//public:

//  void run(const colmap::Bitmap &bitmap,
//           colmap::FeatureKeypoints &keyPoints,
//           colmap::FeatureDescriptors &descriptors) override;

//// ISift interface

//public:

//  void setFeaturesNumber(int featuresNumber) override;
//  void setOctaveLayers(int octaveLayers) override;
//  void setContrastThreshold(double contrastThreshold) override;
//  void setEdgeThreshold(double edgeThreshold) override;
//  void setSigma(double sigma) override;

//// Feature interface

//public:

//  void reset() override;

//protected:

//  cv::Ptr<cv::xfeatures2d::SIFT> mSift;
//};

//#endif

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
                             double contrastThreshold,
                             double edgeThreshold,
                             double sigma);
  ~SiftCudaDetectorDescriptor() override = default;

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
  void setSigma(double sigma) override;

// Feature interface

public:

  void reset() override;

protected:

  std::unique_ptr<SiftGPU> mSiftGpu;
  colmap::SiftExtractionOptions mSiftExtractionOptions;
};


} // namespace inspector

#endif // INSPECTOR_SIFT_DETECTOR_DESCRIPTOR_H
