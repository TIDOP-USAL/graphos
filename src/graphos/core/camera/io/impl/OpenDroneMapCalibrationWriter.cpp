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

#include "graphos/core/camera/io/impl/OpenDroneMapCalibrationWriter.h"
#include "graphos/core/camera/Calibration.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>

namespace graphos
{

void OpenDroneMapCalibrationWriter::write(const tl::Path &path,
                                          const Camera &camera,
                                          bool prior)
{
    try {

        QJsonObject odmCameras;

        std::shared_ptr<Calibration> calibration;
        if (prior)
            calibration = camera.priorCalibration();
        else
            calibration = camera.calibration();

        TL_ASSERT(calibration != nullptr, "No calibration data to write.");

        auto width = camera.width();
        auto height = camera.height();
        auto sensorSize = camera.sensorSize();

        double fx = calibration->parameter(Calibration::Parameters::focalx);
        double fy = calibration->parameter(Calibration::Parameters::focaly);
        double cx = calibration->parameter(Calibration::Parameters::cx);
        double cy = calibration->parameter(Calibration::Parameters::cy);

        double k1 = calibration->existParameter(Calibration::Parameters::k1) ? calibration->parameter(Calibration::Parameters::k1) : 0.0;
        double k2 = calibration->existParameter(Calibration::Parameters::k2) ? calibration->parameter(Calibration::Parameters::k2) : 0.0;
        double p1 = calibration->existParameter(Calibration::Parameters::p1) ? calibration->parameter(Calibration::Parameters::p1) : 0.0;
        double p2 = calibration->existParameter(Calibration::Parameters::p2) ? calibration->parameter(Calibration::Parameters::p2) : 0.0;
        double k3 = calibration->existParameter(Calibration::Parameters::k3) ? calibration->parameter(Calibration::Parameters::k3) : 0.0;

        QString odmModel;
        if (calibration->cameraModel() == Calibration::CameraModel::simple_radial_fisheye || 
                   calibration->cameraModel() == Calibration::CameraModel::radial_fisheye || 
                   calibration->cameraModel() == Calibration::CameraModel::opencv_fisheye) {
            odmModel = "fisheye";
        } else {
            odmModel = "brown";
        }

        double focal_mm = fx * sensorSize / width;
        double focal_ratio = std::round((focal_mm / sensorSize) * 10000.0) / 10000.0;

        QString key = QString("%1 %2 %3 %4 %5 %6")
                            .arg(QString::fromStdString(camera.make()))
                            .arg(QString::fromStdString(camera.model()))
                            .arg(width)
                            .arg(height)
                            .arg(odmModel)
                            .arg(focal_ratio, 0, 'f', 4);

        //QJsonObject cameraObj;
        //cameraObj["projection_type"] = odmModel;
        //cameraObj["width"] = width;
        //cameraObj["height"] = height;
        //cameraObj["focal_x"] = fx / width;
        //cameraObj["focal_y"] = fy / height;
        //cameraObj["c_x"] = (cx / width) - 0.5;
        //cameraObj["c_y"] = (cy / height) - 0.5;

        //if (odmModel == "brown" || odmModel == "fisheye_opencv") {
        //    cameraObj["k1"] = k1;
        //    cameraObj["k2"] = k2;
        //    cameraObj["p1"] = p1;
        //    cameraObj["p2"] = p2;
        //    cameraObj["k3"] = k3;
        //}

        //odmCameras.insert(key, cameraObj);

        //QJsonDocument doc(odmCameras);


        //QFile file(QString::fromStdString(path.toString()));
        //TL_ASSERT(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate), "Unable to open file for writing.");

        //file.write(doc.toJson(QJsonDocument::Indented));
        //file.close();       
        
        QFile file(QString::fromStdString(path.toString()));
        TL_ASSERT(file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text), "Unable to open file for writing.");

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setRealNumberPrecision(8);

        out << "{\n";
        out << "  \"" << key << "\": {\n";
        out << "    \"projection_type\": \"" << odmModel << "\",\n";
        out << "    \"width\": " << width << ",\n";
        out << "    \"height\": " << height << ",\n";
        out << "    \"focal_x\": " << (fx / width) << ",\n";
        out << "    \"focal_y\": " << (fy / height) << ",\n";
        out << "    \"c_x\": " << ((cx / width) - 0.5) << ",\n";
        out << "    \"c_y\": " << ((cy / height) - 0.5);

        if (odmModel == "brown" || odmModel == "fisheye") {
            out << ",\n    \"k1\": " << k1
                << ",\n    \"k2\": " << k2
                << ",\n    \"p1\": " << p1
                << ",\n    \"p2\": " << p2
                << ",\n    \"k3\": " << k3;
        }

        out << "\n  }\n";
        out << "}\n";

        file.close();
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Failed to write Agisoft calibration file: {}", path.toUtf8());
    }
}

} // namespace graphos
