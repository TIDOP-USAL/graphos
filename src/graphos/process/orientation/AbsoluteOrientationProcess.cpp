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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "AbsoluteOrientationProcess.h"

#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace graphos
{

AbsoluteOrientationProcess::AbsoluteOrientationProcess(std::shared_ptr<AbsoluteOrientationAlgorithm> &orientationAlgorithm)
  : ProcessConcurrent(),
    mAbsoluteOrientationAlgorithm(orientationAlgorithm)
{

}

AbsoluteOrientationProcess::~AbsoluteOrientationProcess()
{
}

void AbsoluteOrientationProcess::run()
{
  try {

    msgInfo("Starting Absolute Orientation");

    tl::Chrono chrono("Absolute Orientation process finished");
    chrono.run();

    mAbsoluteOrientationAlgorithm->run();

    emit absoluteOrientationFinished();

    chrono.stop();

  } catch (std::exception &e) {
    emit error(0, "Absolute Orientation error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Absolute Orientation error");
    msgError("Unknow Exception");
  }
}

} // namespace graphos
