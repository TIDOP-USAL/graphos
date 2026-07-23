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

#include "graphos/core/io/ProjectWriter.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

#include <fstream>

namespace graphos
{

void ProjectWriter::write(const tl::Path &file, const Project &project)
{
    const auto &project_info = project.info();

    // tener file y project_path que debería ser lo mismo...
    //auto project_path = project_info.projectPath();

    tl::Path tmp_file = file;
    tmp_file.replaceExtension(".bak");

    try {

        // Esto podría añadirse a una clase Bak
        std::ifstream src(file.toString(), std::ios::binary);
        std::ofstream dst(tmp_file.toString(), std::ios::binary);
        dst << src.rdbuf();
        src.close();
        dst.close();

        QFile output(QString::fromStdWString(file.toWString()));
        if (output.open(QFile::WriteOnly)) {
            QXmlStreamWriter stream(&output);
            stream.setAutoFormatting(true);
            stream.writeStartDocument();

            stream.writeStartElement("Graphos");
            {
                writeInfo(stream, project.info());
                writeCameras(stream, project.cameras());
                writeImages(stream, project.images());
            }

            stream.writeEndElement(); // Graphos

            output.close();
        }

    } catch (...) {

        std::ifstream  src(tmp_file.toString(), std::ios::binary);
        std::ofstream  dst(file.toString(), std::ios::binary);
        dst << src.rdbuf();
        src.close();
        dst.close();
        tl::Path::removeFile(tmp_file);

        TL_THROW_EXCEPTION_WITH_NESTED("Exception detected when writing the project");

    }

    tl::Path::removeFile(tmp_file);
}

void ProjectWriter::writeInfo(QXmlStreamWriter &stream, const ProjectInfo &projectInfo)
{
    stream.writeAttribute("version", projectInfo.version());

    stream.writeStartElement("General");
    {
        stream.writeTextElement("Name", projectInfo.name());
        stream.writeTextElement("Path", QString::fromStdWString(projectInfo.projectFolder().toWString()));
        stream.writeTextElement("Description", projectInfo.description());
    }
    stream.writeEndElement();

    //stream.writeTextElement("Database", QString::fromStdWString(projectInfo.database().toWString()));
}

void ProjectWriter::writeCameras(QXmlStreamWriter &stream, const CameraRepository &cameraRepository)
{
    stream.writeStartElement("Cameras");
    for (const auto &[id, camera] : cameraRepository.all()) {
        stream.writeStartElement("Camera");
        {
            stream.writeAttribute("id", QString::number(id));

            stream.writeTextElement("Make", QString::fromStdString(camera.make()));
            stream.writeTextElement("Model", QString::fromStdString(camera.model()));
            stream.writeTextElement("SerialNumber", QString::fromStdString(camera.serialNumber()));
            stream.writeTextElement("Type", QString::fromStdString(camera.type()));
            stream.writeTextElement("Focal", QString::number(camera.focal()));
            stream.writeTextElement("Width", QString::number(camera.width()));
            stream.writeTextElement("Height", QString::number(camera.height()));
            stream.writeTextElement("BitsPerPixel", QString::number(camera.bitsPerPixel()));
            stream.writeTextElement("SensorSize", QString::number(camera.sensorSize()));

            if (!camera.bandName().empty()) 
                stream.writeTextElement("BandName", QString::fromStdString(camera.bandName()));

            if (auto black_level = camera.blackLevel())
                stream.writeTextElement("BlackLevel", QString::number(camera.blackLevel().value()));

            if (auto &calibrated_h_matrix = camera.calibratedHMatrix()) {
                QStringList hmatrix;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        hmatrix.append(QString::number(calibrated_h_matrix.value().at(i, j), 'f', 10));
                    }
                }
                stream.writeTextElement("CalibratedHMatrix", hmatrix.join(" "));
            }
            writePriorCalibration(stream, camera.priorCalibration().get());
            writeCalibration(stream, camera.calibration().get());
            writeVignetting(stream, camera.vignettingModel().get());
        }
        stream.writeEndElement(); // Camera

    }
    stream.writeEndElement();
}

void ProjectWriter::writePriorCalibration(QXmlStreamWriter &stream, const Calibration *calibration)
{
    if (calibration) {
        stream.writeStartElement("PriorCalibration");
        {
            for (auto &[parameter, value] : *calibration) {
                stream.writeTextElement(QString::fromStdString(calibration->parameterName(parameter)),
                                        QString::number(value, 'f', 10));
            }
        }
        stream.writeEndElement(); // PriorCalibration
    }
}

