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

#include "graphos/core/io/ProjectReader.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>

namespace graphos
{


auto streamToInt(QXmlStreamReader &stream) -> int
{
    QString string = stream.readElementText();
    if (!string.isEmpty()) return string.toInt();
    else return 0;
}

auto streamToDouble(QXmlStreamReader &stream) -> double
{
    QString string = stream.readElementText();
    if (!string.isEmpty()) return string.toDouble();
    else return 0.;
}

auto streamToBoolean(QXmlStreamReader &stream) -> bool
{
    return stream.readElementText().compare("true") == 0 ? true : false;
}

auto streamToStdString(QXmlStreamReader &stream) -> std::string
{
    return stream.readElementText().toStdString();
}


auto readCameraPosition(QXmlStreamReader &stream) -> CameraPose
{
    CameraPose cameraPose;
    tl::Point3d position;
    tl::Vector3d accuracy;
    tl::Quaterniond quaternion;

    while (stream.readNextStartElement()) {
        if (stream.name() == "CRS") {
            cameraPose.setCrs(stream.readElementText());
        } else if (stream.name() == "X") {
            position.x() = streamToDouble(stream);
        } else if (stream.name() == "Y") {
            position.y() = streamToDouble(stream);
        } else if (stream.name() == "Z") {
            position.z() = streamToDouble(stream);
        } else if (stream.name() == "SX") {
            accuracy.x() = streamToDouble(stream);
        } else if (stream.name() == "SY") {
            accuracy.y() = streamToDouble(stream);
        } else if (stream.name() == "SZ") {
            accuracy.z() = streamToDouble(stream);
        } else if (stream.name() == "Source") {
            cameraPose.setSource(stream.readElementText());
        } else if (stream.name() == "QX") {
            quaternion.x() = streamToDouble(stream);
        } else if (stream.name() == "QY") {
            quaternion.y() = streamToDouble(stream);
        } else if (stream.name() == "QZ") {
            quaternion.z() = streamToDouble(stream);
        } else if (stream.name() == "QW") {
            quaternion.w() = streamToDouble(stream);
        } else if (stream.name() == "RtkFlag") {
            cameraPose.setRtkFlag(streamToInt(stream));
        }
    }

    cameraPose.setPosition(position);
    cameraPose.setAccuracy(accuracy);
    cameraPose.setQuaternion(quaternion);

    return cameraPose;
}

void readImageMetadata(QXmlStreamReader &stream, Image &image)
{
    for (auto &attr : stream.attributes()) {

        auto key = attr.name().toString().toStdString();
        auto value = attr.value().toString().toStdString();

        image.addMetadata(key, value);
    }
    stream.skipCurrentElement();
}


void ProjectReader::read(const tl::Path &file, Project &project)
{
    try {
    
        project.clear();

        QFile input(QString::fromStdWString(file.toWString()));

        project.info().setProjectPath(file);
    
        if (input.open(QIODevice::ReadOnly)) {
    
            QXmlStreamReader stream;
            stream.setDevice(&input);
    
            if (stream.readNextStartElement()) {
                if (stream.name() == "Graphos") {
                    while (stream.readNextStartElement()) {

                        auto tag = stream.name();

                        if (tag == "General") {
                            this->readInfo(stream, project.info());
                        } else if (tag == "Cameras") {
                            this->readCameras(stream, project.cameras());
                        } else if (tag == "Images") {
                            this->readImages(stream, project.images());
                        } else {
                            stream.skipCurrentElement();
                        }
                    }
                }
            }

            input.close();
    
        }
    
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Exception detected when reading the project");
    }
}

void ProjectReader::readInfo(QXmlStreamReader &stream, ProjectInfo &projectInfo)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Path") {
            projectInfo.setProjectPath(streamToStdString(stream));
        } else if (stream.name() == "Description") {
            projectInfo.setDescription(streamToStdString(stream));
        } else
            stream.skipCurrentElement();
    }
}

void ProjectReader::readCameras(QXmlStreamReader &stream, CameraRepository &cameraRepository)
{
}

void ProjectReader::readImages(QXmlStreamReader &stream, ImageRepository &imageRepository)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Image") {

            Image img;

            //size_t id = 0;
            //for (auto &attr : stream.attributes()) {
            //    if (attr.name().compare(QString("id")) == 0) {
            //        id = attr.value().toULongLong();
            //        break;
            //    }
            //}

            while (stream.readNextStartElement()) {

                auto tag = stream.name();

                if (tag == "File") {
                    img.setPath(streamToStdString(stream));
                } else if (tag == "CameraId") {
                    img.setCameraId(streamToInt(stream));
                } else if (tag == "CameraPosition") {
                    img.cameraPose() = readCameraPosition(stream);
                } else if (tag == "Metadata") {
                    readImageMetadata(stream, img);
                } else
                    stream.skipCurrentElement();
            }

            imageRepository.add(img);

        } else {
            stream.skipCurrentElement();
        }
    }
}

} // end namespace graphos
