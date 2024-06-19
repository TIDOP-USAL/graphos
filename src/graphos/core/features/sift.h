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


class SiftGPU;

namespace graphos
{


/*!
 * \brief Class representing properties for SIFT detector/descriptor.
 *
 * This class encapsulates properties related to the SIFT (Scale-Invariant Feature Transform) detector/descriptor.
 */
class Sift
  : public Feature
{

public:

    /*!
     * \brief Default constructor for SIFT properties.
     * Constructs a Sift object with default values.
     */
    Sift();

    /*!
     * \brief Copy constructor for SIFT properties.
     * Constructs a new Sift object by copying from another Sift object.
     * \param[in] sift The Sift object to copy from.
     */
    Sift(const Sift &sift);

    /*!
     * \brief Destructor for SIFT.
     */
    ~Sift() override = default;

public:

    /*!
     * \brief Get the number of features for SIFT.
     * The features are ranked by their scores (measured in
     * SIFT algorithm as the local contrast)
     * \return The number of best features to retain.
     */
    virtual auto featuresNumber() const -> int;

    /*!
     * \brief Get the number of layers per octave.
     * 3 is the value used in D. Lowe paper. The number of octaves is computed
     * automatically from the image resolution.
     * \return The number of layers per octave.
     */
    virtual auto octaveLayers() const -> int;

    /*!
     * \brief Check if automatic contrast thresholding is enabled.
     * \return True if automatic contrast thresholding is enabled, false otherwise.
     */
    virtual auto constrastThresholdAuto() const -> bool;

    /*!
     * \brief Get the contrast threshold used to filter out weak features in semi-uniform (low-contrast) regions.
     * The larger the threshold, the less features are produced by the detector.
     * \return The contrast threshold value.
     */
    virtual auto contrastThreshold() const -> double;

    /*!
     * \brief Get the edge threshold value for SIFT.
     * \return The edge threshold value.
     */
    virtual auto edgeThreshold() const -> double;

    virtual auto domainSizePooling() const -> bool;

    /*!
     * \brief Set the number of best features to retain
     * \param[in] featuresNumber The number of features.
     */
    virtual void setFeaturesNumber(int featuresNumber);

    /*!
     * \brief Set the number of layers per octave.
     * \param[in] octaveLayers The number of layers per octave (3 by default).
     */
    virtual void setOctaveLayers(int octaveLayers);

    /*!
     * \brief Enable or disable automatic contrast thresholding.
     * \param[in] active True to enable automatic contrast thresholding, false to disable it.
     */
    virtual void setContrastThresholdAuto(bool active);

    /*!
     * \brief Set the contrast threshold value.
     * \param[in] contrastThreshold The contrast threshold value.
     */
    virtual void setContrastThreshold(double contrastThreshold);

    /*!
     * \brief Set the threshold used to filter out edge-like features
     * \param[in] edgeThreshold The edge threshold value.
     */
    virtual void setEdgeThreshold(double edgeThreshold);

    virtual void setDomainSizePooling(bool domainSizePooling);

// Feature interface methods

public:

    void reset() override;
    auto name() const -> QString final;

private:

    int mFeaturesNumber;
    int mOctaveLayers;
    bool mContrastThresholdAuto;
    double mContrastThreshold;
    double mEdgeThreshold;
    bool mDomainSizePooling;
};




/*!
 * \brief Class for SIFT feature extraction using CPU.
 *
 * This class provides functionality for detecting and describing SIFT (Scale-Invariant Feature Transform) features
 * using CPU-based computation. It inherits properties related to SIFT from the Sift class and implements
 * feature extraction functionality from the FeatureExtractor interface.
 */
class SiftCPUDetectorDescriptor
  : public Sift,
    public FeatureExtractor
{

public:

    /*!
     * \brief Default constructor for SIFT CPU-based detector/descriptor.
     * Constructs an SiftCPUDetectorDescriptor object with default values.
     */
    SiftCPUDetectorDescriptor();

    /*!
     * \brief Copy constructor for SIFT CPU-based detector/descriptor.
     * Constructs a new SiftCPUDetectorDescriptor object by copying from another SiftCPUDetectorDescriptor object.
     * \param[in] sift The SiftCPUDetectorDescriptor object to copy from.
     */
    SiftCPUDetectorDescriptor(const SiftCPUDetectorDescriptor &sift);

    /*!
     * \brief Parameterized constructor for SIFT CPU-based detector/descriptor.
     * Constructs an SiftCPUDetectorDescriptor object with provided parameters.
     * \param[in] featuresNumber The number of features.
     * \param[in] octaveLayers The number of layers per octave.
     * \param[in] edgeThreshold The edge threshold value.
     * \param[in] contrastThreshold The contrast threshold value (default is 0.0).
     */
    SiftCPUDetectorDescriptor(int featuresNumber,
                              int octaveLayers,
                              double edgeThreshold,
                              double contrastThreshold = 0.);

    /*!
     * \brief Destructor for SIFT CPU-based detector/descriptor.
     */
    ~SiftCPUDetectorDescriptor() override;

private:

    void update();

// FeatureExtractor interface

public:

    /*!
     * \brief Runs the SIFT feature extraction on the provided image.
     * Overrides the run method from the FeatureExtractor interface.
     * \param[in] bitmap The input image for feature extraction.
     * \param[out] keyPoints Detected keypoints will be stored here.
     * \param[out] descriptors Computed descriptors will be stored here.
     */
    void run(const cv::Mat &bitmap,
             std::vector<cv::KeyPoint> &keyPoints,
             cv::Mat &descriptors) override;

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
    std::mutex mMutex;
};





/*!
 * \brief Class for CUDA-accelerated SIFT feature extraction.
 *
 * This class provides functionality for detecting and describing SIFT (Scale-Invariant Feature Transform) features
 * using CUDA-accelerated computation. It inherits properties related to SIFT from the Sift class and implements
 * feature extraction functionality from the FeatureExtractor interface.
 */
class SiftCudaDetectorDescriptor
  : public Sift,
    public FeatureExtractor
{

public:

    /*!
     * \brief Default constructor for CUDA-accelerated SIFT detector/descriptor.
     * Constructs an SiftCudaDetectorDescriptor object with default values.
     */
    SiftCudaDetectorDescriptor();

    /*!
     * \brief Copy constructor for CUDA-accelerated SIFT detector/descriptor.
     * Constructs a new SiftCudaDetectorDescriptor object by copying from another SiftCudaDetectorDescriptor object.
     * \param[in] sift The SiftCudaDetectorDescriptor object to copy from.
     */
    SiftCudaDetectorDescriptor(const SiftCudaDetectorDescriptor &sift);

    /*!
     * \brief Parameterized constructor for CUDA-accelerated SIFT detector/descriptor.
     * Constructs an SiftCudaDetectorDescriptor object with provided parameters.
     * \param[in] featuresNumber The number of features.
     * \param[in] octaveLayers The number of layers per octave.
     * \param[in] edgeThreshold The edge threshold value.
     * \param[in] contrastThreshold The contrast threshold value (default is 0.0).
     */
    SiftCudaDetectorDescriptor(int featuresNumber,
                               int octaveLayers,
                               double edgeThreshold,
                               double contrastThreshold = 0.,
                               bool domainSizePooling = true);
    /*!
     * \brief Destructor for CUDA-accelerated SIFT detector/descriptor.
     */
    ~SiftCudaDetectorDescriptor() override;

private:

    void update();

// FeatureExtractor interface

public:

    /*!
     * \brief Runs the CUDA-accelerated SIFT feature extraction on the provided image.
     * Overrides the run method from the FeatureExtractor interface.
     * \param[in] bitmap The input image for feature extraction.
     * \param[out] keyPoints Detected keypoints will be stored here.
     * \param[out] descriptors Computed descriptors will be stored here.
     */
    void run(const cv::Mat &bitmap,
             std::vector<cv::KeyPoint> &keyPoints,
             cv::Mat &descriptors) override;

// Sift interface

public:

    void setFeaturesNumber(int featuresNumber) override;
    void setOctaveLayers(int octaveLayers) override;
    void setContrastThreshold(double contrastThreshold) override;
    void setEdgeThreshold(double edgeThreshold) override;
    void setDomainSizePooling(bool domainSizePooling) override;

// Feature interface

public:

    void reset() override;

protected:

    std::unique_ptr<SiftGPU> mSiftGpu;
    std::mutex mMutex;
};


} // namespace graphos

#endif // GRAPHOS_CORE_SIFT_DETECTOR_DESCRIPTOR_H
