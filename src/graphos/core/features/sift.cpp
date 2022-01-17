#include "sift.h"

#include <tidop/core/messages.h>
#include <tidop/core/exception.h>

#include <colmap/util/opengl_utils.h>
#include <colmap/feature/utils.h>


#include <opencv2/imgcodecs.hpp>

#include <gl/GL.h>

namespace graphos
{


SiftProperties::SiftProperties()
  : mFeaturesNumber(5000),
    mOctaveLayers(3),
    mContrastThresholdAuto(true),
    mContrastThreshold(0.0067),
    mEdgeThreshold(10.)/*,
    mSigma(1.6)*/
{
}

SiftProperties::SiftProperties(const SiftProperties &siftProperties)
  : Sift(siftProperties),
    mFeaturesNumber(siftProperties.mFeaturesNumber),
    mOctaveLayers(siftProperties.mOctaveLayers),
    mContrastThresholdAuto(siftProperties.mContrastThresholdAuto),
    mContrastThreshold(siftProperties.mContrastThreshold),
    mEdgeThreshold(siftProperties.mEdgeThreshold)/*,
    mSigma(siftProperties.mSigma)*/
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

//double SiftProperties::sigma() const
//{
//  return mSigma;
//}

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

//void SiftProperties::setSigma(double sigma)
//{
//  mSigma = sigma;
//}

void SiftProperties::reset()
{
  mFeaturesNumber = 5000;
  mOctaveLayers = 3;
  mContrastThresholdAuto = true;
  mContrastThreshold = 0.02 / 3.;
  mEdgeThreshold = 10.;
  //mSigma = 1.6;
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
                                               double edgeThreshold,
                                               //double sigma,
                                               double contrastThreshold)
  : mSiftCpu(nullptr)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  SiftProperties::setOctaveLayers(octaveLayers);
  if (contrastThreshold > 0.) {
    SiftProperties::setContrastThresholdAuto(false);
    SiftProperties::setContrastThreshold(contrastThreshold);
  }
  SiftProperties::setEdgeThreshold(edgeThreshold);
  //SiftProperties::setSigma(sigma);
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
  mSiftExtractionOptions.max_num_features = SiftProperties::featuresNumber();
  mSiftExtractionOptions.octave_resolution = SiftProperties::octaveLayers();
  mSiftExtractionOptions.edge_threshold = SiftProperties::edgeThreshold();
  mSiftExtractionOptions.peak_threshold = SiftProperties::contrastThreshold();
}

void SiftDetectorDescriptor::run(const colmap::Bitmap &bitmap,
                                 colmap::FeatureKeypoints &keyPoints,
                                 colmap::FeatureDescriptors &descriptors)
{
  std::lock_guard<std::mutex> lck(mMutex);

  try {
    mSiftExtractionOptions.max_image_size = std::max(bitmap.Width(), bitmap.Height());
    mSiftExtractionOptions.use_gpu = false;
    bool err = ExtractSiftFeaturesCPU(mSiftExtractionOptions, bitmap, &keyPoints, &descriptors);
    if (err == false) throw "ExtractSiftFeaturesCPU fail";
  } catch (std::exception &) {
    msgError("SIFT Detector exception");
    throw;
  } catch (...) {
    msgError("SIFT Detector exception");
    throw;
  }
}

