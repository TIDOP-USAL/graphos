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

struct DenseReport
{
    double time = 0.0;
    int points = 0;
    std::string method;
    bool cuda = false;

    bool isEmpty() const
    {
        return time == 0. && points == 0 && method.empty();
    }
};

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

    /*!
     * \brief Reset the Densification properties.
     */
    virtual void reset() = 0;

    /*!
     * \brief Densification method name
     */
    virtual auto name() const -> QString = 0;

    /*!
     * \brief Densification method
     * \see Method
     */
    auto method() const -> Method { return mDensificationMethod.flags(); }

protected:

    tl::EnumFlags<Method> mDensificationMethod;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Densification::Method)



/*----------------------------------------------------------------*/


class Densifier
{

public:

    Densifier() = default;
    virtual ~Densifier() = default;

    virtual void enableCuda(bool enable) = 0;
    virtual auto isCudaEnabled() const -> bool = 0;
    virtual auto denseModel() const -> tl::Path = 0;
    virtual auto report() const -> DenseReport = 0;
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
                  tl::Path outputPath);
    ~DensifierBase() override;

    void setUndistortImagesFormat(UndistortImages::Format format);

protected:

    void undistort(const QString &dir) const;
    auto outputPath() const -> tl::Path;
    auto images() const -> const std::unordered_map<size_t, Image>&;
    auto cameras() const -> const std::map<int, Camera>&;
    auto poses() const -> const std::unordered_map<size_t, CameraPose>&;
    auto groundPoints() const -> const std::vector<GroundPoint>&;
    void setDenseModel(const tl::Path &denseModel);
    void autoSegmentation() const;

// Densifier

public:

    void enableCuda(bool enable) override;
    auto isCudaEnabled() const -> bool override;
    auto denseModel() const -> tl::Path override;
    auto report() const -> DenseReport override;

private:

    std::unordered_map<size_t, Image> mImages;
    std::map<int, Camera> mCameras;
    std::unordered_map<size_t, CameraPose> mPoses;
    std::vector<GroundPoint> mGroundPoints;
    tl::Path mOutputPath;
    bool mCuda;
    tl::Path mDenseModel;
    UndistortImages::Format mFormat;

protected:

    DenseReport mReport;
};



} // namespace graphos

#endif // GRAPHOS_CORE_DENSIFICATION_H
