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

#include "CamerasModel.h"

#include <tidop/core/msg/message.h>

#include <QXmlStreamReader>
#include <QDir>

#include <fstream>

namespace graphos
{

CamerasModelImp::CamerasModelImp(Project *project,
                                 QObject *parent)
  : CamerasModel(parent),
    mProject(project),
    mModifiedProject(false),
    mActiveCameraId(0),
    mCameraCache()
{
    CamerasModelImp::init();
}

auto CamerasModelImp::cameras() const -> const std::map<int, Camera>&
{
    return mProject->cameras();
}

auto CamerasModelImp::addCamera(const Camera &camera) -> int
{
    return mProject->addCamera(camera);
}

auto CamerasModelImp::cameraID(const Camera &camera) const -> int
{
    return cameraID(camera.make().c_str(), camera.model().c_str());
}

auto CamerasModelImp::cameraID(const QString &make,
                               const QString &model) const -> int
{
    int id_camera = 0;

    QString camera_make;
    QString camera_model;

    for (const auto &camera : mProject->cameras()) {

        camera_make.fromStdString(camera.second.make());
        camera_model.fromStdString(camera.second.model());

        if (make.compare(camera_make) == 0 &&
            model.compare(camera_model) == 0) {
            id_camera = camera.first;
            break;
        }
    }

    return id_camera;
}

auto CamerasModelImp::camera(int id) const -> Camera
{
    Camera camera;

    auto it = mCameraCache.find(id);
    if (it != mCameraCache.end()) {
        camera = it->second;
    } else {
        camera = mProject->findCamera(id);
        mCameraCache[id] = camera;
    }

    mActiveCameraId = id;

    return camera;
}

auto CamerasModelImp::camera(const QString &make,
                             const QString &model) const -> Camera
{
    int camera_id = cameraID(make, model);
    return camera(camera_id);
}

auto CamerasModelImp::currentCameraID() const -> int
{
    return mActiveCameraId;
}

auto CamerasModelImp::updateCamera(int id, const Camera &camera) -> bool
{
    return mProject->updateCamera(id, camera);
}

void CamerasModelImp::updateCurrentCameraMake(const QString &make)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setMake(make.toStdString());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraModel(const QString &model)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setModel(model.toStdString());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraSensorSize(const QString &sensorSize)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setSensorSize(sensorSize.toDouble());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraFocal(const QString &focal)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        mCameraCache[mActiveCameraId].setFocal(focal.toDouble());
        mModifiedProject = true;
    }
}

