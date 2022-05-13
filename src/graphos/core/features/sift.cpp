/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/core/features/sift.h"

#include <tidop/core/messages.h>
#include <tidop/core/exception.h>

#include <colmap/util/opengl_utils.h>
#include <colmap/feature/utils.h>
#include <colmap/feature/sift.h>

#include <opencv2/imgcodecs.hpp>

#include <gl/GL.h>

namespace graphos
{


SiftProperties::SiftProperties()
  : mFeaturesNumber(5000),
    mOctaveLayers(3),
    mContrastThresholdAuto(true),
    mContrastThreshold(0.0067),
    mEdgeThreshold(10.)
{
}

SiftProperties::SiftProperties(const SiftProperties &siftProperties)
  : Sift(siftProperties),
    mFeaturesNumber(siftProperties.mFeaturesNumber),
    mOctaveLayers(siftProperties.mOctaveLayers),
    mContrastThresholdAuto(siftProperties.mContrastThresholdAuto),
    mContrastThreshold(siftProperties.mContrastThreshold),
    mEdgeThreshold(siftProperties.mEdgeThreshold)
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

void SiftProperties::reset()
{
  mFeaturesNumber = 5000;
  mOctaveLayers = 3;
  mContrastThresholdAuto = true;
  mContrastThreshold = 0.02 / 3.;
  mEdgeThreshold = 10.;
}

QString SiftProperties::name() const
{
  return QString("SIFT");
}


/*----------------------------------------------------------------*/


SiftCPUDetectorDescriptor::SiftCPUDetectorDescriptor()
{
  update();
}

SiftCPUDetectorDescriptor::SiftCPUDetectorDescriptor(const SiftCPUDetectorDescriptor &siftDetectorDescriptor)
  : SiftProperties(siftDetectorDescriptor),
    FeatureExtractor(siftDetectorDescriptor)
{
  update();
}

SiftCPUDetectorDescriptor::SiftCPUDetectorDescriptor(int featuresNumber,
                                                     int octaveLayers,
                                                     double edgeThreshold,
                                                     double contrastThreshold)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  SiftProperties::setOctaveLayers(octaveLayers);
  if (contrastThreshold > 0.) {
    SiftProperties::setContrastThresholdAuto(false);
    SiftProperties::setContrastThreshold(contrastThreshold);
  }
  SiftProperties::setEdgeThreshold(edgeThreshold);
  update();
}

SiftCPUDetectorDescriptor::~SiftCPUDetectorDescriptor()
{
}

void SiftCPUDetectorDescriptor::update()
{
#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4))
  mSift = cv::SIFT::create(SiftProperties::featuresNumber(),
                           SiftProperties::octaveLayers(),
                           SiftProperties::contrastThreshold(),
                           SiftProperties::edgeThreshold());
#elif defined HAVE_OPENCV_XFEATURES2D && defined OPENCV_ENABLE_NONFREE
  mSift = cv::xfeatures2d::SIFT::create(SiftProperties::featuresNumber(),
                                        SiftProperties::octaveLayers(),
                                        SiftProperties::contrastThreshold(),
                                        SiftProperties::edgeThreshold());
#endif
}

