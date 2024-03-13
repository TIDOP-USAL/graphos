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

#ifndef GRAPHOS_CORE_BA_REPORT_H
#define GRAPHOS_CORE_BA_REPORT_H

#include "graphos/graphos_global.h"

#include <string>

namespace graphos
{

struct OrientationReport
{
	int orientedImages = 0;
	std::string type;

	// Bundle Adjust
    int iterations = 0;
    double initialCost = 0.0;
	double finalCost = 0.0;
    std::string termination = "NO_CONVERGENCE";
	
	double alignmentErrorMean = 0.0;
	double alignmentErrorMedian = 0.0;
	double time = 0.0;

	bool isEmpty() const
	{
		return iterations == 0 && initialCost == 0.0 
			&& finalCost == 0.0 && termination == "NO_CONVERGENCE";
	}
	
};


} // namespace graphos

#endif // GRAPHOS_CORE_BA_REPORT_H
