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
#include <tidop/math/geometry/affine.h>
#include <tidop/geometry/rect.h>
#include <tidop/geospatial/diffrect.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/graphic/entities/polygon.h>

#include "graphos/core/camera/Camera.h"
#include "graphos/core/sfm/poses.h"



namespace graphos
{

/*! Forward declaration */
class Undistort;

/*!
 * \class Orthorectification
 * \brief Provides geometric transformations and tools for orthorectifying aerial or terrestrial images.
 *
 * This class performs the geometric projection between terrain (DSM) coordinates
 * and image coordinates using the camera orientation parameters and the digital
 * surface model (DSM). It supports both forward and inverse projections,
 * image undistortion, and DSM window management.
 *
 * The coordinate reference system follows the local ENU (East-North-Up) system
 * defined for the project. The class is designed to be used by higher-level
 * modules such as Orthoimage and OrthophotoTask.
 *
 * \see ZBuffer, Orthoimage, OrthophotoTask
 */
class Orthorectification
{

public:

    /*!
     * \brief Constructs an Orthorectification object.
     * \param[in] dsm Path to the Digital Surface Model (DSM) file.
     * \param[in] cameraPose Camera pose including position and orientation in ENU coordinates.
     * \param[in] undistortedCamera Undistorted camera.
     * \param[in] zIni Optional initial elevation value.
     */
    Orthorectification(const tl::Path &dsm,
                       CameraPose cameraPose,
                       Camera undistortedCamera,
                       double zIni = 0.);

    ~Orthorectification() = default;

    /*!
     * \brief Projects a 3D terrain point (ENU) into image pixel coordinates.
     * \param[in] terrainPoint Terrain point in ENU coordinates.
     * \return Pixel coordinates in the image.
     */
    auto terrainToImage(const tl::Point3<double> &terrainPoint) const -> tl::Point<int>;
    

    /*!
     * \brief Projects a 3D terrain point (ENU) into photo (camera) coordinates.
     * \param[in] terrainPoint Terrain point in ENU coordinates.
     * \return Point in photo (camera) coordinate system.
     */
    auto terrainToPhotoCoordinates(const tl::Point3<double> &terrainPoint) const -> tl::Point<double>;

    /*!
     * \brief Back-projects a pixel from the image into terrain space using the DSM.
     * \param[in] imageCoordinates Pixel coordinates in the image.
     * \return Corresponding terrain point in ENU coordinates.
     */
    auto imageToTerrain(const tl::Point<int> &imageCoordinates) const -> tl::Point3<double>;

    /*!
     * \brief Back-projects photo (camera) coordinates into terrain coordinates using the DSM.
     * \param[in] photocoordinates Point in photo (camera) coordinates.
     * \return Corresponding terrain point in ENU coordinates.
     */
    auto photocoordinatesToTerrain(const tl::Point<double> &photocoordinates) const -> tl::Point3<double>;

    /*!
     * \brief Converts image pixel coordinates to photo (camera) coordinates.
     * \param[in] imagePoint Pixel coordinates in the image.
     * \return Corresponding photo (camera) coordinates.
     */
    auto imageToPhotocoordinates(const tl::Point<int> &imagePoint) const -> tl::Point<double>;

    /*!
     * \brief Converts photo (camera) coordinates to image pixel coordinates.
     * \param[in] photocoordinates Point in photo (camera) coordinates.
     * \return Pixel coordinates in the image.
     */
    auto photoCoordinatesToImageCoordinates(const tl::Point<double> &photocoordinates) const -> tl::Point<double>;

    /*!
     * \brief Converts DSM image coordinates to terrain (ENU) coordinates.
     * \param[in] imagePoint Pixel coordinates in the DSM image.
     * \return Corresponding terrain point in ENU coordinates.
     */
    auto dsmImageCoordinatesToTerrain(const tl::Point<int> &imagePoint) const -> tl::Point3<double>;

    /*!
     * \brief Converts terrain (ENU) coordinates to DSM image coordinates.
     * \param[in] terrainPoint Terrain point in ENU coordinates.
     * \return Pixel coordinates in the DSM image.
     */
    auto terrainToDsmImageCoordinates(const tl::Point3<double> &terrainPoint) const -> tl::Point<int> ;

    /*!
     * \brief Returns the DSM elevation (Z) at the given terrain coordinates.
     * \param[in] terrainPoint Terrain point in ENU coordinates.
     * \return Elevation value (Z) from the DSM.
     */
    auto z(const tl::Point<double> &terrainPoint) const -> double;

    /*!
     * \brief Returns the image-space rectangle (in pixels) of the input image.
     * \return Rectangle in image coordinates.
     */
    auto rectImage() const -> tl::Rect<int>;

    /*!
     * \brief Returns the image-space rectangle (in pixels) of the DSM.
     * \return Rectangle in DSM image coordinates.
     */
    auto rectDsm() const -> tl::Rect<int>;

    /*!
     * \brief Returns the terrain window (ENU) covered by the DSM used for the current camera.
     * \return Terrain window in ENU coordinates.
     */
    auto windowDsm() const -> tl::Window<tl::Point<double>>;

    /*!
     * \brief Returns the ground footprint polygon of the projected image.
     * \return Footprint polygon in ENU coordinates.
     */
    auto footprint() const -> tl::GPolygon;

    /*!
     * \brief Returns the current camera pose (position and orientation).
     * \return Camera pose in ENU coordinates.
     */
    auto orientation() const -> CameraPose;

    /*!
     * \brief Returns the undistorted camera model.
     * \return Undistorted camera model.
     */
    auto undistortCamera() const -> Camera;

    /*!
     * \brief Checks if the DSM contains nodata values.
     * \return True if nodata values are present, false otherwise.
     */
    auto hasNodataValue() const -> bool;

    /*!
     * \brief Returns the nodata value of the DSM.
     * \return Nodata value.
     */
    auto nodataValue() const -> double;

    /*!
     * \brief Checks if the orthorectification setup is valid.
     * \return True if valid, false otherwise.
     */
    auto isValid() const -> bool;

    /*!
     * \brief Enables or disables CUDA-based processing (if available).
     * \param[in] active True to enable CUDA, false to disable it.
     */
    void setCuda(bool active);

private:

    void init();

    auto focal() const -> float;
    auto principalPoint() const -> tl::Point<float>;

private:

    tl::Path mDsmPath;
    CameraPose mCameraPose;
    Camera mUndistortedCamera;
    cv::Mat mDsm;
    tl::Window<tl::Point<double>> mWindowDsmTerrainExtension;
    tl::Affine<double, 2> mAffineImageToPhotocoordinates;
    tl::Affine<double, 2> mAffinePhotocoordinatesToImage;
    tl::Affine<double, 2> mAffineDsmImageToTerrain;
    tl::Affine<double, 2> mAffineTerrainToDsmImage;
    std::unique_ptr<tl::DifferentialRectification> mDifferentialRectification;
    double mIniZ;
    tl::Rect<int> mRectImage;
    tl::Rect<int> mRectDsm;
    tl::GPolygon mFootprint;
    double mNoDataValue;
    bool bCuda;
};


} // namespace graphos

#endif // GRAPHOS_CORE_ORTHO_ORTHORECTIFICATION_H
