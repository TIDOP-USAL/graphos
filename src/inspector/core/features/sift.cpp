#include "sift.h"

#include <tidop/core/messages.h>

#include <colmap/util/opengl_utils.h>
#include <colmap/feature/utils.h>

#include <opencv2/imgcodecs.hpp>

namespace inspector
{


SiftProperties::SiftProperties()
  : mFeaturesNumber(5000),
    mOctaveLayers(3),
    mContrastThresholdAuto(true),
    mContrastThreshold(0.04),
    mEdgeThreshold(10.),
    mSigma(1.6)
{
}

SiftProperties::SiftProperties(const SiftProperties &siftProperties)
  : Sift(siftProperties),
    mFeaturesNumber(siftProperties.mFeaturesNumber),
    mOctaveLayers(siftProperties.mOctaveLayers),
    mContrastThresholdAuto(siftProperties.mContrastThresholdAuto),
    mContrastThreshold(siftProperties.mContrastThreshold),
    mEdgeThreshold(siftProperties.mEdgeThreshold),
    mSigma(siftProperties.mSigma)
{
}

int SiftProperties::featuresNumber() const
{
  return mFeaturesNumber;
}

int SiftProperties::octaveLayers() const
{
  return mOctaveLayers;
}

bool SiftProperties::constrastThresholdAuto() const
{
  return mContrastThresholdAuto;
}

double SiftProperties::contrastThreshold() const
{
  return mContrastThresholdAuto ? 0.02 / mOctaveLayers : mContrastThreshold;
}

double SiftProperties::edgeThreshold() const
{
  return mEdgeThreshold;
}

double SiftProperties::sigma() const
{
  return mSigma;
}

void SiftProperties::setFeaturesNumber(int featuresNumber)
{
  mFeaturesNumber = featuresNumber;
}

void SiftProperties::setOctaveLayers(int octaveLayers)
{
  mOctaveLayers = octaveLayers;
}

void SiftProperties::setContrastThresholdAuto(bool active)
{
  mContrastThresholdAuto = active;
}

void SiftProperties::setContrastThreshold(double contrastThreshold)
{
  mContrastThreshold = contrastThreshold;
}

void SiftProperties::setEdgeThreshold(double edgeThreshold)
{
  mEdgeThreshold = edgeThreshold;
}

void SiftProperties::setSigma(double sigma)
{
  mSigma = sigma;
}

void SiftProperties::reset()
{
  mFeaturesNumber = 5000;
  mOctaveLayers = 3;
  mContrastThresholdAuto = true;
  mContrastThreshold = 0.04;
  mEdgeThreshold = 10.;
  mSigma = 1.6;
}

QString SiftProperties::name() const
{
  return QString("SIFT");
}


/*----------------------------------------------------------------*/

//#ifdef OPENCV_ENABLE_NONFREE

//SiftDetectorDescriptor::SiftDetectorDescriptor()
//{
//  update();
//}

//SiftDetectorDescriptor::SiftDetectorDescriptor(const SiftDetectorDescriptor &siftDetectorDescriptor)
//  : SiftProperties(siftDetectorDescriptor),
//    FeatureExtractor(siftDetectorDescriptor)
//{
//  update();
//}

//SiftDetectorDescriptor::SiftDetectorDescriptor(int featuresNumber,
//                                               int octaveLayers,
//                                               double contrastThreshold,
//                                               double edgeThreshold,
//                                               double sigma)
//{
//  SiftProperties::setFeaturesNumber(featuresNumber);
//  SiftProperties::setOctaveLayers(octaveLayers);
//  SiftProperties::setContrastThreshold(contrastThreshold);
//  SiftProperties::setEdgeThreshold(edgeThreshold);
//  SiftProperties::setSigma(sigma);
//  update();
//}

//void SiftDetectorDescriptor::update()
//{
//  mSift = cv::xfeatures2d::SIFT::create(SiftProperties::featuresNumber(),
//                                        SiftProperties::octaveLayers(),
//                                        SiftProperties::contrastThreshold(),
//                                        SiftProperties::edgeThreshold(),
//                                        SiftProperties::sigma());
//}

//void SiftDetectorDescriptor::run(const cv::Mat &img,
//                                 colmap::FeatureKeypoints &keyPoints,
//                                 colmap::FeatureDescriptors &descriptors)
//{
//  TL_TODO("La escala se tiene que calcular en otra parte")
//  float scale = 1;
//  try {
//    std::vector<cv::KeyPoint> _keyPoints;
//    cv::Mat _descriptors;
//    mSift->detectAndCompute(img, cv::noArray(), _keyPoints, _descriptors);

//    size_t kp_size = _keyPoints.size();
//    keyPoints.resize(kp_size);
//    for (size_t j = 0; j < kp_size; j++) {
//      keyPoints[j] = colmap::FeatureKeypoint((_keyPoints[j].pt.x + 0.5f) / scale,
//                                             (_keyPoints[j].pt.y + 0.5f) / scale ,
//                                              _keyPoints[j].size / scale,
//                                              _keyPoints[j].angle * static_cast<float>(TL_DEG_TO_RAD));
//    }
//  } catch (cv::Exception &e) {
//    msgError("SIFT Detector exception");
//    throw ;
//  }
//}

////bool SiftDetectorDescriptor::detect(const cv::Mat &img,
////                                    std::vector<cv::KeyPoint> &keyPoints,
////                                    cv::InputArray &mask)
////{
////  try {
////    mSift->detect(img, keyPoints, mask);
////  } catch (cv::Exception &e) {
////    msgError("SIFT Detector error: %s", e.what());
////    return true;
////  }

////  return false;
////}

////bool SiftDetectorDescriptor::extract(const cv::Mat &img,
////                                     std::vector<cv::KeyPoint> &keyPoints,
////                                     cv::Mat &descriptors)
////{
////  try {
////    mSift->compute(img, keyPoints, descriptors);
////  } catch (cv::Exception &e) {
////    msgError("SIFT Descriptor error: %s", e.what());
////    return true;
////  }

////  return false;
////}

//void SiftDetectorDescriptor::setFeaturesNumber(int featuresNumber)
//{
//  SiftProperties::setFeaturesNumber(featuresNumber);
//  update();
//}

//void SiftDetectorDescriptor::setOctaveLayers(int octaveLayers)
//{
//  SiftProperties::setOctaveLayers(octaveLayers);
//  update();
//}

//void SiftDetectorDescriptor::setContrastThreshold(double contrastThreshold)
//{
//  SiftProperties::setContrastThreshold(contrastThreshold);
//  update();
//}

//void SiftDetectorDescriptor::setEdgeThreshold(double edgeThreshold)
//{
//  SiftProperties::setEdgeThreshold(edgeThreshold);
//  update();
//}

//void SiftDetectorDescriptor::setSigma(double sigma)
//{
//  SiftProperties::setSigma(sigma);
//  update();
//}

//void SiftDetectorDescriptor::reset()
//{
//  SiftProperties::reset();
//  update();
//}

//#endif





SiftCudaDetectorDescriptor::SiftCudaDetectorDescriptor()
{
  update();
}

SiftCudaDetectorDescriptor::SiftCudaDetectorDescriptor(const SiftCudaDetectorDescriptor &siftDetectorDescriptor)
  : SiftProperties(siftDetectorDescriptor),
    FeatureExtractor(siftDetectorDescriptor)
{
  update();
}

SiftCudaDetectorDescriptor::SiftCudaDetectorDescriptor(int featuresNumber,
                                                       int octaveLayers,
                                                       double contrastThreshold,
                                                       double edgeThreshold,
                                                       double sigma)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  SiftProperties::setOctaveLayers(octaveLayers);
  SiftProperties::setContrastThresholdAuto(false);
  SiftProperties::setContrastThreshold(contrastThreshold);
  SiftProperties::setEdgeThreshold(edgeThreshold);
  SiftProperties::setSigma(sigma);
  update();
}

