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

#ifndef GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H
#define GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H

#include <tidop/core/task.h>
#include <tidop/core/progress.h>

#include <QObject>
#include <tidop/core/progress.h>
namespace colmap
{
struct IncrementalMapperOptions;
class ReconstructionManager;
class IncrementalMapperController;
class BundleAdjustmentController;
}

namespace graphos
{

class RelativeOrientationProcess
  : public QObject,
    public tl::TaskBase
{
  Q_OBJECT

public:

  RelativeOrientationProcess(const QString &database,
                             const QString &outputPath,
                             bool fixCalibration);
  ~RelativeOrientationProcess() override;

signals:

  void orientation_finished();

// tl::TaskBase interface

public:

  void stop() override;

protected:

  void execute(tl::Progress *progressBar) override;

private:

  QString mDatabase;
  QString mOutputPath;
  colmap::IncrementalMapperOptions *mIncrementalMapper;
  colmap::IncrementalMapperController *mMapper;
  colmap::BundleAdjustmentController *mBundleAdjustmentController;
  std::shared_ptr<colmap::ReconstructionManager> mReconstructionManager;
  bool mRefineFocalLength;
  bool mRefinePrincipalPoint;
  bool mRefineExtraParams;
};

} // namespace graphos

#endif // GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H
