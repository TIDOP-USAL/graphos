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

#include <tidop/core/base/Exception.h>

#include <opencv2/opencv.hpp>

namespace graphos
{


VignettingRadial::VignettingRadial(const tl::Point2f &center, 
                                   const std::vector<float> &poly)
  : Vignetting(Model::radial),
    mCenter(center),
    mPolynomial(poly)
{
}

auto VignettingRadial::computeMap(int width, int height) const -> cv::Mat
{
    cv::Mat map(height, width, CV_32F);

    for (int y = 0; y < height; ++y) {
        float dy = y - mCenter.y();

        for (int x = 0; x < width; ++x) {

            float dx = x - mCenter.x();
            float r = std::hypot(dx, dy);

            // Calcular el factor polinómico
            float factor = 1.0f;
            float r_pow = r;

            for (float k : mPolynomial) {
                factor += k * r_pow;
                r_pow *= r; // siguiente potencia
            }

            map.at<float>(y, x) = factor;
        }
    }
    return map;
}



//auto vignettingMap(int width,
//                   int height,
//                   float centerX, 
//                   float centerY,
//                   const std::vector<float> &k) -> cv::Mat
//{
//
//    TL_ASSERT(k.size() >= 1, "At least one coefficient is required for the vignetting model.");
//
//    cv::Mat vignette_factor(height, width, CV_32F);
//
//    for (int y = 0; y < height; ++y) {
//        float dy = y - centerY;
//        for (int x = 0; x < width; ++x) {
//            float dx = x - centerX;
//            float r = std::hypot(dx, dy);
//            // Calcular el factor polinómico
//            float factor = 1.0;
//            float r_pow = r;
//            for (size_t i = 0; i < k.size(); ++i) {
//                factor += k[i] * r_pow;
//                r_pow *= r; // siguiente potencia
//            }
//
//            vignette_factor.at<float>(y, x) = factor;
//        }
//    }
//
//    return vignette_factor;
//}



VignettingPolynomial2D::VignettingPolynomial2D(const std::vector<float> &coeffs, 
                                               const std::vector<std::pair<int, int>> &powers)
  : Vignetting(Model::polynomial2d), 
    mCoeffs(coeffs),
    mPowers(powers)
{
}

// No esta bien...
auto VignettingPolynomial2D::computeMap(int width, int height) const -> cv::Mat
{
    cv::Mat map(height, width, CV_32F);

    float cx = width / 2.f;
    float cy = height / 2.f;

    for (int y = 0; y < height; ++y) {

        float dy = (y - cy);

        for (int x = 0; x < width; ++x) {

            float dx = (x - cx);

            float value = 0.f;
            for (size_t i = 0; i < mCoeffs.size(); ++i) {
                int px = mPowers[i].first;
                int py = mPowers[i].second;
                value += mCoeffs[i] * static_cast<float>(std::pow(dx, px) * std::pow(dy, py));
            }

            map.at<float>(y, x) = value;
        }
    }
    return map;
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
