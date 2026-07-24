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

#include "graphos/core/features/sift.h"

#include <tidop/core/base/Split.h>

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


void readPriorCalibration(QXmlStreamReader &stream, Camera &camera)
{
    try {

        std::shared_ptr<Calibration> calibration;
        calibration = CalibrationFactory::create(camera.type());
        while (stream.readNextStartElement()) {
            if (stream.name().compare(QString("f")) == 0) {
                calibration->setParameter(Calibration::Parameters::focal, streamToDouble(stream));
            } else if (stream.name() == "fx") {
                calibration->setParameter(Calibration::Parameters::focalx, streamToDouble(stream));
            } else if (stream.name() == "fy") {
                calibration->setParameter(Calibration::Parameters::focaly, streamToDouble(stream));
            } else if (stream.name() == "cx") {
                calibration->setParameter(Calibration::Parameters::cx, streamToDouble(stream));
            } else if (stream.name() == "cy") {
                calibration->setParameter(Calibration::Parameters::cy, streamToDouble(stream));
            } else if (stream.name() == "k1") {
                calibration->setParameter(Calibration::Parameters::k1, streamToDouble(stream));
            } else if (stream.name() == "k2") {
                calibration->setParameter(Calibration::Parameters::k2, streamToDouble(stream));
            } else if (stream.name() == "k3") {
                calibration->setParameter(Calibration::Parameters::k3, streamToDouble(stream));
            } else if (stream.name() == "k4") {
                calibration->setParameter(Calibration::Parameters::k4, streamToDouble(stream));
            } else if (stream.name() == "k5") {
                calibration->setParameter(Calibration::Parameters::k5, streamToDouble(stream));
            } else if (stream.name() == "k6") {
                calibration->setParameter(Calibration::Parameters::k6, streamToDouble(stream));
            } else if (stream.name() == "p1") {
                calibration->setParameter(Calibration::Parameters::p1, streamToDouble(stream));
            } else if (stream.name() == "p2") {
                calibration->setParameter(Calibration::Parameters::p2, streamToDouble(stream));
            } else
                stream.skipCurrentElement();
        }

        camera.setPriorCalibration(calibration);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void readCalibration(QXmlStreamReader &stream, Camera &camera)
{
    try {

        std::shared_ptr<Calibration> calibration;
        calibration = CalibrationFactory::create(camera.type());
        while (stream.readNextStartElement()) {
            if (stream.name().compare(QString("f")) == 0) {
                calibration->setParameter(Calibration::Parameters::focal, streamToDouble(stream));
            } else if (stream.name() == "fx") {
                calibration->setParameter(Calibration::Parameters::focalx, streamToDouble(stream));
            } else if (stream.name() == "fy") {
                calibration->setParameter(Calibration::Parameters::focaly, streamToDouble(stream));
            } else if (stream.name() == "cx") {
                calibration->setParameter(Calibration::Parameters::cx, streamToDouble(stream));
            } else if (stream.name() == "cy") {
                calibration->setParameter(Calibration::Parameters::cy, streamToDouble(stream));
            } else if (stream.name() == "k1") {
                calibration->setParameter(Calibration::Parameters::k1, streamToDouble(stream));
            } else if (stream.name() == "k2") {
                calibration->setParameter(Calibration::Parameters::k2, streamToDouble(stream));
            } else if (stream.name() == "k3") {
                calibration->setParameter(Calibration::Parameters::k3, streamToDouble(stream));
            } else if (stream.name() == "k4") {
                calibration->setParameter(Calibration::Parameters::k4, streamToDouble(stream));
            } else if (stream.name() == "k5") {
                calibration->setParameter(Calibration::Parameters::k5, streamToDouble(stream));
            } else if (stream.name() == "k6") {
                calibration->setParameter(Calibration::Parameters::k6, streamToDouble(stream));
            } else if (stream.name() == "p1") {
                calibration->setParameter(Calibration::Parameters::p1, streamToDouble(stream));
            } else if (stream.name() == "p2") {
                calibration->setParameter(Calibration::Parameters::p2, streamToDouble(stream));
            } else
                stream.skipCurrentElement();
        }

        camera.setCalibration(calibration);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void readVignetting(QXmlStreamReader &stream, Camera &camera)
{
    try {

        QString vignetting_model = 0;
        for (auto &attr : stream.attributes()) {
            if (attr.name().compare(QString("model")) == 0) {
                vignetting_model = attr.value().toString();
                break;
            }
        }

        std::shared_ptr<Vignetting> model;

        if (vignetting_model == "radial") {

            tl::Point2f vignetting_center_point;
            std::vector<float> vignetting_polynomial;

            while (stream.readNextStartElement()) {

                if (stream.name() == "VignettingCenter") {
                    auto vignetting_center = tl::split<float>(stream.readElementText().toStdString(), ';');
                    if (vignetting_center.size() == 2)
                        vignetting_center_point = tl::Point2f(vignetting_center[0], vignetting_center[1]);
                } else if (stream.name() == "VignettingPolynomial") {
                    auto polynomial = tl::split<float>(stream.readElementText().toStdString(), ';');
                    if (polynomial.size() == 6)
                        vignetting_polynomial = polynomial;
                } else
                    stream.skipCurrentElement();
            }

            if (!vignetting_polynomial.empty())
                model = std::make_shared<VignettingRadial>(vignetting_center_point, vignetting_polynomial);

        } else if (vignetting_model == "polynomial2d") {

            std::vector<float> coeffs;
            std::vector<std::pair<int, int>> power_pairs;

            while (stream.readNextStartElement()) {

                if (stream.name() == "VignettingCoeffs") {
                    coeffs = tl::split<float>(stream.readElementText().toStdString(), ';');
                } else if (stream.name() == "VignettingPowers") {
                    auto powers = tl::split<float>(stream.readElementText().toStdString(), ';');
                    power_pairs.reserve(powers.size() / 2);
                    for (size_t i = 0; i < powers.size(); i += 2) {
                        power_pairs.push_back(std::make_pair(static_cast<int>(powers[i]), static_cast<int>(powers[i + 1])));
                    }
                } else
                    stream.skipCurrentElement();
            }

            model = std::make_shared<VignettingPolynomial2D>(coeffs, power_pairs);

        } else {
            stream.skipCurrentElement();
            return;
        }

        camera.setVignettingModel(model);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void readCamera(QXmlStreamReader &stream, Camera &camera)
{
    int id = 0;
    for (auto &attr : stream.attributes()) {
        if (attr.name().compare(QString("id")) == 0) {
            id = attr.value().toInt();
            break;
        }
    }

    while (stream.readNextStartElement()) {
        if (stream.name() == "Make") {
            camera.setMake(streamToStdString(stream));
        } else if (stream.name() == "Model") {
            camera.setModel(stream.readElementText().toStdString());
        } else if (stream.name() == "SerialNumber") {
            camera.setSerialNumber(stream.readElementText().toStdString());
        } else if (stream.name() == "Type") {
            camera.setType(stream.readElementText().toStdString());
        } else if (stream.name() == "Focal") {
            camera.setFocal(streamToDouble(stream));
        } else if (stream.name() == "Width") {
            camera.setWidth(streamToInt(stream));
        } else if (stream.name() == "Height") {
            camera.setHeight(streamToInt(stream));
        } else if (stream.name() == "BitsPerPixel") {
            camera.setBitsPerPixel(streamToInt(stream));
        } else if (stream.name() == "SensorSize") {
            camera.setSensorSize(streamToDouble(stream));
        } else if (stream.name() == "BandName") {
            camera.setBandName(stream.readElementText().toStdString());
        } else if (stream.name() == "BlackLevel") {
            camera.setBlackLevel(streamToInt(stream));
        } else if (stream.name() == "CalibratedHMatrix") {
            auto hmatrix = tl::split<float>(stream.readElementText().toStdString(), ' ');
            if (hmatrix.size() == 9) {
                tl::Matrix3x3f H;
                H(0, 0) = hmatrix[0];
                H(0, 1) = hmatrix[1];
                H(0, 2) = hmatrix[2];
                H(1, 0) = hmatrix[3];
                H(1, 1) = hmatrix[4];
                H(1, 2) = hmatrix[5];
                H(2, 0) = hmatrix[6];
                H(2, 1) = hmatrix[7];
                H(2, 2) = hmatrix[8];
                camera.setCalibratedHMatrix(H);
            }

        } else if (stream.name() == "PriorCalibration") {
            readPriorCalibration(stream, camera);
        } else if (stream.name() == "Calibration") {
            readCalibration(stream, camera);
        } else if (stream.name() == "Vignetting") {
            readVignetting(stream, camera);
        } else
            stream.skipCurrentElement();
    }
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



void readFeatureConfig(QXmlStreamReader &stream, Project &project)
{
    QXmlStreamAttributes attrs = stream.attributes();
    std::string type = attrs.value("type").toString().toStdString();

    auto config = FeatureFactory::create(type);
    if (!config) {
        stream.skipCurrentElement();
        return;
    }

    while (stream.readNextStartElement()) {
        std::string key = stream.name().toString().toStdString();
        std::string value = stream.readElementText().toStdString();

        config->setProperty(key, value);
    }

    project.setFeatureConfig(config);
}

void readFeatureReport(QXmlStreamReader &stream, Project &project)
{
    FeatureExtractorReport report;

    while (stream.readNextStartElement()) {
        if (stream.name() == "Features") {
            report.features = stream.readElementText().toInt();
        } else if (stream.name() == "Time") {
            report.time = stream.readElementText().toDouble();
        } else if (stream.name() == "Cuda") {
            report.cuda = (stream.readElementText() == "true");
        } else {
            stream.skipCurrentElement();
        }
    }

    project.setFeatureReport(report);
}



void readFeatureFiles(QXmlStreamReader &stream, Project &project)
{
    auto &features = project.features();

    while (stream.readNextStartElement()) {
        if (stream.name() == "FeatFile") {
            QXmlStreamAttributes attrs = stream.attributes();

            // Atributo image_id
            uint64_t image_id = attrs.value("image_id").toULongLong();

            features.add(image_id, streamToStdString(stream));
        } else {
            stream.skipCurrentElement();
        }
    }
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
                        } else if (stream.name() == "Features") {
                            this->readFeatures(stream, project);
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
        /*if (stream.name() == "Path") {
            projectInfo.setProjectPath(streamToStdString(stream));
        } else*/ if (stream.name() == "Description") {
            projectInfo.setDescription(streamToStdString(stream));
        } else
            stream.skipCurrentElement();
    }
}

void ProjectReader::readCameras(QXmlStreamReader &stream, CameraRepository &cameraRepository)
{
    while (stream.readNextStartElement()) {
        if (stream.name() == "Camera") {
            Camera camera;
            readCamera(stream, camera);
            cameraRepository.add(camera);
        } else
            stream.skipCurrentElement();
    }
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

void ProjectReader::readFeatures(QXmlStreamReader &stream, Project &project)
{
    //while (stream.readNextStartElement()) {
    //    if (stream.name() == "FeatureExtractor") {
    //        readFeatureConfig(stream, project.featureConfig());
    //    } else if (stream.name() == "Report") {
    //        readFeatureReport(stream, project.featureReport());
    //    } else if (stream.name() == "Files") {
    //        readFeatureFiles(stream, project.features());
    //    } else
    //        stream.skipCurrentElement();
    //}
    while (stream.readNextStartElement()) {
        if (stream.name() == "FeatureExtractor") {
            readFeatureConfig(stream, project);
        } else if (stream.name() == "Report") {
            readFeatureReport(stream, project);
        } else if (stream.name() == "Files") {
            readFeatureFiles(stream, project);
        } else {
            stream.skipCurrentElement();
        }
    }
}

} // end namespace graphos
