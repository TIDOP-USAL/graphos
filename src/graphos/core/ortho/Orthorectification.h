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

#ifndef GRAPHOS_CORE_ORTHO_ORTHORECTIFICATION_H
#define GRAPHOS_CORE_ORTHO_ORTHORECTIFICATION_H

#include <memory>

#include <opencv2/core/mat.hpp>

#include <tidop/core/path.h>
#include <tidop/img/imgreader.h>
#include <tidop/geometry/transform/affine.h>
#include <tidop/geometry/rect.h>
#include <tidop/geospatial/diffrect.h>
#include <tidop/graphic/entities/polygon.h>

#include "graphos/core/camera/Camera.h"
#include "graphos/core/sfm/poses.h"

namespace graphos
{

/*!
 * \brief Orthorectification
 */
class Orthorectification
{

public:

    Orthorectification(const tl::Path &dtm,
                       const Camera &camera,
                       const CameraPose &cameraPose);

    ~Orthorectification()
    {

    }

    tl::Point<int> terrainToImage(const tl::Point3<double> &terrainPoint) const;
    tl::Point<int> terrainToPhotocoordinates(const tl::Point3<double> &terrainPoint) const;
    tl::Point3<double> imageToTerrain(const tl::Point<int> &imagePoint) const;
    tl::Point3<double> photocoordinatesToTerrain(const tl::Point<int> &photocoordinates) const;
    tl::Point<int> imageToPhotocoordinates(const tl::Point<int> &imagePoint) const;
    tl::Point<int> photocoordinatesToImage(const tl::Point<int> &photocoordinates) const;
    tl::Point3<double> dtmToTerrain(const tl::Point<int> &imagePoint) const;
    tl::Point<int> terrainToDTM(const tl::Point3<double> &terrainPoint) const;
    double z(const tl::Point<double> &terrainPoint) const;

    tl::Rect<int> rectImage() const;
    tl::Rect<int> rectDtm() const;
    tl::GPolygon footprint() const;

    CameraPose orientation() const;
    Camera camera() const;
    Camera undistortCamera() const;

    bool hasNodataValue() const;
    double nodataValue() const;

    cv::Mat undistort(const cv::Mat &image);

    bool isValid() const;

    void setCuda(bool active);

private:

    void init();
    void initUndistortCamera();

    float focal() const;
    tl::Point<float> principalPoint() const;
    cv::Mat distCoeffs() const;

private:

    Camera mCamera;
    Camera mUndistortCamera;
    CameraPose mCameraPose;
    std::unique_ptr<tl::ImageReader> mDtmReader;
    cv::Mat mDtm;
    tl::Window<tl::Point<double>> mWindowDtmTerrainExtension;
    tl::Affine<tl::Point<int>> mAffineImageToPhotocoordinates;
    tl::Affine<tl::Point<double>> mAffineDtmImageToTerrain;
    std::unique_ptr<tl::DifferentialRectification> mDifferentialRectification;
    double mIniZ;
    tl::Rect<int> mRectImage;
    tl::Rect<int> mRectDtm;
    tl::GPolygon mFootprint;
    double mNoDataValue;
    bool bCuda;
};


} // namespace graphos

#endif // GRAPHOS_CORE_ORTHO_ORTHORECTIFICATION_H
