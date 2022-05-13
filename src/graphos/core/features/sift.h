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

#ifndef GRAPHOS_CORE_SIFT_DETECTOR_DESCRIPTOR_H
#define GRAPHOS_CORE_SIFT_DETECTOR_DESCRIPTOR_H

#include "graphos/graphos_global.h"

#include "graphos/core/features/features.h"

#include <opencv2/features2d.hpp>
#if defined OPENCV_ENABLE_NONFREE && defined HAVE_OPENCV_XFEATURES2D
#include <opencv2/xfeatures2d.hpp>
#endif // HAVE_OPENCV_XFEATURES2D

#include "SiftGPU/SiftGPU.h"
#include <colmap/feature/sift.h>


namespace graphos
{


/*!
 * \brief SIFT detector/descriptor properties class
 */
class SiftProperties
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

#endif // GRAPHOS_CORE_SIFT_DETECTOR_DESCRIPTOR_H
