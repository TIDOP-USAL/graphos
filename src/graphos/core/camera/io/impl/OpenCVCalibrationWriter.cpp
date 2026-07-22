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

#include "graphos/core/camera/io/impl/OpenCVCalibrationWriter.h"
#include "graphos/core/camera/Calibration.h"

#include <QFile>
#include <QTextStream>

namespace graphos
{

void OpenCVCalibrationWriter::write(const tl::Path &path,
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

        double fx = 0.0, fy = 0.0, cx = 0.0, cy = 0.0;

        if (calibration->existParameter(Calibration::Parameters::focal)) {
            fx = fy = calibration->parameter(Calibration::Parameters::focal);
        }

        if (calibration->existParameter(Calibration::Parameters::focalx)) {
            fx = calibration->parameter(Calibration::Parameters::focalx);
        }

        if (calibration->existParameter(Calibration::Parameters::focaly)) {
            fy = calibration->parameter(Calibration::Parameters::focaly);
        }

        if (calibration->existParameter(Calibration::Parameters::cx)) {
            cx = calibration->parameter(Calibration::Parameters::cx) + 0.5;
        }

        if (calibration->existParameter(Calibration::Parameters::cy)) {
            cy = calibration->parameter(Calibration::Parameters::cy) + 0.5;
        }

        stream << "<?xml version=\"1.0\"?>\n";
        stream << "<opencv_storage>\n";
        stream << "<calibration_time></calibration_time>\n";
        stream << "<image_width>" << width << "</image_width>\n";
        stream << "<image_height>" << height << "</image_height>\n";
        stream << "<camera_matrix type_id=\"opencv-matrix\">\n";
        stream << "  <rows>3</rows>\n";
        stream << "  <cols>3</cols>\n";
        stream << "  <dt>d</dt>\n";
        stream << "  <data>\n";
        stream << "    " << fx << " 0. " << cx << "\n";
        stream << "    0. " << fy << " " << cy << "\n";
        stream << "    0. 0. 1.\n";
        stream << "  </data>\n";
        stream << "</camera_matrix>\n";

        // Distortion coefficients
        Calibration::CameraModel camera_model = calibration->cameraModel();
        int distortions_coefficients_size = 4;
        switch (camera_model) {
        case Calibration::CameraModel::radial1:
        case Calibration::CameraModel::radial2:
        case Calibration::CameraModel::simple_radial_fisheye:
        case Calibration::CameraModel::radial_fisheye:
        case Calibration::CameraModel::opencv:
        case Calibration::CameraModel::opencv_fisheye:
        case Calibration::CameraModel::simple_pinhole:
        case Calibration::CameraModel::pinhole:
            distortions_coefficients_size = 4;
            break;
        case Calibration::CameraModel::radial3:
            distortions_coefficients_size = 5;
            break;
        case Calibration::CameraModel::opencv_full:
            distortions_coefficients_size = 8;
            break;
        }

        auto fisheye = calibration->checkCameraType(Calibration::CameraType::fisheye);

        std::vector<double> distortions_coefficients(distortions_coefficients_size, 0.);

        if (calibration->existParameter(Calibration::Parameters::k1)) {
            distortions_coefficients[0] = calibration->parameter(Calibration::Parameters::k1);
        }

        if (calibration->existParameter(Calibration::Parameters::k2)) {
            distortions_coefficients[1] = calibration->parameter(Calibration::Parameters::k2);
        }

        if (fisheye) {

            if (calibration->existParameter(Calibration::Parameters::k3)) {
                distortions_coefficients[2] = calibration->parameter(Calibration::Parameters::k3);
            }

            if (calibration->existParameter(Calibration::Parameters::k4)) {
                distortions_coefficients[3] = calibration->parameter(Calibration::Parameters::k4);
            }

        } else {

            if (calibration->existParameter(Calibration::Parameters::p1)) {
                distortions_coefficients[2] = calibration->parameter(Calibration::Parameters::p1);
            }

            if (calibration->existParameter(Calibration::Parameters::p2)) {
                distortions_coefficients[3] = calibration->parameter(Calibration::Parameters::p2);
            }

            if (calibration->existParameter(Calibration::Parameters::k3)) {
                distortions_coefficients[4] = calibration->parameter(Calibration::Parameters::k3);
            }

            if (calibration->existParameter(Calibration::Parameters::k4)) {
                distortions_coefficients[5] = calibration->parameter(Calibration::Parameters::k4);
            }

            if (calibration->existParameter(Calibration::Parameters::k5)) {
                distortions_coefficients[6] = calibration->parameter(Calibration::Parameters::k5);
            }

            if (calibration->existParameter(Calibration::Parameters::k6)) {
                distortions_coefficients[7] = calibration->parameter(Calibration::Parameters::k6);
            }
        }

        stream << "<distortion_coefficients type_id=\"opencv-matrix\">\n";
        stream << "  <rows>" << distortions_coefficients_size << "</rows>\n";
        stream << "  <cols>1</cols>\n";
        stream << "  <dt>d</dt>\n";
        stream << "  <data>\n";
        for (const auto coefficient : distortions_coefficients) {
            stream << "    " << QString::number(coefficient, 'e', 8) << "\n";
        }
        stream << "  </data>\n";
        stream << "</distortion_coefficients>\n";
        stream << "</opencv_storage>\n";

        file.close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to write OpenCV calibration file: {}", path.toUtf8());
    }
}

} // namespace graphos
