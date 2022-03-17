/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#ifndef GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H
#define GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/features/features.h"
#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

namespace graphos
{

class GRAPHOS_EXPORT FeatureExtractorProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  FeatureExtractorProcess(const std::vector<Image> &images,
                          const std::map<int, Camera> &cameras,
                          const QString &database,
                          int maxImageSize,
                          bool cuda,
                          const std::shared_ptr<FeatureExtractor> &featureExtractor);

signals:

  void featuresExtracted(QString, QString);

  // ProcessConcurrent interface

protected:

  void run() override;

protected:

  std::vector<Image> mImages;
  std::map<int, Camera> mCameras;
  QString mDatabase;
  int mMaxImageSize;
  bool bUseCuda;
  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
  //bool bOpenCvRead;
};


} // graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_PROCESS_H
