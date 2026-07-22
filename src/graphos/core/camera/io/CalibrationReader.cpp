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

#include "graphos/core/camera/io/CalibrationReader.h"
#include "graphos/core/camera/io/impl/AgisoftCalibrationReader.h"
#include "graphos/core/camera/io/impl/OpenCVCalibrationReader.h"
#include "graphos/core/camera/io/impl/Pix4DCalibrationReader.h"
#include "graphos/core/camera/io/impl/OpenDroneMapCalibrationReader.h"

#include <tidop/core/base/Exception.h>

namespace graphos
{


/* Calibration reader */

CalibrationReader::CalibrationReader() = default;

/* Calibration reader factory */

auto CalibrationReaderFactory::create(const std::string& format) -> std::unique_ptr<CalibrationReader>
{
    std::unique_ptr<CalibrationReader> reader;

    try {	
	    if (format == "Agisoft") {
            reader = std::make_unique<AgisoftCalibrationReader>();
        } else if (format == "OpenCV") {
            reader = std::make_unique<OpenCVCalibrationReader>();
        } else if (format == "Pix4D") {
            reader = std::make_unique<Pix4DCalibrationReader>();
        } else if (format == "ODM") {
            reader = std::make_unique<OpenDroneMapCalibrationReader>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format.c_str());
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }	

    return reader;
}


} // namespace graphos
