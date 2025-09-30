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

#include "graphos/core/camera/io/impl/OpenDroneMapCalibrationReader.h"
#include "graphos/core/camera/Calibration.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

namespace graphos
{

void OpenDroneMapCalibrationReader::read(const tl::Path &path, Camera &camera, bool prior)
{
    try {

        QFile file(QString::fromStdString(path.toString()));
        TL_ASSERT(file.open(QIODevice::ReadOnly | QIODevice::Text), "Unable to open OpenDroneMap calibration file: {}", path.toUtf8());

        QByteArray data = file.readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        file.close();

        TL_ASSERT(parseError.error == QJsonParseError::NoError, "JSON parsing error: {}", parseError.errorString().toStdString());
        TL_ASSERT(doc.isObject(), "OpenDroneMap calibration file is not a JSON object");
        
        std::shared_ptr<Calibration> calibration;

        QJsonObject root = doc.object();

        for (auto it = root.begin(); it != root.end(); ++it) {
            QJsonObject obj = it.value().toObject();

            QString projection = obj.value("projection_type").toString();
            int width = obj.value("width").toInt();
            int height = obj.value("height").toInt();
            double fx_norm = obj.value("focal_x").toDouble();
            double fy_norm = obj.value("focal_y").toDouble();
            double cx_norm = obj.value("c_x").toDouble();
            double cy_norm = obj.value("c_y").toDouble();

            double fx = fx_norm * width;
            double fy = fy_norm * height;
            double cx = (cx_norm + 0.5) * width;
            double cy = (cy_norm + 0.5) * height;

            if (projection == "fisheye_opencv")
                calibration = CalibrationFactory::create(Calibration::CameraModel::opencv_fisheye);
            else if (projection == "brown")
                calibration = CalibrationFactory::create(Calibration::CameraModel::opencv);
            else
                calibration = CalibrationFactory::create(Calibration::CameraModel::pinhole);

            calibration->setParameter(Calibration::Parameters::focalx, fx);
            calibration->setParameter(Calibration::Parameters::focaly, fy);
            calibration->setParameter(Calibration::Parameters::focal, (fx + fy) / 2.0);
            calibration->setParameter(Calibration::Parameters::cx, cx);
            calibration->setParameter(Calibration::Parameters::cy, cy);

            if (obj.contains("k1")) calibration->setParameter(Calibration::Parameters::k1, obj["k1"].toDouble());
            if (obj.contains("k2")) calibration->setParameter(Calibration::Parameters::k2, obj["k2"].toDouble());
            if (obj.contains("p1")) calibration->setParameter(Calibration::Parameters::p1, obj["p1"].toDouble());
            if (obj.contains("p2")) calibration->setParameter(Calibration::Parameters::p2, obj["p2"].toDouble());
            if (obj.contains("k3")) calibration->setParameter(Calibration::Parameters::k3, obj["k3"].toDouble());

            if (prior)
                camera.setPriorCalibration(calibration);
            else
                camera.setCalibration(calibration);

        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to read OpenDroneMap calibration file: {}", path.toUtf8());
        throw;
    }
}


} // namespace graphos
