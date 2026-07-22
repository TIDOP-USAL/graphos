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

#include <tidop/geometry/primitives/Point.h>

namespace graphos
{

class Vignetting
{

public:

    enum class Model : uint8_t
    {
        radial,      // DJI
        polynomial2d // Parrot Sequoia
    };

public:

    Vignetting(Model model) : mModel(model) {}
    virtual ~Vignetting() = default;

    virtual auto computeMap(int width, int height) const -> cv::Mat = 0;

    auto model() const -> Model { return mModel; }

private:

    Model mModel;
};

class VignettingRadial
  : public Vignetting
{

public:

    VignettingRadial(const tl::Point2f &center,
                     const std::vector<float> &poly);

public:

    auto center() const -> tl::Point2f { return mCenter; }
    auto polynomial() const -> const std::vector<float> & { return mPolynomial; }

// Vignetting

public:

    auto computeMap(int width, int height) const -> cv::Mat override;

private:

    tl::Point2f mCenter;
    std::vector<float> mPolynomial;
};


class VignettingPolynomial2D
  : public Vignetting
{

public:

    VignettingPolynomial2D(const std::vector<float> &coeffs,
                           const std::vector<std::pair<int, int>> &powers);

public:

    auto coeffs() const -> const std::vector<float> & { return mCoeffs; }
    auto powers() const -> const std::vector<std::pair<int, int>> & { return mPowers; }

// Vignetting

public:

    auto computeMap(int width, int height) const -> cv::Mat override;

private:

    std::vector<float> mCoeffs;
    std::vector<std::pair<int, int>> mPowers;
};


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
//auto vignettingMap(int width,
//                   int height,
//                   float centerX,
//                   float centerY,
//                   const std::vector<float> &k) -> cv::Mat;

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
