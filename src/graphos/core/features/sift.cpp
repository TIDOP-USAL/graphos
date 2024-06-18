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

#include <tidop/core/msg/message.h>
#include <tidop/core/exception.h>

#include <colmap/util/opengl_utils.h>
#include <colmap/feature/sift.h>
#include "SiftGPU/SiftGPU.h"

#include <opencv2/imgcodecs.hpp>

#include <GL/gl.h>

namespace graphos
{


Sift::Sift()
  : Feature(Type::sift),
    mFeaturesNumber(5000),
    mOctaveLayers(3),
    mContrastThresholdAuto(true),
    mContrastThreshold(0.0067),
    mEdgeThreshold(10.)
{
}

Sift::Sift(const Sift &sift)
  : Feature(Type::sift),
    mFeaturesNumber(sift.mFeaturesNumber),
    mOctaveLayers(sift.mOctaveLayers),
    mContrastThresholdAuto(sift.mContrastThresholdAuto),
    mContrastThreshold(sift.mContrastThreshold),
    mEdgeThreshold(sift.mEdgeThreshold)
{
}

auto Sift::featuresNumber() const -> int
{
    return mFeaturesNumber;
}

auto Sift::octaveLayers() const -> int
{
    return mOctaveLayers;
}

auto Sift::constrastThresholdAuto() const -> bool
{
    return mContrastThresholdAuto;
}

auto Sift::contrastThreshold() const -> double
{
    return mContrastThresholdAuto ? 0.02 / mOctaveLayers : mContrastThreshold;
}

auto Sift::edgeThreshold() const -> double
{
    return mEdgeThreshold;
}

auto Sift::domainSizePooling() const -> bool
{
    return mDomainSizePooling;
}

void Sift::setFeaturesNumber(int featuresNumber)
{
    mFeaturesNumber = featuresNumber;
}

void Sift::setOctaveLayers(int octaveLayers)
{
    mOctaveLayers = octaveLayers;
}

void Sift::setContrastThresholdAuto(bool active)
{
    mContrastThresholdAuto = active;
}

void Sift::setContrastThreshold(double contrastThreshold)
{
    mContrastThreshold = contrastThreshold;
}

void Sift::setEdgeThreshold(double edgeThreshold)
{
    mEdgeThreshold = edgeThreshold;
}

void Sift::setDomainSizePooling(bool domainSizePooling)
{
    mDomainSizePooling = domainSizePooling;
}

void Sift::reset()
{
    mFeaturesNumber = 5000;
    mOctaveLayers = 3;
    mContrastThresholdAuto = true;
    mContrastThreshold = 0.02 / 3.;
    mEdgeThreshold = 10.;
    mDomainSizePooling = true;
}

auto Sift::name() const -> QString
{
    return {"SIFT"};
}




/* SiftCPUDetectorDescriptor */


SiftCPUDetectorDescriptor::SiftCPUDetectorDescriptor()
{
    update();
}

SiftCPUDetectorDescriptor::SiftCPUDetectorDescriptor(const SiftCPUDetectorDescriptor &sift)
  : Sift(sift),
    FeatureExtractor(sift)
{
    update();
}

SiftCPUDetectorDescriptor::SiftCPUDetectorDescriptor(int featuresNumber,
                                                     int octaveLayers,
                                                     double edgeThreshold,
                                                     double contrastThreshold)
{
    Sift::setFeaturesNumber(featuresNumber);
    Sift::setOctaveLayers(octaveLayers);
    if (contrastThreshold > 0.) {
        Sift::setContrastThresholdAuto(false);
        Sift::setContrastThreshold(contrastThreshold);
    }
    Sift::setEdgeThreshold(edgeThreshold);
    update();
}

SiftCPUDetectorDescriptor::~SiftCPUDetectorDescriptor() = default;

void SiftCPUDetectorDescriptor::update()
{
#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4))
    mSift = cv::SIFT::create(Sift::featuresNumber(),
                             Sift::octaveLayers(),
                             Sift::contrastThreshold(),
                             Sift::edgeThreshold());
#elif defined HAVE_OPENCV_XFEATURES2D && defined OPENCV_ENABLE_NONFREE
    mSift = cv::xfeatures2d::SIFT::create(Sift::featuresNumber(),
                                          Sift::octaveLayers(),
                                          Sift::contrastThreshold(),
                                          Sift::edgeThreshold());
#endif
}

void SiftCPUDetectorDescriptor::run(const cv::Mat &bitmap,
                                    std::vector<cv::KeyPoint> &keyPoints,
                                    cv::Mat &descriptors)
{
    std::lock_guard<std::mutex> lck(mMutex);

    try {

        cv::Mat opencv_descriptors;
        std::vector<cv::KeyPoint> opencv_key_points;

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

        keyPoints = opencv_key_points;
        descriptors = opencv_descriptors;

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}


void SiftCPUDetectorDescriptor::setFeaturesNumber(int featuresNumber)
{
    Sift::setFeaturesNumber(featuresNumber);
    update();
}

void SiftCPUDetectorDescriptor::setOctaveLayers(int octaveLayers)
{
    Sift::setOctaveLayers(octaveLayers);
    update();
}

void SiftCPUDetectorDescriptor::setContrastThreshold(double contrastThreshold)
{
    Sift::setContrastThreshold(contrastThreshold);
    update();
}

void SiftCPUDetectorDescriptor::setEdgeThreshold(double edgeThreshold)
{
    Sift::setEdgeThreshold(edgeThreshold);
    update();
}

void SiftCPUDetectorDescriptor::reset()
{
    Sift::reset();
    update();
}




/* SiftCudaDetectorDescriptor */

SiftCudaDetectorDescriptor::SiftCudaDetectorDescriptor()
{
    update();
}

SiftCudaDetectorDescriptor::SiftCudaDetectorDescriptor(const SiftCudaDetectorDescriptor &sift)
  : Sift(sift),
    FeatureExtractor(sift)
{
    update();
}

SiftCudaDetectorDescriptor::SiftCudaDetectorDescriptor(int featuresNumber,
                                                       int octaveLayers,
                                                       double edgeThreshold,
                                                       double contrastThreshold,
                                                       bool domainSizePooling)
{
    Sift::setFeaturesNumber(featuresNumber);
    Sift::setOctaveLayers(octaveLayers);
    if (contrastThreshold > 0.) {
        Sift::setContrastThresholdAuto(false);
        Sift::setContrastThreshold(contrastThreshold);
    }
    Sift::setEdgeThreshold(edgeThreshold);
    Sift::setDomainSizePooling(domainSizePooling);
    update();
}

SiftCudaDetectorDescriptor::~SiftCudaDetectorDescriptor() = default;

void SiftCudaDetectorDescriptor::update()
{
    colmap::SiftExtractionOptions options;
    options.max_num_features = Sift::featuresNumber();
    options.octave_resolution = Sift::octaveLayers();
    options.edge_threshold = Sift::edgeThreshold();
    options.peak_threshold = Sift::contrastThreshold();
    options.domain_size_pooling = Sift::domainSizePooling();
    options.use_gpu = true;

    mSiftGpu.reset(new SiftGPU);

    TL_ASSERT(CreateSiftGPUExtractor(options, mSiftGpu.get()), "Error creating SiftGPUExtractor");
}

void SiftCudaDetectorDescriptor::run(const cv::Mat &bitmap,
                                     std::vector<cv::KeyPoint> &keyPoints,
                                     cv::Mat &descriptors)
{
    std::lock_guard<std::mutex> lck(mMutex);

    try {

        update();

        int err = mSiftGpu->RunSIFT(bitmap.cols, bitmap.rows, bitmap.data, GL_LUMINANCE, GL_UNSIGNED_BYTE);

        TL_ASSERT(err == 1, "ExtractSiftFeaturesGPU fail");

        int feature_number = mSiftGpu->GetFeatureNum();
        std::vector<SiftKeypoint> keypoints_data(feature_number);
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> descriptors_float(feature_number, 128);

        mSiftGpu->GetFeatureVector(keypoints_data.data(), descriptors_float.data());

        size_t max_features = std::min(feature_number, Sift::featuresNumber());
        keyPoints.resize(max_features);
        descriptors = cv::Mat(static_cast<int>(max_features), static_cast<int>(descriptors_float.cols()), CV_32F);
        for (size_t i = 0; i < max_features; i++) {
            keyPoints[i] = cv::KeyPoint(keypoints_data[i].x,
                                        keypoints_data[i].y,
                                        keypoints_data[i].s,
                                        keypoints_data[i].o);
            /// Ver si se puede mejorar
            for (size_t j = 0; j < static_cast<size_t>(descriptors_float.cols()); j++) {
                descriptors.at<float>(static_cast<int>(i), static_cast<int>(j)) = descriptors_float(i, j);
            }
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("SIFT exception");
    }
}

void SiftCudaDetectorDescriptor::setFeaturesNumber(int featuresNumber)
{
    Sift::setFeaturesNumber(featuresNumber);
    update();
}

void SiftCudaDetectorDescriptor::setOctaveLayers(int octaveLayers)
{
    Sift::setOctaveLayers(octaveLayers);
    update();
}

void SiftCudaDetectorDescriptor::setContrastThreshold(double contrastThreshold)
{
    Sift::setContrastThreshold(contrastThreshold);
    update();
}

void SiftCudaDetectorDescriptor::setEdgeThreshold(double edgeThreshold)
{
    Sift::setEdgeThreshold(edgeThreshold);
    update();
}

void SiftCudaDetectorDescriptor::setDomainSizePooling(bool domainSizePooling)
{
    Sift::setDomainSizePooling(domainSizePooling);
    update();
}

void SiftCudaDetectorDescriptor::reset()
{
    mSiftGpu.reset(new SiftGPU);
}



} // namespace graphos
