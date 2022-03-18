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

#ifndef GRAPHOS_FEATURE_MATCHING_PROCESS_H
#define GRAPHOS_FEATURE_MATCHING_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/features/features.h"



namespace colmap
{
struct ExhaustiveMatchingOptions;
struct SiftMatchingOptions;
class ExhaustiveFeatureMatcher;
class Thread;
}

namespace graphos
{

class GRAPHOS_EXPORT FeatureMatchingProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  FeatureMatchingProcess(QString database,
                         bool cuda,
                         bool spatialMatching,
                         const std::shared_ptr<FeatureMatching> &featureMatching);
  ~FeatureMatchingProcess() override;

signals:

  void featureMatchingFinished();

//private slots:
//
//  void onCancel();

// ProcessConcurrent interface

public:

  std::shared_ptr<FeatureMatching> featureMatching() const;
  void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);

  QString database() const;
  void setDatabase(const QString &database);

  bool useGPU() const;
  void setUseGPU(bool useGPU);

  void setSpatialMatching(bool spatialMatching);

protected:

  void run() override;

// Process interface

public slots:

  void stop() override;

private:

  colmap::Thread *mFeatureMatcher;
  QString mDatabase;
  bool bUseCuda;
  bool bSpatialMatching;
  std::shared_ptr<FeatureMatching> mFeatureMatching;

};

} // namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_PROCESS_H