//colmap::FeatureDescriptors TransformVLFeatToUBCFeatureDescriptors(
//  const colmap::FeatureDescriptors &vlfeat_descriptors)
//{
//  colmap::FeatureDescriptors ubc_descriptors(vlfeat_descriptors.rows(),
//                                     vlfeat_descriptors.cols());
//  const std::array<int, 8> q{{0, 7, 6, 5, 4, 3, 2, 1}};
//  for (colmap::FeatureDescriptors::Index n = 0; n < vlfeat_descriptors.rows(); ++n) {
//    for (int i = 0; i < 4; ++i) {
//      for (int j = 0; j < 4; ++j) {
//        for (int k = 0; k < 8; ++k) {
//          ubc_descriptors(n, 8 * (j + 4 * i) + q[k]) =
//            vlfeat_descriptors(n, 8 * (j + 4 * i) + k);
//        }
//      }
//    }
//  }
//  return ubc_descriptors;
//}
//
//void SiftDetectorDescriptor::run(const colmap::Bitmap &bitmap,
//                                 colmap::FeatureKeypoints &keyPoints,
//                                 colmap::FeatureDescriptors &descriptors)
//{
//  std::lock_guard<std::mutex> lck(mMutex);
//  try {
//
//    using namespace colmap;
//
//    mSiftExtractionOptions.use_gpu = false;
//
//    TL_ASSERT(mSiftExtractionOptions.Check(),"");
//    TL_ASSERT(bitmap.IsGrey(), "");
//
//    CHECK(!mSiftExtractionOptions.estimate_affine_shape);
//    CHECK(!mSiftExtractionOptions.domain_size_pooling);
//    
//    //if (mSiftExtractionOptions.darkness_adaptivity) {
//    //  WarnDarknessAdaptivityNotAvailable();
//    //}
//
//    // Setup SIFT extractor.
//    //std::unique_ptr<VlSiftFilt, void (*)(VlSiftFilt *)> sift(
//    //  vl_sift_new(bitmap.Width(), bitmap.Height(), mSiftExtractionOptions.num_octaves,
//    //              mSiftExtractionOptions.octave_resolution, mSiftExtractionOptions.first_octave),
//    //  &vl_sift_delete);
//    int w = bitmap.Width();
//    int h = bitmap.Height();
//    mSiftCpu = vl_sift_new(w, h,
//                           mSiftExtractionOptions.num_octaves,
//                           mSiftExtractionOptions.octave_resolution,
//                           mSiftExtractionOptions.first_octave);
//    if (!mSiftCpu) throw std::runtime_error("ExtractSiftFeaturesCPU fail");
//    //if (!sift) {
//    //  return;
//    //}
//
//    vl_sift_set_peak_thresh(mSiftCpu, mSiftExtractionOptions.peak_threshold);
//    vl_sift_set_edge_thresh(mSiftCpu, mSiftExtractionOptions.edge_threshold);
//
//    // Iterate through octaves.
//    std::vector<size_t> level_num_features;
//    std::vector<FeatureKeypoints> level_keypoints;
//    std::vector<FeatureDescriptors> level_descriptors;
//    bool first_octave = true;
//    while (true) {
//      if (first_octave) {
//        const std::vector<uint8_t> data_uint8 = bitmap.ConvertToRowMajorArray();
//        std::vector<float> data_float(data_uint8.size());
//        for (size_t i = 0; i < data_uint8.size(); ++i) {
//          data_float[i] = static_cast<float>(data_uint8[i]) / 255.0f;
//        }
//        if (vl_sift_process_first_octave(mSiftCpu, data_float.data())) {
//          break;
//        }
//        first_octave = false;
//      } else {
//        if (vl_sift_process_next_octave(mSiftCpu)) {
//          break;
//        }
//      }
//
//      // Detect keypoints.
//      vl_sift_detect(mSiftCpu);
//
//      // Extract detected keypoints.
//      const VlSiftKeypoint *vl_keypoints = vl_sift_get_keypoints(mSiftCpu);
//      const int num_keypoints = vl_sift_get_nkeypoints(mSiftCpu);
//      if (num_keypoints == 0) {
//        continue;
//      }
//
//      // Extract features with different orientations per DOG level.
//      size_t level_idx = 0;
//      int prev_level = -1;
//      for (int i = 0; i < num_keypoints; ++i) {
//        if (vl_keypoints[i].is != prev_level) {
//          if (i > 0) {
//            // Resize containers of previous DOG level.
//            level_keypoints.back().resize(level_idx);
//            //if (descriptors != nullptr) {
//              level_descriptors.back().conservativeResize(level_idx, 128);
//            //}
//          }
//
//          // Add containers for new DOG level.
//          level_idx = 0;
//          level_num_features.push_back(0);
//          level_keypoints.emplace_back(mSiftExtractionOptions.max_num_orientations *
//                                       num_keypoints);
//          //if (descriptors != nullptr) {
//            level_descriptors.emplace_back(
//              mSiftExtractionOptions.max_num_orientations * num_keypoints, 128);
//          //}
//        }
//
//        level_num_features.back() += 1;
//        prev_level = vl_keypoints[i].is;
//
//        // Extract feature orientations.
//        double angles[4];
//        int num_orientations;
//        if (mSiftExtractionOptions.upright) {
//          num_orientations = 1;
//          angles[0] = 0.0;
//        } else {
//          num_orientations = vl_sift_calc_keypoint_orientations(
//            mSiftCpu, angles, &vl_keypoints[i]);
//        }
//
//        // Note that this is different from SiftGPU, which selects the top
//        // global maxima as orientations while this selects the first two
//        // local maxima. It is not clear which procedure is better.
//        const int num_used_orientations =
//          std::min(num_orientations, mSiftExtractionOptions.max_num_orientations);
//
//        for (int o = 0; o < num_used_orientations; ++o) {
//          level_keypoints.back()[level_idx] =
//            FeatureKeypoint(vl_keypoints[i].x + 0.5f, vl_keypoints[i].y + 0.5f,
//                            vl_keypoints[i].sigma, angles[o]);
//          //if (descriptors != nullptr) {
//            Eigen::MatrixXf desc(1, 128);
//            vl_sift_calc_keypoint_descriptor(mSiftCpu, desc.data(),
//                                             &vl_keypoints[i], angles[o]);
//            if (mSiftExtractionOptions.normalization ==
//                SiftExtractionOptions::Normalization::L2) {
//              desc = L2NormalizeFeatureDescriptors(desc);
//            } else if (mSiftExtractionOptions.normalization ==
//                       SiftExtractionOptions::Normalization::L1_ROOT) {
//              desc = L1RootNormalizeFeatureDescriptors(desc);
//            } else {
//              LOG(FATAL) << "Normalization type not supported";
//            }
//
//            level_descriptors.back().row(level_idx) =
//              FeatureDescriptorsToUnsignedByte(desc);
//          //}
//
//          level_idx += 1;
//        }
//      }
//
//      // Resize containers for last DOG level in octave.
//      level_keypoints.back().resize(level_idx);
//      //if (descriptors != nullptr) {
//        level_descriptors.back().conservativeResize(level_idx, 128);
//      //}
//    }
//
//    // Determine how many DOG levels to keep to satisfy max_num_features option.
//    int first_level_to_keep = 0;
//    int num_features = 0;
//    int num_features_with_orientations = 0;
//    for (int i = level_keypoints.size() - 1; i >= 0; --i) {
//      num_features += level_num_features[i];
//      num_features_with_orientations += level_keypoints[i].size();
//      if (num_features > mSiftExtractionOptions.max_num_features) {
//        first_level_to_keep = i;
//        break;
//      }
//    }
//
//    // Extract the features to be kept.
//    {
//      size_t k = 0;
//      keyPoints.resize(num_features_with_orientations);
//      for (size_t i = first_level_to_keep; i < level_keypoints.size(); ++i) {
//        for (size_t j = 0; j < level_keypoints[i].size(); ++j) {
//          keyPoints[k] = level_keypoints[i][j];
//          k += 1;
//        }
//      }
//    }
//
//    // Compute the descriptors for the detected keypoints.
//    //if (descriptors != nullptr) {
//      size_t k = 0;
//      descriptors.resize(num_features_with_orientations, 128);
//      for (size_t i = first_level_to_keep; i < level_keypoints.size(); ++i) {
//        for (size_t j = 0; j < level_keypoints[i].size(); ++j) {
//          descriptors.row(k) = level_descriptors[i].row(j);
//          k += 1;
//        }
//      }
//      descriptors = TransformVLFeatToUBCFeatureDescriptors(descriptors);
//    //}
//
//
//  } catch (std::exception &) {
//    msgError("SIFT Detector exception");
//    throw;
//  } catch (...) {
//    msgError("SIFT Detector exception");
//    throw;
//  }
//}

