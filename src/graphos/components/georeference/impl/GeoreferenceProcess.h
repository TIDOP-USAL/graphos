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

#ifndef GRAPHOS_GEOREFERENCE_PROCESS_H
#define GRAPHOS_GEOREFERENCE_PROCESS_H

#include "graphos/core/orientation/gcp.h"

#include <tidop/core/process.h>
#include <tidop/core/progress.h>

#include <QObject>

namespace colmap
{
class OptionManager;
struct IncrementalMapperOptions;
class ReconstructionManager;
class IncrementalMapperController;
}

namespace graphos
{

class AbsoluteOrientationAlgorithm;

class GRAPHOS_EXPORT GeoreferenceProcess
  : public QObject,
    public tl::ProcessBase
{
  Q_OBJECT

public:

  GeoreferenceProcess(const QString &inputPath,
                      const QString &outputPath,
                      const std::vector<GroundControlPoint> &groundControlPoints);
  ~GeoreferenceProcess() override;

signals:

  void georeferenceFinished();

// tl::ProcessBase interface

protected:

  void execute(tl::Progress *progressBar) override;

private:

  QString mInputPath;
  QString mOutputPath;
  std::vector<GroundControlPoint> mGroundControlPoints;
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_PROCESS_H
