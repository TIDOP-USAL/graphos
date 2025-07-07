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

#include "graphos/core/camera/io/impl/OpenCVCalibrationReader.h"
#include "graphos/core/camera/Calibration.h"

#include <QXmlStreamReader>
#include <QFile>

namespace graphos
{

void OpenCVCalibrationReader::read(const tl::Path &path, Camera &camera)
{
    try {

        QFile file(QString::fromStdString(path.toString()));
        TL_ASSERT(file.open(QIODevice::ReadOnly | QIODevice::Text), "Unable to open OpenCV calibration file.");

        QXmlStreamReader stream(&file);

        stream.readNextStartElement();

        TL_ASSERT(stream.name() == "opencv_storage", "Invalid OpenCV calibration file: root element is not <opencv_storage>.");

        double fx = 0.0, fy = 0.0, cx = 0.0, cy = 0.0;
        // Distortion coefficients
        double k1 = 0., k2 = 0., p1 = 0., p2 = 0., k3 = 0., k4 = 0.;
        std::vector<double> distortion_coefficients;

        while (stream.readNextStartElement()) {
            QString name = stream.name().toString();

            if (name == "camera_matrix") {
                while (stream.readNextStartElement()) {
                    if (stream.name() == "data") {

                        QStringList values = stream.readElementText().trimmed().split(QRegExp("\\s+"), Qt::SkipEmptyParts);
                        TL_ASSERT(values.size() == 9, "Invalid camera matrix size in OpenCV file (expected 3x3 = 9 elements).");

                        fx = values[0].toDouble();
                        cx = values[2].toDouble() - 0.5;
                        fy = values[4].toDouble();
                        cy = values[5].toDouble() - 0.5;

                        //calibration->setParameter(Calibration::Parameters::focal, (fx + fy) / 2.0);
                        //calibration->setParameter(Calibration::Parameters::focalx, fx);
                        //calibration->setParameter(Calibration::Parameters::focaly, fy);
                        //calibration->setParameter(Calibration::Parameters::cx, cx);
                        //calibration->setParameter(Calibration::Parameters::cy, cy);

                    } else {
                        stream.skipCurrentElement();
                    }
                }
            } else if (name == "distortion_coefficients") {

                while (stream.readNextStartElement()) {
                    if (stream.name() == "data") {

                        QStringList values = stream.readElementText().trimmed().split(QRegExp("\\s+"), Qt::SkipEmptyParts);
                        for (const auto &v : values) {
                            distortion_coefficients.push_back(v.toDouble());
                        }

                    } else {
                        stream.skipCurrentElement();
                    }
                }

            } else {
                stream.skipCurrentElement();
            }
        }

        std::shared_ptr<Calibration> calibration;
        if (distortion_coefficients.size() == 4) {
            // Ver si es fisheye
            double fov = 2. * atan(camera.width() / (2. * camera.focal())) * tl::consts::rad_to_deg<double>;
            if (fov > 150) {
                calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
                calibration->setParameter(Calibration::Parameters::k1, distortion_coefficients[0]);
                calibration->setParameter(Calibration::Parameters::k2, distortion_coefficients[1]);
                calibration->setParameter(Calibration::Parameters::k3, distortion_coefficients[2]);
                calibration->setParameter(Calibration::Parameters::k4, distortion_coefficients[3]);
            } else {

                if (distortion_coefficients[0] == 0. && distortion_coefficients[1] == 0. && 
                    distortion_coefficients[2] == 0. && distortion_coefficients[3] == 0. && 
                    distortion_coefficients[4] == 0.) {
                    calibration = CalibrationFactory::create(Calibration::CameraModel::pinhole);
                } else if (distortion_coefficients[1] == 0. && distortion_coefficients[2] == 0. && 
                           distortion_coefficients[3] == 0. && distortion_coefficients[4] == 0.) {
                    calibration = CalibrationFactory::create(Calibration::CameraModel::radial1);
                } else if (distortion_coefficients[2] == 0. && distortion_coefficients[3] == 0. && 
                           distortion_coefficients[4] == 0.) {
                     calibration = CalibrationFactory::create(Calibration::CameraModel::radial2);
                } else {
                    calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
                }
            }

        } else if (distortion_coefficients.size() == 5) {
            calibration = CalibrationFactory::create(Calibration::CameraModel::radial3);
        } else {
            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_full);
        }

        calibration->setParameter(Calibration::Parameters::focal, (fx + fy) / 2.0);
        calibration->setParameter(Calibration::Parameters::focalx, fx);
        calibration->setParameter(Calibration::Parameters::focaly, fy);
        calibration->setParameter(Calibration::Parameters::cx, cx);
        calibration->setParameter(Calibration::Parameters::cy, cy);

        if (distortion_coefficients[0] > 0.)
            calibration->setParameter(Calibration::Parameters::k1, distortion_coefficients[0]);

        if (distortion_coefficients[1] > 0.)
            calibration->setParameter(Calibration::Parameters::k2, distortion_coefficients[1]);

        if (calibration->checkCameraType(Calibration::CameraType::fisheye)) {

            if (distortion_coefficients[2] > 0.)
                calibration->setParameter(Calibration::Parameters::k3, distortion_coefficients[2]);
            if (distortion_coefficients[3] > 0.)
                calibration->setParameter(Calibration::Parameters::k4, distortion_coefficients[3]);

        } else {

            if (distortion_coefficients[2] > 0.)
                calibration->setParameter(Calibration::Parameters::p1, distortion_coefficients[2]);
            if (distortion_coefficients[3] > 0.)
                calibration->setParameter(Calibration::Parameters::p2, distortion_coefficients[3]);
        }

        if (distortion_coefficients.size() > 4 && distortion_coefficients[4] > 0.)
            calibration->setParameter(Calibration::Parameters::k3, distortion_coefficients[4]);

        if (distortion_coefficients.size() > 5) {
            if (distortion_coefficients[5] > 0.)
                calibration->setParameter(Calibration::Parameters::k4, distortion_coefficients[5]);
            if (distortion_coefficients[6] > 0.)
                calibration->setParameter(Calibration::Parameters::k5, distortion_coefficients[6]);
            if (distortion_coefficients[7] > 0.)
                calibration->setParameter(Calibration::Parameters::k6, distortion_coefficients[7]);
        }

        camera.setCalibration(calibration);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to read OpenCV calibration file: {}", path.toUtf8());
        throw;
    }
}

} // namespace graphos