void SiftCPUDetectorDescriptor::run(const cv::Mat &bitmap,
                                    colmap::FeatureKeypoints &keyPoints,
                                    colmap::FeatureDescriptors &descriptors)
{
  std::lock_guard<std::mutex> lck(mMutex);

  std::vector<cv::KeyPoint> opencv_key_points;
  cv::Mat opencv_descriptors;

  try {

#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4)) || (defined HAVE_OPENCV_XFEATURES2D && defined OPENCV_ENABLE_NONFREE)
    mSift->detect(bitmap, opencv_key_points/*, mask*/);
#else
    TL_COMPILER_WARNING("OpenCV not built with extra modules. Sift Detector/Descriptor not supported")
      throw TL_ERROR("OpenCV not built with extra modules. Sift Detector/Descriptor not supported");
#endif // HAVE_OPENCV_XFEATURES2D



#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4)) || (defined HAVE_OPENCV_XFEATURES2D && defined OPENCV_ENABLE_NONFREE)
    mSift->compute(bitmap, opencv_key_points, opencv_descriptors);
#else
    TL_COMPILER_WARNING("OpenCV not built with extra modules. Sift Detector/Descriptor not supported")
      throw TL_ERROR("OpenCV not built with extra modules. Sift Detector/Descriptor not supported");
#endif // HAVE_OPENCV_XFEATURES2D

    size_t max_features = std::min(static_cast<int>(opencv_key_points.size()), SiftProperties::featuresNumber());
    keyPoints.resize(max_features);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> descriptors_float_resize(max_features, 
                                                                                                   opencv_descriptors.cols);
    for (size_t i = 0; i < max_features; i++) {
      keyPoints[i] = colmap::FeatureKeypoint(opencv_key_points[i].pt.x, 
                                             opencv_key_points[i].pt.y,
                                             opencv_key_points[i].size, 
                                             opencv_key_points[i].angle);
      /// Ver si se puede mejorar
      for (size_t j = 0; j < opencv_descriptors.cols; j++) {
        descriptors_float_resize(i, j) = opencv_descriptors.at<float>(i, j);
      }
    }

    if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L2) {
      descriptors_float_resize = colmap::L2NormalizeFeatureDescriptors(descriptors_float_resize);
    } else if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L1_ROOT) {
      descriptors_float_resize = colmap::L1RootNormalizeFeatureDescriptors(descriptors_float_resize);
    } else {
      throw std::runtime_error("Description normalization type not supported");
    }

    descriptors = colmap::FeatureDescriptorsToUnsignedByte(descriptors_float_resize);

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }
}


void SiftCPUDetectorDescriptor::setFeaturesNumber(int featuresNumber)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  update();
}

void SiftCPUDetectorDescriptor::setOctaveLayers(int octaveLayers)
{
  SiftProperties::setOctaveLayers(octaveLayers);
  update();
}

void SiftCPUDetectorDescriptor::setContrastThreshold(double contrastThreshold)
{
  SiftProperties::setContrastThreshold(contrastThreshold);
  update();
}

void SiftCPUDetectorDescriptor::setEdgeThreshold(double edgeThreshold)
{
  SiftProperties::setEdgeThreshold(edgeThreshold);
  update();
}

void SiftCPUDetectorDescriptor::reset()
{
  SiftProperties::reset();
  update();
}


/*----------------------------------------------------------------*/



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
                                                       double contrastThreshold)
{
  SiftProperties::setFeaturesNumber(featuresNumber);
  SiftProperties::setOctaveLayers(octaveLayers);
  if (contrastThreshold > 0.) {
    SiftProperties::setContrastThresholdAuto(false);
    SiftProperties::setContrastThreshold(contrastThreshold);
  }
  SiftProperties::setEdgeThreshold(edgeThreshold);
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

    size_t max_features = std::min(feature_number, SiftProperties::featuresNumber());
    //size_t max_features = feature_number;
    keyPoints.resize(max_features);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> descriptors_float_resize(max_features, descriptors_float.cols());
    for (size_t i = 0; i < max_features; i++){
      keyPoints[i] = colmap::FeatureKeypoint(keypoints_data[i].x, keypoints_data[i].y,
                                             keypoints_data[i].s, keypoints_data[i].o);
      /// Ver si se puede mejorar
      for (size_t j = 0; j < descriptors_float.cols(); j++) {
        descriptors_float_resize(i, j) = descriptors_float(i, j);
      }
    }

    if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L2){
      descriptors_float_resize = colmap::L2NormalizeFeatureDescriptors(descriptors_float_resize);
    } else if (mSiftExtractionOptions.normalization == colmap::SiftExtractionOptions::Normalization::L1_ROOT){
      descriptors_float_resize = colmap::L1RootNormalizeFeatureDescriptors(descriptors_float_resize);
    } else {
      throw std::runtime_error("Description normalization type not supported");
    }
    
    descriptors = colmap::FeatureDescriptorsToUnsignedByte(descriptors_float_resize);

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

void SiftCudaDetectorDescriptor::reset()
{
  mSiftGpu.reset(new SiftGPU);
}



} // namespace graphos







