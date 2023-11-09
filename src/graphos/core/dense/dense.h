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

#ifndef GRAPHOS_CORE_DENSIFICATION_H
#define GRAPHOS_CORE_DENSIFICATION_H

#include "graphos/graphos_global.h"


#include <map>
#include <unordered_map>
#include <memory>

#include <QString>

#include <tidop/core/flags.h>
#include <tidop/core/path.h>
#include <tidop/core/task.h>

#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/image.h"

namespace graphos
{

class Camera;
class CameraPose;

class Densification
{

public:

    enum class Method
    {
        cmvs_pmvs,
        smvs,
        mvs
    };

public:

    Densification(Method method) : mDensificationMethod(method) {}
    virtual ~Densification() = default;


    virtual void reset() = 0;
    virtual QString name() const = 0;
    Method method() const { return mDensificationMethod.flags(); }

protected:

    tl::EnumFlags<Method> mDensificationMethod;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Densification::Method)



/*----------------------------------------------------------------*/


class Densifier
{

public:

    Densifier() {}
    virtual ~Densifier() = default;

    virtual void enableCuda(bool enable) = 0;
    virtual bool isCudaEnabled() const = 0;
    virtual tl::Path denseModel() const = 0;

};



class DensifierBase
  : public Densifier,
    public tl::TaskBase
{

public:

    DensifierBase(const std::unordered_map<size_t, Image> &images,
                  const std::map<int, Camera> &cameras,
                  const std::unordered_map<size_t, CameraPose> &poses,
                  const std::vector<GroundPoint> &groundPoints,
                  const tl::Path &outputPath);
    ~DensifierBase();

// Densifier

public:

    void enableCuda(bool enable) override;
    virtual bool isCudaEnabled() const override;
    tl::Path denseModel() const override;
    void setUndistortImagesFormat(UndistortImages::Format format);

protected:

    void undistort(const QString &dir);
    tl::Path outputPath() const;
    const std::unordered_map<size_t, Image> &images() const;
    const std::map<int, Camera> &cameras() const;
    const std::unordered_map<size_t, CameraPose> &poses() const;
    const std::vector<GroundPoint> &groundPoints() const;
    void setDenseModel(const tl::Path &denseModel);
    void autoSegmentation();

private:

    std::unordered_map<size_t, Image> mImages;
    std::map<int, Camera> mCameras;
    std::unordered_map<size_t, CameraPose> mPoses;
    std::vector<GroundPoint> mGroundPoints;
    tl::Path mOutputPath;
    bool mCuda;
    tl::Path mDenseModel;
    UndistortImages::Format mFormat;
};



/*----------------------------------------------------------------*/



class CmvsPmvs
  : public Densification
{

public:

    CmvsPmvs() : Densification(Densification::Method::cmvs_pmvs) {}
    ~CmvsPmvs() override = default;

    virtual bool useVisibilityInformation() const = 0;
    virtual int imagesPerCluster() const = 0;
    virtual int level() const = 0;
    virtual int cellSize() const = 0;
    virtual double threshold() const = 0;
    virtual int windowSize() const = 0;
    virtual int minimunImageNumber() const = 0;

    virtual void setUseVisibilityInformation(bool useVisibilityInformation) = 0;
    virtual void setImagesPerCluster(int imagesPerCluster) = 0;
    virtual void setLevel(int level) = 0;
    virtual void setCellSize(int cellSize) = 0;
    virtual void setThreshold(double threshold) = 0;
    virtual void setWindowSize(int windowSize) = 0;
    virtual void setMinimunImageNumber(int minimunImageNumber) = 0;
};


/*----------------------------------------------------------------*/



class Smvs
  : public Densification
{

public:

    Smvs() : Densification(Densification::Method::smvs) {}
    ~Smvs() override = default;

    virtual int inputImageScale() const = 0;
    virtual int outputDepthScale() const = 0;
    virtual bool shadingBasedOptimization() const = 0;
    virtual bool semiGlobalMatching() const = 0;
    virtual double surfaceSmoothingFactor() const = 0;

    virtual void setInputImageScale(int inputImageScale) = 0;
    virtual void setOutputDepthScale(int outputDepthScale) = 0;
    virtual void setShadingBasedOptimization(bool shadingBasedOptimization) = 0;
    virtual void setSemiGlobalMatching(bool semiGlobalMatching) = 0;
    virtual void setSurfaceSmoothingFactor(double surfaceSmoothingFactor) = 0;

};

/*----------------------------------------------------------------*/



class Mvs
  : public Densification
{

public:

    Mvs() : Densification(Densification::Method::mvs)
    {
    }
    ~Mvs() override = default;

    virtual int resolutionLevel() const = 0;
    virtual int minResolution() const = 0;
    virtual int maxResolution() const = 0;
    virtual int numberViews() const = 0;
    virtual int numberViewsFuse() const = 0;
    virtual bool estimateColors() const = 0;
    virtual bool estimateNormals() const = 0;

    virtual void setResolutionLevel(int resolutionLevel) = 0;
    virtual void setMinResolution(int minResolution) = 0;
    virtual void setMaxResolution(int maxResolution) = 0;
    virtual void setNumberViews(int numberViews) = 0;
    virtual void setNumberViewsFuse(int numberViewsFuse) = 0;
    virtual void setEstimateColors(bool estimateColors) = 0;
    virtual void setEstimateNormals(bool estimateNormals) = 0;
};


} // namespace graphos

#endif // GRAPHOS_CORE_DENSIFICATION_H