void CamerasModelImp::updateCurrentCameraType(const QString &type)
{
    try {

        auto it = mCameraCache.find(mActiveCameraId);
        if (it != mCameraCache.end()) {
            mCameraCache[mActiveCameraId].setType(type.toStdString());
            mModifiedProject = true;
            if (auto calibration_old = mCameraCache[mActiveCameraId].calibration()) {

                auto calibration = CalibrationFactory::create(type.toStdString());

                for (const auto &param : *calibration){
                    Calibration::Parameters param_name = param.first;

                    if (param_name == Calibration::Parameters::focal && !calibration_old->existParameter(param_name)) {
                        double focal = calibration_old->parameter(Calibration::Parameters::focalx) + calibration_old->parameter(Calibration::Parameters::focaly) / 2.;
                        calibration->setParameter(param_name, focal);
                    } else if ((param_name == Calibration::Parameters::focalx || param_name == Calibration::Parameters::focaly) &&
                               calibration_old->existParameter(Calibration::Parameters::focal)) {
                        calibration->setParameter(param_name, calibration_old->parameter(Calibration::Parameters::focal));
                    } else if (calibration_old->existParameter(param_name)) {
                        calibration->setParameter(param_name, calibration_old->parameter(param_name));
                    }

                }

                mCameraCache[mActiveCameraId].setCalibration(calibration);

            }
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

void CamerasModelImp::updateCurrentCameraCalibCx(double cx)
{
    updateCalibrationParameter(Calibration::Parameters::cx, cx);
}

void CamerasModelImp::updateCurrentCameraCalibCy(double cy)
{
    updateCalibrationParameter(Calibration::Parameters::cy, cy);
}

void CamerasModelImp::updateCurrentCameraCalibF(double f)
{
    updateCalibrationParameter(Calibration::Parameters::focal, f);
}

void CamerasModelImp::updateCurrentCameraCalibFx(double fx)
{
    updateCalibrationParameter(Calibration::Parameters::focalx, fx);
}

void CamerasModelImp::updateCurrentCameraCalibFy(double fy)
{
    updateCalibrationParameter(Calibration::Parameters::focaly, fy);
}

void CamerasModelImp::updateCurrentCameraCalibK1(double k1)
{
    updateCalibrationParameter(Calibration::Parameters::k1, k1);
}

void CamerasModelImp::updateCurrentCameraCalibK2(double k2)
{
    updateCalibrationParameter(Calibration::Parameters::k2, k2);
}

void CamerasModelImp::updateCurrentCameraCalibK3(double k3)
{
    updateCalibrationParameter(Calibration::Parameters::k3, k3);
}

void CamerasModelImp::updateCurrentCameraCalibK4(double k4)
{
    updateCalibrationParameter(Calibration::Parameters::k4, k4);
}

void CamerasModelImp::updateCurrentCameraCalibK5(double k5)
{
    updateCalibrationParameter(Calibration::Parameters::k5, k5);
}

void CamerasModelImp::updateCurrentCameraCalibK6(double k6)
{
    updateCalibrationParameter(Calibration::Parameters::k6, k6);
}

void CamerasModelImp::updateCurrentCameraCalibP1(double p1)
{
    updateCalibrationParameter(Calibration::Parameters::p1, p1);
}

void CamerasModelImp::updateCurrentCameraCalibP2(double p2)
{
    updateCalibrationParameter(Calibration::Parameters::p2, p2);
}

void CamerasModelImp::calibrationImport(const QString &file,
                                        const QString &format)
{

    if (QFileInfo(file).exists()) {

        if (format.compare("Pix4D") == 0) {

            std::ifstream ifs;
            ifs.open(file.toStdString(), std::ifstream::in);

            if (ifs.is_open()) {

                std::string line;
                while (std::getline(ifs, line)) {

                    /// Hay que ver como es el formato si no se conoce la c�mara
                    /// �en lugar de mm se pone px?
                }

                ifs.close();
            }

        } else if (format.compare("Agisoft") == 0) {

            QFile input(file);
            if (input.open(QIODevice::ReadOnly)) {

                QXmlStreamReader stream;
                stream.setDevice(&input);

                if (stream.readNextStartElement()) {

                    if (stream.name() == "calibration") {

                        std::shared_ptr<Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
                        if (!calibration) {
                            calibration = CalibrationFactory::create(mCameraCache[mActiveCameraId].type());
                            mCameraCache[mActiveCameraId].setCalibration(calibration);
                        }

                        while (stream.readNextStartElement()) {

                            if (stream.name() == "projection") {
                                stream.readElementText();
                                TL_TODO("Comprobar tipo de c�mara")
                            } else if (stream.name() == "width") {
                                    mCameraCache[mActiveCameraId].setWidth(stream.readElementText().toInt());
                                } else if (stream.name() == "height") {
                                    mCameraCache[mActiveCameraId].setHeight(stream.readElementText().toInt());
                                } else if (stream.name() == "f") {
                                    calibration->setParameter(Calibration::Parameters::focal, stream.readElementText().toDouble());
                                    calibration->setParameter(Calibration::Parameters::focalx, stream.readElementText().toDouble());
                                    calibration->setParameter(Calibration::Parameters::focaly, stream.readElementText().toDouble());
                                } else if (stream.name() == "cx") {
                                    calibration->setParameter(Calibration::Parameters::cx, stream.readElementText().toDouble());
                                } else if (stream.name() == "cy") {
                                    calibration->setParameter(Calibration::Parameters::cy, stream.readElementText().toDouble());
                                } else if (stream.name() == "k1") {
                                    calibration->setParameter(Calibration::Parameters::k1, stream.readElementText().toDouble());
                                } else if (stream.name() == "k2") {
                                    calibration->setParameter(Calibration::Parameters::k2, stream.readElementText().toDouble());
                                } else if (stream.name() == "k3") {
                                    calibration->setParameter(Calibration::Parameters::k3, stream.readElementText().toDouble());
                                } else if (stream.name() == "p1") {
                                    calibration->setParameter(Calibration::Parameters::p1, stream.readElementText().toDouble());
                                } else if (stream.name() == "p2") {
                                    calibration->setParameter(Calibration::Parameters::p2, stream.readElementText().toDouble());
                                } else if (stream.name() == "date") {
                                } else
                                    stream.skipCurrentElement();
                        }

                    } else {
                        tl::Message::error("Incorrect Agisoft Calibration file");
                    }

                } else {
                    tl::Message::error("Incorrect Agisoft Calibration file");
                }

                input.close();
            }

        } else if (format.compare("OpenCV") == 0) {

            QFile input(file);
            if (input.open(QIODevice::ReadOnly)) {

                QXmlStreamReader stream;
                stream.setDevice(&input);

                if (stream.readNextStartElement()) {

                    if (stream.name() == "opencv_storage") {

                        while (stream.readNextStartElement()) {
                            if (stream.name() == "image_Width") {
                                mCameraCache[mActiveCameraId].setWidth(stream.readElementText().toInt());
                            } else if (stream.name() == "image_Height") {
                                mCameraCache[mActiveCameraId].setHeight(stream.readElementText().toInt());
                            } else if (stream.name() == "Camera_Matrix") {

                                while (stream.readNextStartElement()) {
                                    if (stream.name() == "rows") {
                                        QString rows = stream.readElementText();
                                    } else if (stream.name() == "cols") {
                                        QString cols = stream.readElementText();
                                    } else if (stream.name() == "data") {

                                        QString text = stream.readElementText();
                                        QStringList list = text.trimmed().split(" ", Qt::SkipEmptyParts);

                                        std::shared_ptr<Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
                                        if (!calibration) {
                                            calibration = CalibrationFactory::create(mCameraCache[mActiveCameraId].type());
                                            mCameraCache[mActiveCameraId].setCalibration(calibration);
                                        }
                                        calibration->setParameter(Calibration::Parameters::focal, list[0].toDouble());
                                        calibration->setParameter(Calibration::Parameters::focalx, list[0].toDouble());
                                        calibration->setParameter(Calibration::Parameters::focaly, list[4].toDouble());
                                        calibration->setParameter(Calibration::Parameters::cx, list[2].toDouble() - 0.5);
                                        calibration->setParameter(Calibration::Parameters::cy, list[5].toDouble() - 0.5);

                                    } else
                                        stream.skipCurrentElement();
                                }

                            } else if (stream.name() == "Distortion_Coefficients") {

                                while (stream.readNextStartElement()) {
                                    if (stream.name() == "rows") {
                                        QString rows = stream.readElementText();
                                    } else if (stream.name() == "cols") {
                                        QString cols = stream.readElementText();
                                    } else if (stream.name() == "data") {


                                        QString text = stream.readElementText();
                                        QStringList list = text.trimmed().split(" ", Qt::SkipEmptyParts);

                                        std::shared_ptr<Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
                                        calibration->setParameter(Calibration::Parameters::k1, list[0].toDouble());
                                        calibration->setParameter(Calibration::Parameters::k2, list[1].toDouble());
                                        calibration->setParameter(Calibration::Parameters::p1, list[2].toDouble());
                                        calibration->setParameter(Calibration::Parameters::p2, list[3].toDouble());
                                        calibration->setParameter(Calibration::Parameters::k3, list[4].toDouble());

                                    } else
                                        stream.skipCurrentElement();
                                }

                            } else
                                stream.skipCurrentElement();
                        }

                    } else {
                        tl::Message::error("Incorrect OpenCV Calibration file");
                    }

                } else {
                    tl::Message::error("Incorrect OpenCV Calibration file");
                }

                input.close();
            }

        } else {

        }

    }
}

void CamerasModelImp::calibrationExport(const QString &file,
                                        const QString &format)
{
    TL_TODO("Extraer a clases para la importaci�n exportaci�n")

    QFileInfo file_info(file);
    QDir parent_path(file_info.absolutePath());

    if (!parent_path.exists()) {
        if (!parent_path.mkpath(".")) {
            std::string err = "The output directory cannot be created: ";
            err.append(file_info.absolutePath().toStdString());
            throw std::runtime_error(err);
        }
    }

    std::ofstream stream(file.toStdString(), std::ios::trunc);
    if (stream.is_open()) {

        if (format.compare("Pix4D") == 0) {

            Camera camera = mCameraCache[mActiveCameraId];

            QString model_id = camera.type().c_str();
            if (model_id.compare("Pinhole 1") == 0 ||
                model_id.compare("Radial 1") == 0 ||
                model_id.compare("Radial 2") == 0 /*||
                model_id.compare("Radial 3") == 0*/) {

                size_t sensor_width_px = std::max(camera.width(), camera.height());
                size_t sensor_width_mm = static_cast<size_t>(camera.sensorSize());
                double scale = sensor_width_mm == 1 ? 1. : static_cast<double>(sensor_width_mm) / static_cast<double>(sensor_width_px);
                double w = camera.width() * scale;
                double h = camera.height() * scale;

                std::shared_ptr<Calibration> calibration = camera.calibration();
                double focal = calibration->parameter(Calibration::Parameters::focal) * scale;

                ///TODO: �El 0 se refiere a un identificador de c�mara??
                stream << "Pix4D camera calibration file 0\n";
                stream << "#Focal Length mm assuming a sensor width of " << w << "x" << h << "mm\n";
                stream << "F " << focal << "\n";
                stream << "#Principal Point mm" << "\n";
                stream << "Px " << calibration->parameter(Calibration::Parameters::cx) * scale << "\n";
                stream << "Py " << calibration->parameter(Calibration::Parameters::cy) * scale << "\n";
                stream << "#Symmetrical Lens Distortion Coeffs" << std::endl;
                stream << "K1 " << (calibration->existParameter(Calibration::Parameters::k1) ? calibration->parameter(Calibration::Parameters::k1) : 0.0) << "\n";
                stream << "K2 " << (calibration->existParameter(Calibration::Parameters::k2) ? calibration->parameter(Calibration::Parameters::k2) : 0.0) << "\n";
                stream << "K3 " << (calibration->existParameter(Calibration::Parameters::k3) ? calibration->parameter(Calibration::Parameters::k3) : 0.0) << "\n";
                stream << "#Tangential Lens Distortion Coeffs\n";
                stream << "T1 " << (calibration->existParameter(Calibration::Parameters::p1) ? calibration->parameter(Calibration::Parameters::p1) : 0.0) << "\n";
                stream << "T2 " << (calibration->existParameter(Calibration::Parameters::p2) ? calibration->parameter(Calibration::Parameters::p2) : 0.0) << "\n" << std::endl;
            } else {
                ///TODO: C�mara no soportada. �devolver error, escribir mensaje de error, ...?
            }

        } else if (format.compare("Agisoft") == 0) {

            Camera camera = mCameraCache[mActiveCameraId];

            QString model_id = camera.type().c_str();
            if (model_id.compare("Pinhole 1") == 0 ||
                model_id.compare("Radial 1") == 0 ||
                model_id.compare("Radial 2") == 0 ||
                /*model_id.compare("Radial 3") == 0 ||*/
                model_id.compare("OpenCV 1") == 0 ||
                model_id.compare("OpenCV 2") == 0) {

                std::shared_ptr<Calibration> calibration = camera.calibration();

                stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
                stream << "<calibration>\n";
                stream << "  <projection>frame</projection>\n";
                stream << "  <width>" << camera.width() << "</width>\n";
                stream << "  <height>" << camera.height() << "</height>\n";
                stream << "  <f>" << calibration->parameter(Calibration::Parameters::focal) << "</f>\n";
                stream << "  <cx>" << static_cast<double>(camera.width()) / 2. - calibration->parameter(Calibration::Parameters::cx) << "</cx>\n";
                stream << "  <cy>" << static_cast<double>(camera.height()) / 2. - calibration->parameter(Calibration::Parameters::cy) << "</cy>\n";
                if (calibration->existParameter(Calibration::Parameters::k1))
                    stream << "  <k1>" << calibration->parameter(Calibration::Parameters::k1) << "</k1>\n";
                if (calibration->existParameter(Calibration::Parameters::k2))
                    stream << "  <k2>" << calibration->parameter(Calibration::Parameters::k2) << "</k2>\n";
                if (calibration->existParameter(Calibration::Parameters::k3))
                    stream << "  <k3>" << calibration->parameter(Calibration::Parameters::k3) << "</k3>";
                if (calibration->existParameter(Calibration::Parameters::p1))
                    stream << "  <p1>" << calibration->parameter(Calibration::Parameters::p1) << "</p1>\n";
                if (calibration->existParameter(Calibration::Parameters::p2))
                    stream << "  <p2>" << calibration->parameter(Calibration::Parameters::p2) << "</p2>\n";
                stream << "  <date></date>\n";
                stream << "</calibration>\n";
            } else {
                tl::Message::error("Unsupported camera type");
            }

        } else if (format.compare("OpenCV") == 0) {

            Camera camera = mCameraCache[mActiveCameraId];

            QString model_id = camera.type().c_str();
            if (model_id.compare("Pinhole 1") == 0 ||
                model_id.compare("Radial 1") == 0 ||
                model_id.compare("Radial 2") == 0 ||
                /*model_id.compare("Radial 3") == 0 ||*/
                model_id.compare("OpenCV 1") == 0 ||
                model_id.compare("OpenCV 2") == 0) {

                std::shared_ptr<Calibration> calibration = camera.calibration();
                bool b_fisheye = calibration->checkCameraType(Calibration::CameraType::fisheye);

                stream << "<?xml version=\"1.0\"?>\n";
                stream << "<opencv_storage>\n";
                stream << "<calibration_Time></calibration_Time>\n";
                stream << "<image_Width>" << camera.width() << "</image_Width>\n";
                stream << "<image_Height>" << camera.height() << "</image_Height>\n";
                stream << "<Camera_Matrix type_id=\"opencv-matrix\">\n";
                stream << "  <rows>3</rows>\n";
                stream << "  <cols>3</cols>\n";
                stream << "  <dt>d</dt>\n";
                stream << "  <data>\n";
                stream << "    " << QString::number(calibration->existParameter(Calibration::Parameters::focal) ?
                                                    calibration->parameter(Calibration::Parameters::focal) :
                                                    calibration->parameter(Calibration::Parameters::focalx), 'e', 8).toStdString() << " 0. " << QString::number(calibration->parameter(Calibration::Parameters::cx) + 0.5, 'e', 8).toStdString() << " 0. \n";
                stream << "    " << QString::number(calibration->existParameter(Calibration::Parameters::focal) ?
                                                    calibration->parameter(Calibration::Parameters::focal) :
                                                    calibration->parameter(Calibration::Parameters::focaly), 'e', 8).toStdString() << " " << QString::number(calibration->parameter(Calibration::Parameters::cy) + 0.5, 'e', 8).toStdString() << " 0. 0. 1.\n";
                stream << "  </data>\n";
                stream << "</Camera_Matrix>\n";
                stream << "<Distortion_Coefficients type_id=\"opencv-matrix\">\n";
                stream << "  <rows>5</rows>\n";
                stream << "  <cols>1</cols>\n";
                stream << "  <dt>d</dt>\n";
                stream << "  <data>\n";
                stream << "    " << (calibration->existParameter(Calibration::Parameters::k1) ? QString::number(calibration->parameter(Calibration::Parameters::k1), 'e', 8).toStdString() : "0.0") << " "
                    << (calibration->existParameter(Calibration::Parameters::k2) ? QString::number(calibration->parameter(Calibration::Parameters::k2), 'e', 8).toStdString() : "0.0") << " ";

                if (b_fisheye) {
                    stream << (calibration->existParameter(Calibration::Parameters::k3) ? QString::number(calibration->parameter(Calibration::Parameters::k3), 'e', 8).toStdString() : "0.0") << " "
                        << (calibration->existParameter(Calibration::Parameters::k4) ? QString::number(calibration->parameter(Calibration::Parameters::k4), 'e', 8).toStdString() : "0.0");
                } else {
                    stream << (calibration->existParameter(Calibration::Parameters::p1) ? QString::number(calibration->parameter(Calibration::Parameters::p1), 'e', 8).toStdString() : "0.0") << " "
                        << (calibration->existParameter(Calibration::Parameters::p2) ? QString::number(calibration->parameter(Calibration::Parameters::p2), 'e', 8).toStdString() : "0.0") << " \n    "
                        << (calibration->existParameter(Calibration::Parameters::k3) ? QString::number(calibration->parameter(Calibration::Parameters::k3), 'e', 8).toStdString() : "0.0");
                }
                stream << "</data>";
                stream << "</Distortion_Coefficients>\n";
                stream << "</opencv_storage>\n";

            } else {
                tl::Message::error("Unsupported camera type");
            }

        } else {

        }

        stream.close();
    }

}

auto CamerasModelImp::removeCamera(int id) -> bool
{
    return mProject->removeCamera(id);
}

auto CamerasModelImp::removeCamera(const Camera &camera) -> bool
{
    int id_camera = cameraID(camera.make().c_str(), camera.model().c_str());
    return mProject->removeCamera(id_camera);
}

auto CamerasModelImp::imagesFromCamera(int id) const -> QStringList
{
    QStringList images_list;
    auto &images = mProject->images();
    for (const auto &image : images) {
        if (image.second.cameraId() == id) {
            images_list.push_back(image.second.path());
        }
    }

    return images_list;
}

void CamerasModelImp::save()
{
    if (mModifiedProject) {

        for (auto &camera : mCameraCache) {
            mProject->updateCamera(camera.first, camera.second);
        }

        mModifiedProject = false;
    }
}

bool CamerasModelImp::modified()
{
    return mModifiedProject;
}

void CamerasModelImp::init()
{

}

void CamerasModelImp::updateCalibrationParameter(Calibration::Parameters param, double value)
{
    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
        auto calibration = mCameraCache[mActiveCameraId].calibration();
        if (calibration) {
            calibration->setParameter(param, value);
            mModifiedProject = true;
        }
    }
}

void CamerasModelImp::clear()
{
    mActiveCameraId = 0;
    mCameraCache.clear();
    mModifiedProject = false;
}

} // namespace graphos


