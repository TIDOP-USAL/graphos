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

#include "graphos/components/featextract/impl/SiftCPUExtractor.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>

#include <opencv2/imgcodecs.hpp>

namespace graphos
{

/* SiftCPUExtractor */


SiftCPUExtractor::SiftCPUExtractor(const SiftProperties &config)
{
    updateConfig(config);
}

void SiftCPUExtractor::run(const cv::Mat &bitmap,
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
        TL_COMPILER_WARNING("OpenCV not built with extra modules. SiftProperties Detector/Descriptor not supported")
        throw TL_ERROR("OpenCV not built with extra modules. SiftProperties Detector/Descriptor not supported");
#endif // HAVE_OPENCV_XFEATURES2D



#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4)) || (defined HAVE_OPENCV_XFEATURES2D && defined OPENCV_ENABLE_NONFREE)
        mSift->compute(bitmap, opencv_key_points, opencv_descriptors);
#else
        TL_COMPILER_WARNING("OpenCV not built with extra modules. SiftProperties Detector/Descriptor not supported")
        throw TL_ERROR("OpenCV not built with extra modules. SiftProperties Detector/Descriptor not supported");
#endif // HAVE_OPENCV_XFEATURES2D

        keyPoints = opencv_key_points;
        descriptors = opencv_descriptors;

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void SiftCPUExtractor::updateConfig(const SiftProperties &config)
{
#if (CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 4))
    mSift = cv::SIFT::create(config.featuresNumber(),
                             config.octaveLayers(),
                             config.contrastThreshold(),
                             config.edgeThreshold());
#elif defined HAVE_OPENCV_XFEATURES2D && defined OPENCV_ENABLE_NONFREE
    mSift = cv::xfeatures2d::SIFT::create(config.featuresNumber(),
                                          config.octaveLayers(),
                                          config.contrastThreshold(),
                                          config.edgeThreshold());
#endif
}

} // namespace graphos