SiftCudaDetectorDescriptor::~SiftCudaDetectorDescriptor()
{
  //mSiftGpu.reset();
}

void SiftCudaDetectorDescriptor::update()
{
  mSiftGpu.reset(new SiftGPU);
  mSiftExtractionOptions.max_num_features = SiftProperties::featuresNumber();
  mSiftExtractionOptions.octave_resolution = SiftProperties::octaveLayers();
  mSiftExtractionOptions.edge_threshold = SiftProperties::edgeThreshold();
  mSiftExtractionOptions.peak_threshold = SiftProperties::contrastThreshold();

  if (!CreateSiftGPUExtractor(mSiftExtractionOptions, mSiftGpu.get())) {
    return;
  }
}

void SiftCudaDetectorDescriptor::run(const colmap::Bitmap &bitmap,
                                     colmap::FeatureKeypoints &keyPoints,
                                     colmap::FeatureDescriptors &descriptors)
{
  try {
    bool err = ExtractSiftFeaturesGPU(mSiftExtractionOptions, bitmap, mSiftGpu.get(), &keyPoints, &descriptors);
    if (err == false) throw "ExtractSiftFeaturesGPU fail";
  } catch (std::exception &e) {
    msgError("SIFT Detector exception");
    throw;
  } catch (...) {
    msgError("SIFT Detector exception");
    throw;
  }
}

