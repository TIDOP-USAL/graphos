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

#ifndef GRAPHOS_CORE_FEATURE_EXTRACTOR_TASK_H
#define GRAPHOS_CORE_FEATURE_EXTRACTOR_TASK_H

#include <unordered_map>

#include <QObject>

#include <tidop/core/task.h>
#include <tidop/core/progress.h>
#include <tidop/core/path.h>

#include "graphos/core/features/features.h"
#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

namespace graphos
{

class FeatureExtractorTask
  : public QObject,
    public tl::TaskBase
{

  Q_OBJECT

public:

  FeatureExtractorTask(const std::unordered_map<size_t, Image> &images,
                       const std::map<int, Camera> &cameras,
                       const tl::Path &database,
                       int maxImageSize,
                       bool cuda,
                       const std::shared_ptr<FeatureExtractor> &featureExtractor);

  ~FeatureExtractorTask() override = default;

signals:

  void features_extracted(qulonglong, QString);

  // tl::TaskBase interface

protected:

  void execute(tl::Progress *progressBar) override;

protected:

  const std::unordered_map<size_t, Image> &mImages;
  const std::map<int, Camera> &mCameras;
  tl::Path mDatabase;
  int mMaxImageSize;
  bool bUseCuda;
  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
};

} // namespace graphos

#endif // GRAPHOS_CORE_FEATURE_EXTRACTOR_TASK_H