void ProjectWriter::writeCalibration(QXmlStreamWriter &stream, const Calibration *calibration)
{
    if (calibration) {
        stream.writeStartElement("Calibration");
        {
            for (auto &[parameter, value] : *calibration) {
                stream.writeTextElement(QString::fromStdString(calibration->parameterName(parameter)),
                    QString::number(value, 'f', 10));
            }
        }
        stream.writeEndElement(); // Calibration
    }
}

void ProjectWriter::writeVignetting(QXmlStreamWriter &stream, const Vignetting *vignetting)
{
    if (vignetting) {
        stream.writeStartElement("Vignetting");

        auto model = vignetting->model();
        switch (model) {
        case Vignetting::Model::radial:
        {
            stream.writeAttribute("model", QString("radial"));
            auto vignetting_radial = dynamic_cast<const VignettingRadial *>(vignetting);
            stream.writeTextElement("VignettingCenter",
                QString::number(vignetting_radial->center().x()) + ";" +
                QString::number(vignetting_radial->center().y()));
            QStringList vignetting_polynomial;
            for (const auto &coef : vignetting_radial->polynomial()) {
                vignetting_polynomial.append(QString::number(coef));
            }
            stream.writeTextElement("VignettingPolynomial", vignetting_polynomial.join(";"));
            break;
        }
        case Vignetting::Model::polynomial2d:
        {
            stream.writeAttribute("model", QString("polynomial2d"));

            auto vignetting_polynomial2d = dynamic_cast<const VignettingPolynomial2D *>(vignetting);
            QStringList coeffs;
            for (const auto &coef : vignetting_polynomial2d->coeffs()) {
                coeffs.append(QString::number(coef));
            }
            stream.writeTextElement("VignettingCoeffs", coeffs.join(";"));
            QStringList _powers;
            for (const auto &power_pair : vignetting_polynomial2d->powers()) {
                _powers.append(QString::number(power_pair.first));
                _powers.append(QString::number(power_pair.second));
            }
            stream.writeTextElement("VignettingPowers", _powers.join(";"));
            break;
        }
        default:
            break;
        }

        stream.writeEndElement(); // Vignetting
    }
}

void ProjectWriter::writeImages(QXmlStreamWriter &stream, const ImageRepository &imageRepository)
{
    stream.writeStartElement("Images");
    {
        for (const auto &[id, image] : imageRepository.all()) {
            stream.writeStartElement("Image");
            {
                stream.writeAttribute("id", QString::number(id));
                stream.writeTextElement("File", QString::fromStdString(image.path().toString()));
                stream.writeTextElement("CameraId", QString::number(image.cameraId()));
                writeCameraPosition(stream, image.cameraPose());
                writeImageMetadata(stream, image.metadata());
            }
            stream.writeEndElement(); // Image
        }
    }
    stream.writeEndElement();
}

void ProjectWriter::writeCameraPosition(QXmlStreamWriter &stream,
                                        const CameraPose &cameraPosition)
{
    if (!cameraPosition.isEmpty()) {
        stream.writeStartElement("CameraPosition");
        {
            stream.writeTextElement("CRS", cameraPosition.crs());
            stream.writeTextElement("X", QString::number(cameraPosition.position().x(), 'f', 8));
            stream.writeTextElement("Y", QString::number(cameraPosition.position().y(), 'f', 8));
            stream.writeTextElement("Z", QString::number(cameraPosition.position().z(), 'f', 3));
            stream.writeTextElement("SX", QString::number(cameraPosition.accuracy().x(), 'f', 3));
            stream.writeTextElement("SY", QString::number(cameraPosition.accuracy().y(), 'f', 3));
            stream.writeTextElement("SZ", QString::number(cameraPosition.accuracy().z(), 'f', 3));
            stream.writeTextElement("QX", QString::number(cameraPosition.quaternion().x(), 'f', 10));
            stream.writeTextElement("QY", QString::number(cameraPosition.quaternion().y(), 'f', 10));
            stream.writeTextElement("QZ", QString::number(cameraPosition.quaternion().z(), 'f', 10));
            stream.writeTextElement("QW", QString::number(cameraPosition.quaternion().w(), 'f', 10));
            stream.writeTextElement("Source", cameraPosition.source());
            stream.writeTextElement("RtkFlag", QString::number(cameraPosition.rtkFlag()));
        }
        stream.writeEndElement();
    }
}

void ProjectWriter::writeImageMetadata(QXmlStreamWriter &stream, const Image::Metadata &metadata)
{
    for (const auto &[key, value] : metadata) {
        stream.writeStartElement("Metadata");
        stream.writeAttribute(QString::fromStdString(key), QString::fromStdString(value));
        stream.writeEndElement();
    }
}

} // end namespace graphos
