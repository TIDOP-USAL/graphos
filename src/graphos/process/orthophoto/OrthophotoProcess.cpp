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


#include "OrthophotoProcess.h"

#include "graphos/core/Orthophoto.h"

#include <tidop/core/messages.h>

namespace graphos
{

OrthophotoProcess::OrthophotoProcess(std::shared_ptr<OrthophotoAlgorithm> &orthophotoAlgorithm)
  : ProcessConcurrent(),
    mOrthophotoAlgorithm(orthophotoAlgorithm)
{

}

OrthophotoProcess::~OrthophotoProcess()
{

}

void OrthophotoProcess::run()
{
	
  try {
    
	mOrthophotoAlgorithm->run();

  } catch (std::exception &e) {
    emit error(0, "Process error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Process error");
    msgError("Unknow Exception");
  }

}

} // namespace graphos
