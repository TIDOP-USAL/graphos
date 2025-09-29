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

#include "graphos/core/camera/io/impl/Pix4DCalibrationReader.h"
#include "graphos/core/camera/Calibration.h"

#include <fstream>
#include <sstream>
#include <QString>

namespace graphos
{

void Pix4DCalibrationReader::read(const tl::Path &path, Camera &camera, bool prior)
{
    try {

        std::ifstream in(path.toString());
        TL_ASSERT(in.is_open(), "Unable to open Pix4D calibration file.");


        size_t sensor_width_px = std::max(camera.width(), camera.height());
        size_t sensor_width_mm = static_cast<size_t>(camera.sensorSize());
        double scale = sensor_width_mm == 1 ? 1. : static_cast<double>(sensor_width_mm) / static_cast<double>(sensor_width_px);

        std::string line;
        double f = 0.0, px = 0.0, py = 0.0;
        double k1 = 0.0, k2 = 0.0, k3 = 0.0;
        double t1 = 0.0, t2 = 0.0;

        while (std::getline(in, line)) {
            std::istringstream ss(line);
            std::string label;
            ss >> label;

            if (label == "F") ss >> f;
            else if (label == "Px") ss >> px;
            else if (label == "Py") ss >> py;
            else if (label == "K1") ss >> k1;
            else if (label == "K2") ss >> k2;
            else if (label == "K3") ss >> k3;
            else if (label == "T1") ss >> t1;
            else if (label == "T2") ss >> t2;
        }


        auto calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
        calibration->setParameter(Calibration::Parameters::focal, f / scale);
        calibration->setParameter(Calibration::Parameters::focalx, f / scale);
        calibration->setParameter(Calibration::Parameters::focaly, f / scale);
        calibration->setParameter(Calibration::Parameters::cx, px / scale);
        calibration->setParameter(Calibration::Parameters::cy, py / scale);
        calibration->setParameter(Calibration::Parameters::k1, k1);
        calibration->setParameter(Calibration::Parameters::k2, k2);
        calibration->setParameter(Calibration::Parameters::k3, k3);
        calibration->setParameter(Calibration::Parameters::p1, t1);
        calibration->setParameter(Calibration::Parameters::p2, t2);

        if (prior)
            camera.setPriorCalibration(calibration);
        else
            camera.setCalibration(calibration);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to read Pix4D calibration file: {}", path.toUtf8());
        throw;
    }
}

} // namespace graphos