void SiftDetectorDescriptor::run(const cv::Mat &bitmap,
                                 colmap::FeatureKeypoints &keyPoints,
                                 colmap::FeatureDescriptors &descriptors)
{
  std::lock_guard<std::mutex> lck(mMutex);

  try {

    /// Copiado mas o menos de Colmap

    mSiftExtractionOptions.max_image_size = std::max(bitmap.rows, bitmap.cols);
    mSiftExtractionOptions.use_gpu = false;

    mSiftCpu = vl_sift_new(bitmap.cols,
                           bitmap.rows,
                           mSiftExtractionOptions.num_octaves,
                           mSiftExtractionOptions.octave_resolution,
                           mSiftExtractionOptions.first_octave);
    
    if (!mSiftCpu) throw std::runtime_error("ExtractSiftFeaturesCPU fail");

    vl_sift_set_peak_thresh(mSiftCpu, mSiftExtractionOptions.peak_threshold);
    vl_sift_set_edge_thresh(mSiftCpu, mSiftExtractionOptions.edge_threshold);

    // Iterate through octaves.
    std::vector<size_t> level_num_features;
    std::vector<colmap::FeatureKeypoints> level_keypoints;
    std::vector<colmap::FeatureDescriptors> level_descriptors;
    bool first_octave = true;
    while (true) {
      if (first_octave) {
        //const std::vector<uint8_t> data_uint8 = bitmap.ConvertToRowMajorArray();
        std::vector<uint8_t> data_uint8(bitmap.rows * bitmap.cols, 0);
        data_uint8.assign(bitmap.data, bitmap.data + data_uint8.size());
        std::vector<float> data_float(data_uint8.size());
        for (size_t i = 0; i < data_uint8.size(); ++i) {
          data_float[i] = static_cast<float>(data_uint8[i]) / 255.0f;
        }
        if (vl_sift_process_first_octave(mSiftCpu, data_float.data())) {
          break;
        }
        first_octave = false;
      } else {
        if (vl_sift_process_next_octave(mSiftCpu)) {
          break;
        }
      }

      // Detect keypoints.
      vl_sift_detect(mSiftCpu);

      // Extract detected keypoints.
      const VlSiftKeypoint* vl_keypoints = vl_sift_get_keypoints(mSiftCpu);
      const int num_keypoints = vl_sift_get_nkeypoints(mSiftCpu);
      if (num_keypoints == 0) {
        continue;
      }

      // Extract features with different orientations per DOG level.
      size_t level_idx = 0;
      int prev_level = -1;
      for (int i = 0; i < num_keypoints; ++i) {
        if (vl_keypoints[i].is != prev_level) {
          if (i > 0) {
            // Resize containers of previous DOG level.
            level_keypoints.back().resize(level_idx);
            // No lo paso como puntero y no es opcional
            //if (descriptors != nullptr) {
              level_descriptors.back().conservativeResize(level_idx, 128);
            //}
          }

          // Add containers for new DOG level.
          level_idx = 0;
          level_num_features.push_back(0);
          /// Pongo en valor por defecto de Colmap
          level_keypoints.emplace_back(mSiftExtractionOptions.max_num_orientations *
                                       num_keypoints);
          //if (descriptors != nullptr) {
            level_descriptors.emplace_back(
                mSiftExtractionOptions.max_num_orientations * num_keypoints, 128);
          //}
        }

        level_num_features.back() += 1;
        prev_level = vl_keypoints[i].is;

        // Extract feature orientations.
        double angles[4];
        int num_orientations;
        if (mSiftExtractionOptions.upright) {
          num_orientations = 1;
          angles[0] = 0.0;
        } else {
          num_orientations = vl_sift_calc_keypoint_orientations(
              mSiftCpu, angles, &vl_keypoints[i]);
        }

        // Note that this is different from SiftGPU, which selects the top
        // global maxima as orientations while this selects the first two
        // local maxima. It is not clear which procedure is better.
        const int num_used_orientations =
            std::min(num_orientations, mSiftExtractionOptions.max_num_orientations);

        for (int o = 0; o < num_used_orientations; ++o) {
          level_keypoints.back()[level_idx] =
            colmap::FeatureKeypoint(vl_keypoints[i].x + 0.5f, vl_keypoints[i].y + 0.5f,
                                    vl_keypoints[i].sigma, angles[o]);

          Eigen::MatrixXf desc(1, 128);
          vl_sift_calc_keypoint_descriptor(mSiftCpu, desc.data(),
                                           &vl_keypoints[i], angles[o]);

          if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L2) {
            desc = colmap::L2NormalizeFeatureDescriptors(desc);
          } else if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L1_ROOT) {
            desc = colmap::L1RootNormalizeFeatureDescriptors(desc);
          } else {
            throw std::runtime_error("Description normalization type not supported");
          }

          level_descriptors.back().row(level_idx) = colmap::FeatureDescriptorsToUnsignedByte(desc);

          level_idx += 1;
        }
      }

      // Resize containers for last DOG level in octave.
      level_keypoints.back().resize(level_idx);
      //if (descriptors != nullptr) {
        level_descriptors.back().conservativeResize(level_idx, 128);
      //}
    }

    // Determine how many DOG levels to keep to satisfy max_num_features option.
    int first_level_to_keep = 0;
    int num_features = 0;
    int num_features_with_orientations = 0;
    for (int i = level_keypoints.size() - 1; i >= 0; --i) {
      num_features += level_num_features[i];
      num_features_with_orientations += level_keypoints[i].size();
      if (num_features > mSiftExtractionOptions.max_num_features) {
        first_level_to_keep = i;
        break;
      }
    }

    // Extract the features to be kept.
    {
      size_t k = 0;
      keyPoints.resize(num_features_with_orientations);
      for (size_t i = first_level_to_keep; i < level_keypoints.size(); ++i) {
        for (size_t j = 0; j < level_keypoints[i].size(); ++j) {
          keyPoints[k] = level_keypoints[i][j];
          k += 1;
        }
      }
    }

    // Compute the descriptors for the detected keypoints.
    //if (descriptors != nullptr) {
      size_t k = 0;
      descriptors.resize(num_features_with_orientations, 128);
      for (size_t i = first_level_to_keep; i < level_keypoints.size(); ++i) {
        for (size_t j = 0; j < level_keypoints[i].size(); ++j) {
          descriptors.row(k) = level_descriptors[i].row(j);
          k += 1;
        }
      }
      //descriptors = colmap::TransformVLFeatToUBCFeatureDescriptors(descriptors);
      colmap::FeatureDescriptors ubc_descriptors(descriptors.rows(),
                                                 descriptors.cols());
      const std::array<int, 8> q{{0, 7, 6, 5, 4, 3, 2, 1}};
      for (colmap::FeatureDescriptors::Index n = 0; n < descriptors.rows(); ++n) {
        for (int i = 0; i < 4; ++i) {
          for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 8; ++k) {
              ubc_descriptors(n, 8 * (j + 4 * i) + q[k]) =
                descriptors(n, 8 * (j + 4 * i) + k);
            }
          }
        }
      }
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

