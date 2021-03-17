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


SiftDetectorDescriptor::SiftDetectorDescriptor()
  : mSiftCpu(nullptr)
{
  update();
}

SiftDetectorDescriptor::SiftDetectorDescriptor(const SiftDetectorDescriptor &siftDetectorDescriptor)
  : SiftProperties(siftDetectorDescriptor),
    FeatureExtractor(siftDetectorDescriptor),
    mSiftCpu(nullptr)
{
  update();
}

SiftDetectorDescriptor::SiftDetectorDescriptor(int featuresNumber,
                                               int octaveLayers,
                                               double contrastThreshold,
                                               double edgeThreshold,
                                               double sigma)
  : mSiftCpu(nullptr)
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

SiftDetectorDescriptor::~SiftDetectorDescriptor()
{
  if (mSiftCpu) {
    vl_sift_delete(mSiftCpu);
    mSiftCpu = nullptr;
  }
}

void SiftDetectorDescriptor::update()
{
}

void SiftDetectorDescriptor::run(const colmap::Bitmap &bitmap,
                                 colmap::FeatureKeypoints &keyPoints,
                                 colmap::FeatureDescriptors &descriptors)
{
  try {
    bool err = ExtractSiftFeaturesCPU(mSiftExtractionOptions, bitmap, &keyPoints, &descriptors);
    if (err == false) throw "ExtractSiftFeaturesCPU fail";
  } catch (std::exception &e) {
    msgError("SIFT Detector exception");
    throw;
  } catch (...) {
    msgError("SIFT Detector exception");
    throw;
  }
}

