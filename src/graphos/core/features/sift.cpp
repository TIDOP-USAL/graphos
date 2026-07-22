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

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>

#include <colmap/util/opengl_utils.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/thirdparty/SiftGPU/SiftGPU.h>

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

//auto Sift::domainSizePooling() const -> bool
//{
//    return mDomainSizePooling;
//}

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

//void Sift::setDomainSizePooling(bool domainSizePooling)
//{
//    mDomainSizePooling = domainSizePooling;
//}

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
                                                       double contrastThreshold/*,
                                                       bool domainSizePooling*/)
{
    Sift::setFeaturesNumber(featuresNumber);
    Sift::setOctaveLayers(octaveLayers);
    if (contrastThreshold > 0.) {
        Sift::setContrastThresholdAuto(false);
        Sift::setContrastThreshold(contrastThreshold);
    }
    Sift::setEdgeThreshold(edgeThreshold);
    //Sift::setDomainSizePooling(domainSizePooling);
    update();
}

SiftCudaDetectorDescriptor::~SiftCudaDetectorDescriptor() = default;

void SiftCudaDetectorDescriptor::update()
{
    colmap::FeatureExtractionOptions options;
    options.sift = std::make_shared<colmap::SiftExtractionOptions>();
    options.sift->max_num_features = Sift::featuresNumber();
    options.sift->octave_resolution = Sift::octaveLayers();
    options.sift->edge_threshold = Sift::edgeThreshold();
    options.sift->peak_threshold = Sift::contrastThreshold();
    //options.domain_size_pooling = Sift::domainSizePooling();
    options.use_gpu = true;

    std::lock_guard<std::mutex> lck(mMutex);

    mSiftGpu.reset(new SiftGPU);

    std::vector<int> gpu_indices = colmap::CSVToVector<int>(options.gpu_index);
    TL_ASSERT(gpu_indices.size() == 1, "SiftGPU can only run on one GPU");

    std::vector<std::string> sift_gpu_args;

    sift_gpu_args.push_back("./sift_gpu");

#if defined(COLMAP_CUDA_ENABLED)
    // Use CUDA version by default if darkness adaptivity is disabled.
    if (!options.sift->darkness_adaptivity && gpu_indices[0] < 0) {
        gpu_indices[0] = 0;
    }

    if (gpu_indices[0] >= 0) {
        sift_gpu_args.push_back("-cuda");
        sift_gpu_args.push_back(std::to_string(gpu_indices[0]));
    }
#endif  // COLMAP_CUDA_ENABLED

    // Darkness adaptivity (hidden feature). Significantly improves
    // distribution of features. Only available in GLSL version.
    //if (options.sift->darkness_adaptivity) {
    //    if (gpu_indices[0] >= 0) {
    //        WarnDarknessAdaptivityNotAvailable();
    //    }
    //    sift_gpu_args.push_back("-da");
    //}

    // No verbose logging.
    sift_gpu_args.push_back("-v");
    sift_gpu_args.push_back("0");

    // Set maximum image dimension.
    // Note the max dimension of SiftGPU is the maximum dimension of the
    // first octave in the pyramid (which is the 'first_octave').
    const int compensation_factor = 1
        << -std::min(0, options.sift->first_octave);
    sift_gpu_args.push_back("-maxd");
    sift_gpu_args.push_back(
        std::to_string(options.EffMaxImageSize() * compensation_factor));

    // Keep the highest level features.
    sift_gpu_args.push_back("-tc2");
    sift_gpu_args.push_back(std::to_string(options.sift->max_num_features));

    // First octave level.
    sift_gpu_args.push_back("-fo");
    sift_gpu_args.push_back(std::to_string(options.sift->first_octave));

    // Number of octave levels.
    sift_gpu_args.push_back("-d");
    sift_gpu_args.push_back(std::to_string(options.sift->octave_resolution));

    // Peak threshold.
    sift_gpu_args.push_back("-t");
    sift_gpu_args.push_back(std::to_string(options.sift->peak_threshold));

    // Edge threshold.
    sift_gpu_args.push_back("-e");
    sift_gpu_args.push_back(std::to_string(options.sift->edge_threshold));

    if (options.sift->upright) {
        // Fix the orientation to 0 for upright features.
        sift_gpu_args.push_back("-ofix");
        // Maximum number of orientations.
        sift_gpu_args.push_back("-mo");
        sift_gpu_args.push_back("1");
    } else {
        // Maximum number of orientations.
        sift_gpu_args.push_back("-mo");
        sift_gpu_args.push_back(
            std::to_string(options.sift->max_num_orientations));
    }

    std::vector<const char *> sift_gpu_args_cstr;
    sift_gpu_args_cstr.reserve(sift_gpu_args.size());
    for (const auto &arg : sift_gpu_args) {
        sift_gpu_args_cstr.push_back(arg.c_str());
    }

    //auto extractor = std::make_unique<SiftGPUFeatureExtractor>(options);

    // Note that the SiftGPU object is not movable (for whatever reason).
    // If we instead create the object here and move it to the constructor, the
    // program segfaults inside SiftGPU.

    mSiftGpu->ParseParam(sift_gpu_args_cstr.size(),
                         sift_gpu_args_cstr.data());

    mSiftGpu->gpu_index = gpu_indices[0];
    //if (sift_gpu_mutexes_.count(gpu_indices[0]) == 0) {
    //    sift_gpu_mutexes_.emplace(gpu_indices[0], std::make_unique<std::mutex>());
    //}

    TL_ASSERT(mSiftGpu->VerifyContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED, "Error creating SiftGPUExtractor");
}

void SiftCudaDetectorDescriptor::run(const cv::Mat &bitmap,
                                     std::vector<cv::KeyPoint> &keyPoints,
                                     cv::Mat &descriptors)
{
    std::lock_guard<std::mutex> lck(mMutex);

    try {

        TL_ASSERT(bitmap.type() == CV_8UC1, "SiftCudaDetectorDescriptor supports only 8-bit, single-channel images.");

        update();

        int err = mSiftGpu->RunSIFT(bitmap.cols,
                                    bitmap.rows, 
                                    bitmap.data, 
                                    GL_LUMINANCE, 
                                    GL_UNSIGNED_BYTE);

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

//void SiftCudaDetectorDescriptor::setDomainSizePooling(bool domainSizePooling)
//{
//    Sift::setDomainSizePooling(domainSizePooling);
//    update();
//}

void SiftCudaDetectorDescriptor::reset()
{
    mSiftGpu.reset(new SiftGPU);
}



} // namespace graphos