//void SiftDetectorDescriptor::setSigma(double sigma)
//{
//  SiftProperties::setSigma(sigma);
//  update();
//}

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
                                                       //double sigma,
                                                       double contrastThreshold)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  SiftProperties::setOctaveLayers(octaveLayers);
  if (contrastThreshold > 0.) {
    SiftProperties::setContrastThresholdAuto(false);
    SiftProperties::setContrastThreshold(contrastThreshold);
  }
  SiftProperties::setEdgeThreshold(edgeThreshold);
  //SiftProperties::setSigma(sigma);
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
  mSiftExtractionOptions.domain_size_pooling = false;
  mSiftExtractionOptions.use_gpu = true;
  if (!CreateSiftGPUExtractor(mSiftExtractionOptions, mSiftGpu.get())) {
    return;
  }
}

void SiftCudaDetectorDescriptor::run(const colmap::Bitmap &bitmap,
                                     colmap::FeatureKeypoints &keyPoints,
                                     colmap::FeatureDescriptors &descriptors)
{
  std::lock_guard<std::mutex> lck(mMutex);
  update();
  try {
    mSiftExtractionOptions.max_image_size = std::max(bitmap.Width(), bitmap.Height());
    mSiftGpu->SetMaxDimension(mSiftExtractionOptions.max_image_size);
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
  update();
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
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> descriptors_float_resize(max_features, descriptors_float.cols());
    for (size_t i = 0; i < max_features; i++){
      keyPoints[i] = colmap::FeatureKeypoint(keypoints_data[i].x, keypoints_data[i].y,
                                             keypoints_data[i].s, keypoints_data[i].o);
      /// Ver si se puede mejorar
      //for (size_t j = 0; j < descriptors_float.cols(); j++) {
      //  descriptors_float_resize(i, j) = descriptors_float(i, j);
      //}
    }

    /// Destruye el contenido
    ///descriptors_float.resize(max_features, Eigen::NoChange);

    if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L2){
      descriptors_float = colmap::L2NormalizeFeatureDescriptors(descriptors_float);
    } else if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L1_ROOT){
      descriptors_float = colmap::L1RootNormalizeFeatureDescriptors(descriptors_float);
    } else {
      throw std::runtime_error("Description normalization type not supported");
    }
    
    descriptors = colmap::FeatureDescriptorsToUnsignedByte(descriptors_float);

  } catch (std::exception &) {
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

//void SiftCudaDetectorDescriptor::setSigma(double sigma)
//{
//  SiftProperties::setSigma(sigma);
//  update();
//}

void SiftCudaDetectorDescriptor::reset()
{
  mSiftGpu.reset(new SiftGPU);
}



} // namespace graphos







