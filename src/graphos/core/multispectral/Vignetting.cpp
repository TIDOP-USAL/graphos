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

#include "graphos/core/multispectral/Vignetting.h"

#include <tidop/core/exception.h>

#include <opencv2/opencv.hpp>

namespace graphos
{

auto vignettingMap(int width,
                   int height,
                   float centerX, 
                   float centerY,
                   const std::vector<float> &k) -> cv::Mat
{

    TL_ASSERT(k.size() >= 1, "At least one coefficient is required for the vignetting model.");

    cv::Mat vignette_factor(height, width, CV_32F);

    for (int y = 0; y < height; ++y) {
        float dy = y - centerY;
        for (int x = 0; x < width; ++x) {
            float dx = x - centerX;
            float r = std::hypot(dx, dy);
            // Calcular el factor polinómico
            float factor = 1.0;
            float r_pow = r;
            for (size_t i = 0; i < k.size(); ++i) {
                factor += k[i] * r_pow;
                r_pow *= r; // siguiente potencia
            }

            vignette_factor.at<float>(y, x) = factor;
        }
    }

    return vignette_factor;
}

auto correctVignetting(const cv::Mat &inputImage, const cv::Mat &vignettingMap) -> cv::Mat
{
    TL_ASSERT(inputImage.channels() == 1, "Only single-channel images are supported for vignetting correction.");

    cv::Mat corrected = inputImage.clone();

    // Aplicar la corrección: I_corr = I * factor
    corrected = corrected.mul(vignettingMap);

    return corrected;
}


} // namespace graphos
