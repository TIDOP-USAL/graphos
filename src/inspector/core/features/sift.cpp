#include "sift.h"

#include <tidop/core/messages.h>

#include <colmap/util/opengl_utils.h>
#include <colmap/feature/utils.h>
#include "VLFeat/sift.h"

#include <opencv2/imgcodecs.hpp>

namespace inspector
{


SiftProperties::SiftProperties()
  : mFeaturesNumber(5000),
    mOctaveLayers(3),
    mContrastThresholdAuto(true),
    mContrastThreshold(0.0067),
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
  mContrastThreshold = 0.02 / 3.;
  mEdgeThreshold = 10.;
  mSigma = 1.6;
}

QString SiftProperties::name() const
{
  return QString("SIFT");
}


/*----------------------------------------------------------------*/


//SiftDetectorDescriptor::SiftDetectorDescriptor()
//{
//  update();
//}
//
//SiftDetectorDescriptor::SiftDetectorDescriptor(const SiftDetectorDescriptor &siftDetectorDescriptor)
//  : SiftProperties(siftDetectorDescriptor),
//    FeatureExtractor(siftDetectorDescriptor)
//{
//  update();
//}
//
//SiftDetectorDescriptor::SiftDetectorDescriptor(int featuresNumber,
//                                               int octaveLayers,
//                                               double contrastThreshold,
//                                               double edgeThreshold,
//                                               double sigma)
//{
//  SiftProperties::setFeaturesNumber(featuresNumber);
//  SiftProperties::setOctaveLayers(octaveLayers);
//  if (contrastThreshold > 0.) {
//    SiftProperties::setContrastThresholdAuto(false);
//    SiftProperties::setContrastThreshold(contrastThreshold);
//  }
//  SiftProperties::setEdgeThreshold(edgeThreshold);
//  SiftProperties::setSigma(sigma);
//  update();
//}
//
//void SiftDetectorDescriptor::update()
//{
//}
//
//void SiftDetectorDescriptor::run(const colmap::Bitmap &bitmap,
//                                 colmap::FeatureKeypoints &keyPoints,
//                                 colmap::FeatureDescriptors &descriptors)
//{
//  try {
//    bool err = ExtractSiftFeaturesCPU(mSiftExtractionOptions, bitmap, &keyPoints, &descriptors);
//    if (err == false) throw "ExtractSiftFeaturesCPU fail";
//  } catch (std::exception &e) {
//    msgError("SIFT Detector exception");
//    throw;
//  } catch (...) {
//    msgError("SIFT Detector exception");
//    throw;
//  }
//}
//
//void SiftDetectorDescriptor::run(const cv::Mat &bitmap,
//                                 colmap::FeatureKeypoints &keyPoints,
//                                 colmap::FeatureDescriptors &descriptors)
//{
//  try {
//
//    //std::unique_ptr<VlSiftFilt, void (*)(VlSiftFilt*)> sift(
//    //  vl_sift_new(bitmap.cols, bitmap.rows, mSiftExtractionOptions.num_octaves,
//    //              mSiftExtractionOptions.octave_resolution, mSiftExtractionOptions.first_octave),
//    //              &vl_sift_delete);
//    //if (!sift) throw std::runtime_error("ExtractSiftFeaturesCPU fail");
//
//
//
//  } catch (std::exception &e) {
//    msgError("SIFT Detector exception");
//    throw;
//  }
//}
//
//
//void SiftDetectorDescriptor::setFeaturesNumber(int featuresNumber)
//{
//  SiftProperties::setFeaturesNumber(featuresNumber);
//  update();
//}
//
//void SiftDetectorDescriptor::setOctaveLayers(int octaveLayers)
//{
//  SiftProperties::setOctaveLayers(octaveLayers);
//  update();
//}
//
//void SiftDetectorDescriptor::setContrastThreshold(double contrastThreshold)
//{
//  SiftProperties::setContrastThreshold(contrastThreshold);
//  update();
//}
//
//void SiftDetectorDescriptor::setEdgeThreshold(double edgeThreshold)
//{
//  SiftProperties::setEdgeThreshold(edgeThreshold);
//  update();
//}
//
//void SiftDetectorDescriptor::setSigma(double sigma)
//{
//  SiftProperties::setSigma(sigma);
//  update();
//}
//
//void SiftDetectorDescriptor::reset()
//{
//  SiftProperties::reset();
//  update();
//}


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
                                                       double edgeThreshold,
                                                       double sigma,
                                                       double contrastThreshold)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  SiftProperties::setOctaveLayers(octaveLayers);
  if (contrastThreshold > 0.) {
    SiftProperties::setContrastThresholdAuto(false);
    SiftProperties::setContrastThreshold(contrastThreshold);
  }
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
    if (err == false) throw std::runtime_error("ExtractSiftFeaturesGPU fail");
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
    if (err != 1) throw std::runtime_error("ExtractSiftFeaturesGPU fail");

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
      throw std::runtime_error("Description normalization type not supported");
    }

    descriptors = colmap::FeatureDescriptorsToUnsignedByte(descriptors_float);

  } catch (std::exception &e) {
    msgError("SIFT Detector exception");
    throw;
  }
}

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







