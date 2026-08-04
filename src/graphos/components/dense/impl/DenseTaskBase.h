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


#include <map>
#include <unordered_map>
#include <memory>

#include <QString>

#include <tidop/core/base/flags.h>
#include <tidop/core/base/Path.h>
#include <tidop/core/task/Task.h>

#include "graphos/core/camera/CameraRepository.h"
#include "graphos/core/camera/Undistort.h"
#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/orientation/GroundPoint.h"
#include "graphos/core/orientation/CameraPosesRepository.h"
#include "graphos/core/dense/DensificationReport.h"

namespace graphos
{

class DenseTaskBase
  : public tl::Task
{

public:

    DenseTaskBase(const ImageRepository &imageRepo,
                  const CameraRepository &cameraRepo,
                  const CameraPosesRepository &posesRepo,
                  const std::vector<GroundPoint> &groundPoints,
                  tl::Path outputPath);
    ~DenseTaskBase() override;

    void setUndistortImagesFormat(UndistortImages::Format format);
    void enableCuda(bool enable);
    auto isCudaEnabled() const -> bool;
    auto denseModel() const -> tl::Path;
    auto report() const -> DensificationReport;

protected:

    void undistort(const QString &dir) const;
    auto outputPath() const -> tl::Path;
    auto images() const -> const ImageRepository &;
    auto cameras() const -> const CameraRepository &;
    auto poses() const -> const CameraPosesRepository &;
    auto groundPoints() const -> const std::vector<GroundPoint>&;
    void setDenseModel(const tl::Path &denseModel);
    void autoSegmentation() const;
    virtual void copyUndistortedImages() const = 0;

private:

    const ImageRepository &mImageRepo;
    const CameraRepository &mCameraRepo;
    const CameraPosesRepository &mPosesRepo;
    std::vector<GroundPoint> mGroundPoints;
    tl::Path mOutputPath;
    bool mCuda;
    tl::Path mDenseModel;
    UndistortImages::Format mFormat;

protected:

    DensificationReport mReport;
};



} // namespace graphos
