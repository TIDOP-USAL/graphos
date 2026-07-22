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

#include "graphos/core/camera/io/impl/AgisoftCalibrationWriter.h"
#include "graphos/core/camera/Calibration.h"

#include <QFile>
#include <QTextStream>

namespace graphos
{

void AgisoftCalibrationWriter::write(const tl::Path &path,
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

        QFile file(QString::fromStdString(path.toString()));
        TL_ASSERT(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate), "Unable to open file for writing.");

        QTextStream stream(&file);

        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        stream << "<calibration>\n";
        if (calibration->cameraModel() == Calibration::CameraModel::opencv) {
            stream << "  <projection>frame</projection>\n";
        } else if (calibration->cameraModel() == Calibration::CameraModel::opencv_fisheye) {
            stream << "  <projection>fisheye</projection>\n";
        }

        auto focal = calibration->existParameter(Calibration::Parameters::focal) ?
                     calibration->parameter(Calibration::Parameters::focal) :
                     (calibration->parameter(Calibration::Parameters::focalx) + calibration->parameter(Calibration::Parameters::focaly)) / 2.;

        stream << "  <width>" << width << "</width>\n";
        stream << "  <height>" << height << "</height>\n";
        stream << "  <f>" << focal << "</f>\n";

        if (calibration->existParameter(Calibration::Parameters::cx)) {
            stream << "  <cx>" << static_cast<double>(camera.width()) / 2. - calibration->parameter(Calibration::Parameters::cx) << "</cx>\n";
        }

        if (calibration->existParameter(Calibration::Parameters::cy)) {
            stream << "  <cy>" << static_cast<double>(camera.height()) / 2. - calibration->parameter(Calibration::Parameters::cy) << "</cy>\n";
        }

        if (calibration->existParameter(Calibration::Parameters::k1)) {
            stream << "  <k1>" << calibration->parameter(Calibration::Parameters::k1) << "</k1>\n";
        }

        if (calibration->existParameter(Calibration::Parameters::k2)) {
            stream << "  <k2>" << calibration->parameter(Calibration::Parameters::k2) << "</k2>\n";
        }

        if (calibration->existParameter(Calibration::Parameters::k3)) {
            stream << "  <k3>" << calibration->parameter(Calibration::Parameters::k3) << "</k3>\n";
        }

        if (calibration->existParameter(Calibration::Parameters::k4)) {
            stream << "  <k4>" << calibration->parameter(Calibration::Parameters::k4) << "</k4>\n";
        }

        if (calibration->existParameter(Calibration::Parameters::p1)) {
            stream << "  <p1>" << calibration->parameter(Calibration::Parameters::p1) << "</p1>\n";
        }

        if (calibration->existParameter(Calibration::Parameters::p2)) {
            stream << "  <p2>" << calibration->parameter(Calibration::Parameters::p2) << "</p2>\n";
        }

        stream << "  <date></date>\n";
        stream << "</calibration>\n";

        file.close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to write Agisoft calibration file: {}", path.toUtf8());
    }
}

} // namespace graphos
