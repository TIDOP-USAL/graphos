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

#include "graphos/process/orientation/RelativeOrientationProcess.h"

#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace graphos
{

RelativeOrientationProcess::RelativeOrientationProcess(std::shared_ptr<RelativeOrientationAlgorithm> &orientationAlgorithm)
  : ProcessConcurrent(),
    mRelativeOrientationAlgorithm(orientationAlgorithm)
{

}

RelativeOrientationProcess::~RelativeOrientationProcess()
{
}

void RelativeOrientationProcess::run()
{
  try {

    msgInfo("Starting Orientation");

    tl::Chrono chrono("Orientation process finished");
    chrono.run();

    mRelativeOrientationAlgorithm->run();

    emit orientationFinished();

    chrono.stop();

  } catch (std::exception &e) {
    emit error(0, "Relative Orientation error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Relative Orientation error");
    msgError("Unknow Exception");
  }
}

} // namespace graphos
