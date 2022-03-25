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

#ifndef GRAPHOS_FEATURE_EXTRACTOR_COMMAND_H
#define GRAPHOS_FEATURE_EXTRACTOR_COMMAND_H

#include "graphos/core/command.h"
#include "graphos/core/features/features.h"
#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

#include <tidop/core/process.h>
#include <tidop/core/progress.h>

#include <QObject>

namespace graphos
{
	

class FeatureExtractorProcess
  : public QObject,
    public tl::ProcessBase
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

// tl::ProcessBase interface

protected:

  void execute(tl::Progress *progressBar) override;

protected:

  std::vector<Image> mImages;
  std::map<int, Camera> mCameras;
  QString mDatabase;
  int mMaxImageSize;
  bool bUseCuda;
  std::shared_ptr<FeatureExtractor> mFeatureExtractor;
};




class FeatureExtractorCommand
  : public QObject,
    public Command
{

  Q_OBJECT

public:

  FeatureExtractorCommand();
  ~FeatureExtractorCommand() override;

private:


// Command

  bool run() override;

private:

  tl::Path mProjectFile;
  int mMaxImageSize;
  int mMaxFeaturesNumber;
  int mOctaveResolution;
  double mContrastThreshold;
  double mEdgeThreshold;
  bool mDisableCuda;

};

	
} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_COMMAND_H