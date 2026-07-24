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

#pragma once

#include "graphos/graphos_global.h"

#include "graphos/core/features/Features.h"
#include "graphos/core/features/Sift.h"

#include <opencv2/features2d.hpp>
#if defined OPENCV_ENABLE_NONFREE && defined HAVE_OPENCV_XFEATURES2D
#include <opencv2/xfeatures2d.hpp>
#endif // HAVE_OPENCV_XFEATURES2D

namespace graphos
{



/*!
 * \brief Class for SIFT feature extraction using CPU.
 *
 * This class provides functionality for detecting and describing SIFT (Scale-Invariant Feature Transform) features
 * using CPU-based computation. It inherits properties related to SIFT from the Sift class and implements
 * feature extraction functionality from the FeatureExtractor interface.
 */
class SiftCPUExtractor
  : public FeatureExtractor
{

public:

    /*!
     * \brief Default constructor for SIFT CPU-based detector/descriptor.
     * Constructs an SiftCPUExtractor object with default values.
     */
    SiftCPUExtractor(const Sift &config);

    /*!
     * \brief Destructor for SIFT CPU-based detector/descriptor.
     */
    ~SiftCPUExtractor() override = default;

    TL_DISABLE_COPY(SiftCPUExtractor)
    TL_DISABLE_MOVE(SiftCPUExtractor)

    void updateConfig(const Sift &config);

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

protected:

#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4))
    cv::Ptr <cv::SIFT>  mSift;
#elif defined OPENCV_ENABLE_NONFREE && defined HAVE_OPENCV_XFEATURES2D
    cv::Ptr<cv::xfeatures2d::SIFT> mSift;
#endif // HAVE_OPENCV_XFEATURES2D
    std::mutex mMutex;
};


} // namespace graphos
