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

#include "graphos/core/camera/io/impl/AgisoftCalibrationReader.h"
#include "graphos/core/camera/Calibration.h"

#include <QXmlStreamReader>
#include <QFile>

namespace graphos
{

void AgisoftCalibrationReader::read(const tl::Path &path, Camera &camera)
{
    try {

        QFile input(QString::fromStdString(path.toString()));
        TL_ASSERT(input.open(QIODevice::ReadOnly | QIODevice::Text), "Unable to open Agisoft calibration file.");

        QXmlStreamReader stream(&input);

        stream.readNextStartElement();

        TL_ASSERT(stream.name() == "calibration", "Invalid Agisoft calibration file format: root element is not <calibration>.");


        QString projection;
        int width = 0, height = 0;
        double f = 0.0;
        double cx = 0., cy = 0., k1 = 0., k2 = 0., k3 = 0., k4 = 0., p1 = 0., p2 = 0.;

        while (stream.readNextStartElement()) {

            QString name = stream.name().toString();
            QString text = stream.readElementText();

            if (name == "projection") {
                projection = stream.readElementText();
            } else if (name == "width") {
                width = text.toInt();
            } else if (name == "height") {
                height = text.toInt();
            } else if (name == "f") {
                f = text.toDouble();
            } else if (name == "cx") {
                cx = text.toDouble() + static_cast<double>(camera.width()) / 2.;
            } else if (name == "cy") {
                cy = text.toDouble() + static_cast<double>(camera.height()) / 2.;
            } else if (name == "k1") {
                k1 = text.toDouble();
            } else if (name == "k2") {
                k2 = text.toDouble();
            } else if (name == "k3") {
                k3 = text.toDouble();
            } else if (name == "k4") {
                k4 = text.toDouble();
            } else if (name == "p1") {
                p1 = text.toDouble();
            } else if (name == "p2") {
                p2 = text.toDouble();
            } else {
                stream.skipCurrentElement();
            }
        }

        TL_ASSERT(projection.isEmpty(), "Missing projection type in Agisoft calibration file.");

        std::shared_ptr<Calibration> calibration;
        if (projection == "frame") {
            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
        } else if (projection == "fisheye") {
            calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
        }  else {
            TL_THROW_EXCEPTION("Unsupported projection type: {}", projection.toStdString());
        }

        calibration->setParameter(Calibration::Parameters::cx, stream.readElementText().toDouble());
        calibration->setParameter(Calibration::Parameters::cy, stream.readElementText().toDouble());
        calibration->setParameter(Calibration::Parameters::focal, f);
        calibration->setParameter(Calibration::Parameters::focalx, f);
        calibration->setParameter(Calibration::Parameters::focaly, f);
        calibration->setParameter(Calibration::Parameters::k1, k1);
        calibration->setParameter(Calibration::Parameters::k2, k2);
        calibration->setParameter(Calibration::Parameters::k3, k3);
        if (projection == "fisheye") calibration->setParameter(Calibration::Parameters::k4, k4);
        calibration->setParameter(Calibration::Parameters::p1, p1);
        calibration->setParameter(Calibration::Parameters::p2, p2);

        camera.setCalibration(calibration);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to read Agisoft calibration file: {}", path.toUtf8());
        throw;
    }
}


} // namespace graphos
