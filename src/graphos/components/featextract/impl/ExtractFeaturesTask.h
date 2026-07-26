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

#include <unordered_map>

#include <QObject>

#include <tidop/core/task/Task.h>
#include <tidop/core/task/Progress.h>
#include <tidop/core/base/Path.h>

#include "graphos/core/features/Features.h"
#include "graphos/core/features/FeatureExtractorReport.h"
#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"


namespace graphos
{

class ImageRepository;
class CameraRepository;

class ExtractFeaturesTask
  : public QObject,
    public tl::Task
{

    Q_OBJECT

public:

    ExtractFeaturesTask(const ImageRepository &imageRepo,
                        const CameraRepository &cameraRepo,
                        tl::Path database,
                        int maxImageSize,
                        bool cuda,
                        const std::shared_ptr<FeatureExtractor> &featureExtractor);

    ~ExtractFeaturesTask() override = default;

    TL_DISABLE_COPY(ExtractFeaturesTask)
    TL_DISABLE_MOVE(ExtractFeaturesTask)

    auto report() const -> FeatureExtractorReport;

signals:

    void features_extracted(qulonglong, QString);

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

protected:

    const ImageRepository &mImageRepo;
    const CameraRepository &mCameraRepo;
    tl::Path mDatabase;
    int mMaxImageSize;
    bool bUseCuda;
    std::shared_ptr<FeatureExtractor> mFeatureExtractor;
    FeatureExtractorReport mReport;
};

} // namespace graphos