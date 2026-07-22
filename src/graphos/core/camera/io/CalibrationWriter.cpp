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

#include "graphos/core/camera/io/CalibrationWriter.h"
#include "graphos/core/camera/io/impl/AgisoftCalibrationWriter.h"
#include "graphos/core/camera/io/impl/OpenCVCalibrationWriter.h"
#include "graphos/core/camera/io/impl/Pix4DCalibrationWriter.h"
#include "graphos/core/camera/io/impl/OpenDroneMapCalibrationWriter.h"

#include <tidop/core/base/Exception.h>

namespace graphos
{


/* Calibration writer */

CalibrationWriter::CalibrationWriter() = default;


/* Camera Poses Writer Factory */

auto CalibrationWriterFactory::create(const std::string &format) -> std::unique_ptr<CalibrationWriter>
{
    std::unique_ptr<CalibrationWriter> writer;

    try {

        if (format == "Agisoft") {
            writer = std::make_unique<AgisoftCalibrationWriter>();
        } else if (format == "OpenCV") {
            writer = std::make_unique<OpenCVCalibrationWriter>();
        } else if (format == "Pix4D") {
            writer = std::make_unique<Pix4DCalibrationWriter>();
        } else if (format == "ODM") {
            writer = std::make_unique<OpenDroneMapCalibrationWriter>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return writer;
}

} // namespace graphos
