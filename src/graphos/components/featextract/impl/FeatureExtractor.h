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

#include <opencv2/features2d.hpp>

namespace graphos
{

class FeatureExtractor
{

public:

    FeatureExtractor() = default;
    virtual ~FeatureExtractor() = default;

    /*!
     * \brief Detect keypoints and extract descriptors
     * \param[in] bitmap Image
     * \param[out] keyPoints Detected keypoints
     * \param[out] descriptors Computed descriptors
     */
    virtual void run(const cv::Mat &bitmap,
                     std::vector<cv::KeyPoint> &keyPoints,
                     cv::Mat &descriptors) = 0;

};

} // namespace graphos