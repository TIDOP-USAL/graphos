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

#include "graphos/core/camera/io/impl/Pix4DCalibrationWriter.h"
#include "graphos/core/camera/Calibration.h"

#include <fstream>
#include <iomanip>

namespace graphos
{

void Pix4DCalibrationWriter::write(const tl::Path &path,
                                   const Camera &camera,
                                   bool prior)
{
    try {

        std::shared_ptr<Calibration> calibration;
        if (prior)
            calibration = camera.priorCalibration();
        else
            calibration = camera.calibration();
        auto width = camera.width();
        auto height = camera.height();

        TL_ASSERT(calibration != nullptr, "No calibration data to write.");

        std::ofstream out(path.toString());
        TL_ASSERT(out.is_open(), "Cannot open file for writing Pix4D format.");

        size_t sensor_width_px = std::max(camera.width(), camera.height());
        size_t sensor_width_mm = static_cast<size_t>(camera.sensorSize());
        double scale = sensor_width_mm == 1 ? 1. : static_cast<double>(sensor_width_mm) / static_cast<double>(sensor_width_px);
        double w = camera.width() * scale;
        double h = camera.height() * scale;

        auto focal = calibration->existParameter(Calibration::Parameters::focal) ?
                     calibration->parameter(Calibration::Parameters::focal) :
                     (calibration->parameter(Calibration::Parameters::focalx) + calibration->parameter(Calibration::Parameters::focaly)) / 2.;
        focal *= scale;

        out << "Pix4D camera calibration file 0\n";
        out << "#Focal Length mm assuming a sensor width of " << w << "x" << h << "mm\n";
        out << "F " << focal << "\n";
        out << "#Principal Point mm\n";
        out << "Px " << calibration->parameter(Calibration::Parameters::cx) * scale << "\n";
        out << "Py " << calibration->parameter(Calibration::Parameters::cy) * scale << "\n";
        out << "#Symmetrical Lens Distortion Coeffs\n";
        out << "K1 " << (calibration->existParameter(Calibration::Parameters::k1) ? calibration->parameter(Calibration::Parameters::k1) : 0.0) << "\n";
        out << "K2 " << (calibration->existParameter(Calibration::Parameters::k2) ? calibration->parameter(Calibration::Parameters::k2) : 0.0) << "\n";
        out << "K3 " << (calibration->existParameter(Calibration::Parameters::k3) ? calibration->parameter(Calibration::Parameters::k3) : 0.0) << "\n";
        out << "#Tangential Lens Distortion Coeffs\n";
        out << "T1 " << (calibration->existParameter(Calibration::Parameters::p1) ? calibration->parameter(Calibration::Parameters::p1) : 0.0) << "\n";
        out << "T2 " << (calibration->existParameter(Calibration::Parameters::p2) ? calibration->parameter(Calibration::Parameters::p2) : 0.0) << "\n";

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to write Pix4D calibration file: {}", path.toUtf8());
    }

}

} // namespace graphos
