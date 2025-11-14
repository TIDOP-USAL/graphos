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

#ifndef GRAPHOS_CORE_VIGNETTING_H
#define GRAPHOS_CORE_VIGNETTING_H

#include <opencv2/opencv.hpp>

namespace graphos
{

/*!
 * \brief Generates a vignetting map based on the DJI Mavic 3M model.
 *
 * \param[in] width Image width.
 * \param[in] height Image height.
 * \param[in] centerX X coordinate of the calibrated optical center.
 * \param[in] centerY Y coordinate of the calibrated optical center.
 * \param[in] k Polynomial coefficients [k0, k1, ..., k5] of the vignetting.
 * \return Vignetting map.
 */
auto vignettingMap(int width,
                   int height,
                   float centerX,
                   float centerY,
                   const std::vector<float> &k) -> cv::Mat;

/*!
 * \brief Apply vignette correction according to the DJI Mavic 3M model.
 * 
 * \param[in] inputImage Original image
 * \param[in] vignettingMap Iignetting map
 * \return Corrected image
 */
auto correctVignetting(const cv::Mat &inputImage,
                       const cv::Mat &vignettingMap) -> cv::Mat;

} // namespace graphos

#endif // GRAPHOS_CORE_VIGNETTING_H
