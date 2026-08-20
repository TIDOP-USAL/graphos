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

#include <opencv2/core/mat.hpp>

#include <tidop/geometry/spatial/BoundingBox.h>

#include "graphos/components/orthophoto/impl/Orthorectification.h"

namespace graphos
{

/*!
 * \class ZBuffer
 * \brief Implements a Z-buffer projection algorithm for orthophoto generation.
 *
 * This class computes the correspondence between the Digital Surface Model (DSM)
 * and the projected image pixels, determining for each orthorectified pixel the
 * shortest distance to the camera projection center.
 *
 * The result is a distance map (Z-buffer) together with auxiliary maps that
 * preserve the relationship between terrain coordinates and image coordinates.
 *
 * \details
 * The algorithm iterates over the DTM and projects each terrain cell into the
 * image space using the camera orientation parameters and the georeference
 * transform of the orthophoto. In case of overlapping projections, the algorithm
 * keeps the value corresponding to the smallest distance to the camera center.
 * This ensures that, for each orthophoto pixel, the visible (nearest) point is used.
 *
 * \see Orthorectification, Orthoimage, OrthophotoTask
 */
class ZBuffer
{

public:

    /*!
     * \brief Constructs a ZBuffer instance.
     * \param[in] orthorectification Pointer to the Orthorectification object providing camera and DSM access.
     * \param[in] sizeOrtho Image-space Size defining the orthophoto area.
     * \param[in] georeference Affine transform relating terrain and image coordinates.
     */
    ZBuffer(Orthorectification *orthorectification,
            const tl::Size<int> &sizeOrtho,
            const tl::Affine<double, 2> &georeference);

    ~ZBuffer();

    /*!
     * \brief Runs the Z-buffer computation.
     *
     * Iterates over the DSM and computes the minimum distance between the terrain surface 
     * and the camera projection center. The resulting distance maps are stored internally.
     *
     * \throws tl::Exception If a projection or data access error occurs.
     */
    void run();

    //! \brief Returns the computed distance map (Z-buffer).
    auto distances() const -> cv::Mat;

    //! \brief Returns the auxiliary map containing DTM X indices for each orthophoto pixel.
    auto mapX() const -> cv::Mat;

    //! \brief Returns the auxiliary map containing DTM Y indices for each orthophoto pixel.
    auto mapY() const -> cv::Mat;

    //! \brief Clears internal matrices and releases memory.
    void clear();

private:

    Orthorectification *mOrthorectification;
    tl::Rect<int> mRectOrtho;
    tl::Affine<double, 2> mEnuGeoreference;
    tl::BoundingBox2d mWindowOrthoTerrain;
    cv::Mat mDistances;
    cv::Mat mY;
    cv::Mat mX;

};


} // namespace graphos
