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

#ifndef GRAPHOS_CORE_ORIENTATION_COLMAP_H
#define GRAPHOS_CORE_ORIENTATION_COLMAP_H

#include "graphos/core/sfm/orientation.h"
#include "graphos/core/sfm/OrientationReport.h"

#include <memory>
#include <map>
#include <unordered_map>
#include <vector>

#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>

namespace tl
{
class Progress;
}

namespace colmap
{
class Reconstruction;
class ReconstructionManager;
struct IncrementalMapperOptions;
class IncrementalMapperController;
}

namespace graphos
{

class Image;
class Camera;
class GroundPoint;
class CameraPose;
class Calibration;



/*!
 * \brief COLMAP Reconstruction Converter
 *
 * The `ColmapReconstructionConvert` class is responsible for converting a COLMAP reconstruction into GRAPHOS format.
 *
 * It provides methods to access ground points, camera poses, and calibration information.
 */
class ColmapReconstructionConvert
{

public:

    /*!
     * \brief Constructor.
     *
     * Constructs a `ColmapReconstructionConvert` object with the given COLMAP reconstruction and images.
     *
     * \param[in] reconstruction Pointer to the COLMAP reconstruction.
     * \param[in] images Images associated with the reconstruction.
     */
    ColmapReconstructionConvert(const colmap::Reconstruction *reconstruction,
                                const std::vector<Image> &images);

    ~ColmapReconstructionConvert()
    {
    }

    /*!
     * \brief Get the ground points from the reconstruction.
     *
     * \return A vector of ground points extracted from the reconstruction.
     */
    auto groundPoints() const -> std::vector<GroundPoint>;

    /*!
     * \brief Get the camera poses from the reconstruction.
     *
     * \return An unordered map containing image IDs and corresponding camera poses.
     */
    auto cameraPoses() const -> std::unordered_map<size_t, CameraPose>;

    /*!
     * \brief Read calibration information for a specific camera.
     *
     * Reads calibration information for the camera with the specified ID.
     *
     * \param[in] cameraId The ID of the camera.
     * \return A shared pointer to the calibration of the specified camera.
     */
    auto readCalibration(size_t cameraId) const -> std::shared_ptr<Calibration>;

private:

