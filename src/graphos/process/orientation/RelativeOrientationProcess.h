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

#ifndef GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H
#define GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"

namespace colmap
{
class OptionManager;
struct IncrementalMapperOptions;
class ReconstructionManager;
class IncrementalMapperController;
}

namespace graphos
{

class RelativeOrientationAlgorithm;

class GRAPHOS_EXPORT RelativeOrientationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  RelativeOrientationProcess(std::shared_ptr<RelativeOrientationAlgorithm> &orientationAlgorithm);
  ~RelativeOrientationProcess() override;

signals:

  void orientationFinished();

// ProcessConcurrent interface

protected:

  void run() override;

private:

  std::shared_ptr<RelativeOrientationAlgorithm> mRelativeOrientationAlgorithm;

};

} // namespace graphos

#endif // GRAPHOS_RELATIVE_ORIENTATION_PROCESS_H
