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
class BundleAdjustmentController;
}

namespace graphos
{

class Image;
class Camera;
class GroundPoint;
class CameraPose;
class Calibration;

class ColmapReconstructionConvert
{

public:

    ColmapReconstructionConvert(const colmap::Reconstruction *reconstruction,
                                const std::vector<Image> &images);

    ~ColmapReconstructionConvert()
    {
    }

    std::vector<GroundPoint> groundPoints() const;
    std::unordered_map<size_t, CameraPose> cameraPoses() const;
    std::shared_ptr<Calibration> readCalibration(size_t cameraId) const;

private:

    const colmap::Reconstruction *mReconstruction;
    std::vector<Image> mImages;
    std::unordered_map<uint32_t, size_t> mImageIds;
};


class RelativeOrientationColmapProperties
  : public RelativeOrientation
{

public:

    RelativeOrientationColmapProperties();
    ~RelativeOrientationColmapProperties() override = default;

    virtual bool refineFocalLength() const;
    virtual void setRefineFocalLength(bool refineFocalLength);

    virtual bool refinePrincipalPoint() const;
    virtual void setRefinePrincipalPoint(bool refinePrincipalPoint);

    virtual bool refineExtraParams() const;
    virtual void setRefineExtraParams(bool refineExtraParams);

// RelativeOrientation interface

public:

    void reset() override;
    QString name() const override;

private:

    bool mRefineFocalLength;
    bool mRefinePrincipalPoint;
    bool mRefineExtraParams;
};





class RelativeOrientationColmapTask
  : public RelativeOrientationColmapProperties,
    public tl::TaskBase
{

public:

    RelativeOrientationColmapTask(const tl::Path &database,
                                  const tl::Path &outputPath,
                                  const std::vector<Image> &images,
                                  const std::map<int, Camera> &cameras,
                                  bool fixCalibration);
    ~RelativeOrientationColmapTask() override;

    std::map<int, Camera> cameras() const;

// tl::TaskBase

public:

    void stop() override;

protected:

    void execute(tl::Progress *progressBar = nullptr) override;

private:

    tl::Path mDatabase;
    tl::Path mOutputPath;
    std::vector<Image> mImages;
    std::map<int, Camera> mCameras;
    colmap::IncrementalMapperOptions *mIncrementalMapper;
    colmap::IncrementalMapperController *mMapper;
    std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
    colmap::BundleAdjustmentController *mBundleAdjustmentController;

};





class AbsoluteOrientationColmapProperties
  : public AbsoluteOrientation
{

public:

    AbsoluteOrientationColmapProperties();
    ~AbsoluteOrientationColmapProperties() override = default;

    virtual int minCommonImages() const;
    virtual void setMinCommonImages(int minCommonImages);

    virtual bool robustAlignment() const;
    virtual void setRobustAlignment(bool robustAlignment);

    virtual double robustAlignmentMaxError() const;
    virtual void setRobustAlignmentMaxError(double robustAlignmentMaxError);

// AbsoluteOrientation Interface

public:

    void reset() override;
    QString name() const override;

private:

    int mMinCommonImages;
    bool mRobustAlignment;
    double mRobustAlignmentMaxError;
};





class AbsoluteOrientationColmapTask
  : public AbsoluteOrientationColmapProperties,
    public tl::TaskBase
{

public:

    AbsoluteOrientationColmapTask(const tl::Path &path,
                                  const std::vector<Image> &images);
    ~AbsoluteOrientationColmapTask() override;

    std::unordered_map<size_t, double> cameraPosesErrors() const;

// tl::TaskBase interface

public:

    void stop() override;

protected:

    void execute(tl::Progress *progressBar) override;

private:

    tl::Path mInputPath;
    std::vector<Image> mImages;
    std::unordered_map<size_t, double> mCameraPosesErrors;
};





class ImportPosesTask
    : public tl::TaskBase
{

public:

    ImportPosesTask(const std::vector<Image> &images,
                    const std::map<int, Camera> &cameras,
                    const tl::Path &outputPath,
                    const tl::Path &database,
                    bool fixCalibration = false,
                    bool fixPoses = true);
    ~ImportPosesTask() override;

    std::map<int, Camera> cameras() const;

    void setFixCalibration(bool fixCalibration);
    void setFixPoses(bool fixPoses);

private:

    void computeOffset();
    void temporalReconstruction(const tl::Path &tempPath);
    void writeImages(const tl::Path &tempPath);
    void writeCameras(const tl::Path &tempPath);
    void writePoints(const tl::Path &tempPath);
    bool isCoordinatesLocal() const;

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
};


/*----------------------------------------------------------------*/

void colmapRemoveOrientations(const std::vector<std::string> &images,
                              const std::string &reconstruction);




} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_COLMAP_H