    const colmap::Reconstruction *mReconstruction;
    std::vector<Image> mImages;
    std::unordered_map<uint32_t, size_t> mImageIds;
};



/*!
 * \brief Relative Orientation with COLMAP Properties
 *
 * The `RelativeOrientationColmapProperties` class represents the relative orientation of images with additional
 * properties specific to COLMAP reconstruction. It provides methods to refine focal length, principal point, and
 * extra parameters during relative orientation computation.
 */
//class RelativeOrientationColmapProperties
//  : public RelativeOrientation
//{
//
//public:
//
//    RelativeOrientationColmapProperties();
//    ~RelativeOrientationColmapProperties() override = default;
//
//    /*!
//     * \brief Check if focal length refinement is enabled.
//     *
//     * \return True if focal length refinement is enabled, false otherwise.
//     */
//    virtual auto refineFocalLength() const -> bool;
//
//    /*!
//     * \brief Set whether to enable focal length refinement.
//     *
//     * \param refineFocalLength True to enable focal length refinement, false otherwise.
//     */
//    virtual void setRefineFocalLength(bool refineFocalLength);
//
//    /*!
//     * \brief Check if principal point refinement is enabled.
//     *
//     * \return True if principal point refinement is enabled, false otherwise.
//     */
//    virtual auto refinePrincipalPoint() const -> bool;
//
//    /*!
//     * \brief Set whether to enable principal point refinement.
//     *
//     * \param refinePrincipalPoint True to enable principal point refinement, false otherwise.
//     */
//    virtual void setRefinePrincipalPoint(bool refinePrincipalPoint);
//
//    /*!
//     * \brief Check if extra parameters refinement is enabled.
//     *
//     * \return True if extra parameters refinement is enabled, false otherwise.
//     */
//    virtual auto refineExtraParams() const -> bool;
//
//    /*!
//     * \brief Set whether to enable extra parameters refinement.
//     *
//     * \param refineExtraParams True to enable extra parameters refinement, false otherwise.
//     */
//    virtual void setRefineExtraParams(bool refineExtraParams);
//
//// RelativeOrientation interface
//
//public:
//
//    void reset() override;
//    auto name() const -> QString override;
//
//private:
//
//    bool mRefineFocalLength;
//    bool mRefinePrincipalPoint;
//    bool mRefineExtraParams;
//};
//
//
//
//
///*!
// * \brief Relative Orientation Task with COLMAP Properties
// *
// * The `RelativeOrientationColmapTask` class represents a task for performing relative orientation of images with
// * additional properties specific to COLMAP reconstruction. It inherits from `RelativeOrientationColmapProperties` to
// * leverage COLMAP-specific refinement options and from `tl::TaskBase` to support task execution and progress reporting.
// */
//class RelativeOrientationColmapTask
//  : public RelativeOrientationColmapProperties,
//    public tl::TaskBase
//{
//
//public:
//
//    /*!
//     * \brief Constructor.
//     *
//     * Constructs a `RelativeOrientationColmapTask` object with the given database path, output path, images, cameras,
//     * and calibration fixing option.
//     *
//     * \param[in] database Path to the COLMAP database.
//     * \param[in] outputPath Path to the output directory.
//     * \param[in] images Images to be used for relative orientation.
//     * \param[in] cameras Camera.
//     * \param[in] fixCalibration Flag indicating whether to fix the calibration parameters.
//     */
//    RelativeOrientationColmapTask(tl::Path database,
//                                  tl::Path outputPath,
//                                  const std::vector<Image> &images,
//                                  const std::map<int, Camera> &cameras,
//                                  bool fixCalibration);
//    ~RelativeOrientationColmapTask() override;
//
//    /*!
//     * \brief Get the cameras used in the task.
//     *
//     * \return A map of camera IDs to camera objects.
//     */
//    auto cameras() const -> std::map<int, Camera>;
//
//    /*!
//     * \brief Get the orientation report after task execution.
//     *
//     * \return An `OrientationReport` containing information about the orientation task.
//     */
//    auto report() const -> OrientationReport;
//
//// tl::TaskBase
//
//public:
//
//    void stop() override;
//
//protected:
//
//    void execute(tl::Progress *progressBar = nullptr) override;
//
//private:
//
//    tl::Path mDatabase;
//    tl::Path mOutputPath;
//    std::vector<Image> mImages;
//    std::map<int, Camera> mCameras;
//    colmap::IncrementalMapperOptions *mIncrementalMapper;
//    colmap::IncrementalMapperController *mMapper;
//    std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
//    OrientationReport mOrientationReport;
//
//};
//
//
//
//
//
//class AbsoluteOrientationColmapProperties
//  : public AbsoluteOrientation
//{
//
//public:
//
//    AbsoluteOrientationColmapProperties();
//    ~AbsoluteOrientationColmapProperties() override = default;
//
//    virtual auto minCommonImages() const -> int;
//    virtual void setMinCommonImages(int minCommonImages);
//
//    virtual auto robustAlignment() const -> bool;
//    virtual void setRobustAlignment(bool robustAlignment);
//
//    virtual auto robustAlignmentMaxError() const -> double;
//    virtual void setRobustAlignmentMaxError(double robustAlignmentMaxError);
//
//// AbsoluteOrientation Interface
//
//public:
//
//    void reset() override;
//    auto name() const -> QString override;
//
//private:
//
//    int mMinCommonImages;
//    bool mRobustAlignment;
//    double mRobustAlignmentMaxError;
//};
//
//
//
//
//
//class AbsoluteOrientationColmapTask
//  : public AbsoluteOrientationColmapProperties,
//    public tl::TaskBase
//{
//
//public:
//
//    AbsoluteOrientationColmapTask(tl::Path path,
//                                  const std::vector<Image> &images);
//    ~AbsoluteOrientationColmapTask() override;
//
//    auto cameraPosesErrors() const -> std::unordered_map<size_t, double>;
//
//// tl::TaskBase interface
//
//public:
//
//    void stop() override;
//
//protected:
//
//    void execute(tl::Progress *progressBar) override;
//
//private:
//
//    tl::Path mInputPath;
//    std::vector<Image> mImages;
//    std::unordered_map<size_t, double> mCameraPosesErrors;
//};





class ImportPosesTask
    : public tl::TaskBase
{

public:

    ImportPosesTask(const std::vector<Image> &images,
                    const std::map<int, Camera> &cameras,
                    tl::Path outputPath,
                    tl::Path database,
                    bool fixCalibration = false,
                    bool fixPoses = true);
    ~ImportPosesTask() override;

    auto cameras() const -> std::map<int, Camera>;

    void setFixCalibration(bool fixCalibration);
    void setFixPoses(bool fixPoses);

    /*!
     * \brief Get the orientation report after task execution.
     *
     * \return An `OrientationReport` containing information about the orientation task.
     */
    auto report() const -> OrientationReport;

private:

    void computeOffset();
    void temporalReconstruction(const tl::Path &tempPath);
    void writeImages(const tl::Path &tempPath);
    void writeCameras(const tl::Path &tempPath) const;
    void writePoints(const tl::Path &tempPath);
    auto isCoordinatesLocal() const -> bool;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

private:

    std::vector<Image> mImages;
    std::map<int, Camera> mCameras;
    tl::Path mOutputPath;
    tl::Path mDatabase;
    bool mFixCalibration;
    bool mFixPoses;
    tl::Point3<double> mOffset;
    std::unordered_map<size_t, int> mGraphosToColmapId;
    OrientationReport mOrientationReport;
};





void colmapRemoveOrientations(const std::vector<std::string> &images,
                              const std::string &reconstruction);




} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_COLMAP_H