void SiftCudaDetectorDescriptor::run(const cv::Mat &bitmap,
                                     colmap::FeatureKeypoints &keyPoints,
                                     colmap::FeatureDescriptors &descriptors)
{
  try {
    int err = mSiftGpu->RunSIFT(bitmap.cols, bitmap.rows, bitmap.data, GL_LUMINANCE, GL_UNSIGNED_BYTE);
    if (err != 1) throw "ExtractSiftFeaturesGPU fail";

    int feature_number = mSiftGpu->GetFeatureNum();
    std::vector<SiftKeypoint> keypoints_data(feature_number);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> descriptors_float(feature_number, 128);

    mSiftGpu->GetFeatureVector(keypoints_data.data(), descriptors_float.data());

    //size_t max_features = std::min(feature_number, SiftProperties::featuresNumber());
    size_t max_features = feature_number;
    keyPoints.resize(max_features);
    for (size_t i = 0; i < max_features; i++){
      keyPoints[i] = colmap::FeatureKeypoint(keypoints_data[i].x, keypoints_data[i].y,
                                             keypoints_data[i].s, keypoints_data[i].o);
    }

    if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L2){
      descriptors_float = colmap::L2NormalizeFeatureDescriptors(descriptors_float);
    } else if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L1_ROOT){
      descriptors_float = colmap::L1RootNormalizeFeatureDescriptors(descriptors_float);
    } else {
      throw "Description normalization type not supported";
    }

    descriptors = colmap::FeatureDescriptorsToUnsignedByte(descriptors_float);

  } catch (std::exception &e) {
    msgError("SIFT Detector exception");
    throw;
  }
}

//bool SiftCudaDetectorDescriptor::extract(const cv::Mat &img, std::vector<cv::KeyPoint> &keyPoints, cv::Mat &descriptors)
//{
//}

//bool SiftCudaDetectorDescriptor::detect(const cv::Mat &img, std::vector<cv::KeyPoint> &keyPoints, cv::InputArray &mask)
//{
//}

void SiftCudaDetectorDescriptor::setFeaturesNumber(int featuresNumber)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  update();
}

void SiftCudaDetectorDescriptor::setOctaveLayers(int octaveLayers)
{
  SiftProperties::setOctaveLayers(octaveLayers);
  update();
}

void SiftCudaDetectorDescriptor::setContrastThreshold(double contrastThreshold)
{
  SiftProperties::setContrastThreshold(contrastThreshold);
  update();
}

void SiftCudaDetectorDescriptor::setEdgeThreshold(double edgeThreshold)
{
  SiftProperties::setEdgeThreshold(edgeThreshold);
  update();
}

void SiftCudaDetectorDescriptor::setSigma(double sigma)
{
  SiftProperties::setSigma(sigma);
  update();
}

void SiftCudaDetectorDescriptor::reset()
{
  mSiftGpu.reset(new SiftGPU);
}



} // namespace inspector