void SiftDetectorDescriptor::run(const cv::Mat &bitmap,
                                 colmap::FeatureKeypoints &keyPoints,
                                 colmap::FeatureDescriptors &descriptors)
{
  try {

    /// Copiado mas o menos de Colmap

    mSiftCpu = vl_sift_new(bitmap.cols,
                           bitmap.rows,
                           mSiftExtractionOptions.num_octaves,
                           mSiftExtractionOptions.octave_resolution,
                           mSiftExtractionOptions.first_octave);
    
    if (!mSiftCpu) throw std::runtime_error("ExtractSiftFeaturesCPU fail");

    //vl_sift_set_peak_thresh(mSiftCpu, mSiftExtractionOptions.peak_threshold);
    //vl_sift_set_edge_thresh(mSiftCpu, mSiftExtractionOptions.edge_threshold);

    //// Iterate through octaves.
    //std::vector<size_t> level_num_features;
    //std::vector<colmap::FeatureKeypoints> level_keypoints;
    //std::vector<colmap::FeatureDescriptors> level_descriptors;
    //bool first_octave = true;
    //while (true) {
    //  if (first_octave) {
    //    const std::vector<uint8_t> data_uint8 = bitmap.ConvertToRowMajorArray();
    //    std::vector<float> data_float(data_uint8.size());
    //    for (size_t i = 0; i < data_uint8.size(); ++i) {
    //      data_float[i] = static_cast<float>(data_uint8[i]) / 255.0f;
    //    }
    //    if (vl_sift_process_first_octave(mSiftCpu, data_float.data())) {
    //      break;
    //    }
    //    first_octave = false;
    //  } else {
    //    if (vl_sift_process_next_octave(mSiftCpu)) {
    //      break;
    //    }
    //  }

    //  // Detect keypoints.
    //  vl_sift_detect(mSiftCpu);

    //  // Extract detected keypoints.
    //  const VlSiftKeypoint* vl_keypoints = vl_sift_get_keypoints(mSiftCpu);
    //  const int num_keypoints = vl_sift_get_nkeypoints(mSiftCpu);
    //  if (num_keypoints == 0) {
    //    continue;
    //  }

    //  // Extract features with different orientations per DOG level.
    //  size_t level_idx = 0;
    //  int prev_level = -1;
    //  for (int i = 0; i < num_keypoints; ++i) {
    //    if (vl_keypoints[i].is != prev_level) {
    //      if (i > 0) {
    //        // Resize containers of previous DOG level.
    //        level_keypoints.back().resize(level_idx);
    //        if (descriptors != nullptr) {
    //          level_descriptors.back().conservativeResize(level_idx, 128);
    //        }
    //      }

    //      // Add containers for new DOG level.
    //      level_idx = 0;
    //      level_num_features.push_back(0);
    //      level_keypoints.emplace_back(options.max_num_orientations *
    //                                   num_keypoints);
    //      if (descriptors != nullptr) {
    //        level_descriptors.emplace_back(
    //            options.max_num_orientations * num_keypoints, 128);
    //      }
    //    }

    //    level_num_features.back() += 1;
    //    prev_level = vl_keypoints[i].is;

    //    // Extract feature orientations.
    //    double angles[4];
    //    int num_orientations;
    //    if (options.upright) {
    //      num_orientations = 1;
    //      angles[0] = 0.0;
    //    } else {
    //      num_orientations = vl_sift_calc_keypoint_orientations(
    //          mSiftCpu, angles, &vl_keypoints[i]);
    //    }

    //    // Note that this is different from SiftGPU, which selects the top
    //    // global maxima as orientations while this selects the first two
    //    // local maxima. It is not clear which procedure is better.
    //    const int num_used_orientations =
    //        std::min(num_orientations, options.max_num_orientations);

    //    for (int o = 0; o < num_used_orientations; ++o) {
    //      level_keypoints.back()[level_idx] =
    //          FeatureKeypoint(vl_keypoints[i].x + 0.5f, vl_keypoints[i].y + 0.5f,
    //                          vl_keypoints[i].sigma, angles[o]);
    //      if (descriptors != nullptr) {
    //        Eigen::MatrixXf desc(1, 128);
    //        vl_sift_calc_keypoint_descriptor(mSiftCpu, desc.data(),
    //                                         &vl_keypoints[i], angles[o]);
    //        if (options.normalization ==
    //            SiftExtractionOptions::Normalization::L2) {
    //          desc = L2NormalizeFeatureDescriptors(desc);
    //        } else if (options.normalization ==
    //                   SiftExtractionOptions::Normalization::L1_ROOT) {
    //          desc = L1RootNormalizeFeatureDescriptors(desc);
    //        } else {
    //          LOG(FATAL) << "Normalization type not supported";
    //        }

    //        level_descriptors.back().row(level_idx) =
    //            FeatureDescriptorsToUnsignedByte(desc);
    //      }

    //      level_idx += 1;
    //    }
    //  }

    //  // Resize containers for last DOG level in octave.
    //  level_keypoints.back().resize(level_idx);
    //  if (descriptors != nullptr) {
    //    level_descriptors.back().conservativeResize(level_idx, 128);
    //  }
    //}

    //// Determine how many DOG levels to keep to satisfy max_num_features option.
    //int first_level_to_keep = 0;
    //int num_features = 0;
    //int num_features_with_orientations = 0;
    //for (int i = level_keypoints.size() - 1; i >= 0; --i) {
    //  num_features += level_num_features[i];
    //  num_features_with_orientations += level_keypoints[i].size();
    //  if (num_features > options.max_num_features) {
    //    first_level_to_keep = i;
    //    break;
    //  }
    //}

    //// Extract the features to be kept.
    //{
    //  size_t k = 0;
    //  keypoints->resize(num_features_with_orientations);
    //  for (size_t i = first_level_to_keep; i < level_keypoints.size(); ++i) {
    //    for (size_t j = 0; j < level_keypoints[i].size(); ++j) {
    //      (*keypoints)[k] = level_keypoints[i][j];
    //      k += 1;
    //    }
    //  }
    //}

    //// Compute the descriptors for the detected keypoints.
    //if (descriptors != nullptr) {
    //  size_t k = 0;
    //  descriptors->resize(num_features_with_orientations, 128);
    //  for (size_t i = first_level_to_keep; i < level_keypoints.size(); ++i) {
    //    for (size_t j = 0; j < level_keypoints[i].size(); ++j) {
    //      descriptors->row(k) = level_descriptors[i].row(j);
    //      k += 1;
    //    }
    //  }
    //  *descriptors = TransformVLFeatToUBCFeatureDescriptors(*descriptors);
    //}
  } catch (std::exception &e) {
    msgError("SIFT Detector exception");
    throw;
  }
}


void SiftDetectorDescriptor::setFeaturesNumber(int featuresNumber)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  update();
}

void SiftDetectorDescriptor::setOctaveLayers(int octaveLayers)
{
  SiftProperties::setOctaveLayers(octaveLayers);
  update();
}

void SiftDetectorDescriptor::setContrastThreshold(double contrastThreshold)
{
  SiftProperties::setContrastThreshold(contrastThreshold);
  update();
}

void SiftDetectorDescriptor::setEdgeThreshold(double edgeThreshold)
{
  SiftProperties::setEdgeThreshold(edgeThreshold);
  update();
}

void SiftDetectorDescriptor::setSigma(double sigma)
{
  SiftProperties::setSigma(sigma);
  update();
}

void SiftDetectorDescriptor::reset()
{
  SiftProperties::reset();
  update();
}


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
  std::lock_guard<std::mutex> lck(mMutex);
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
  std::lock_guard<std::mutex> lck(mMutex);

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







