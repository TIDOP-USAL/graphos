/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "CamerasModel.h"

#include "graphos/core/camera/colmap.h"

#include <tidop/core/messages.h>

#include <colmap/base/database.h>
#include <colmap/base/camera_models.h>

#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <fstream>

namespace graphos
{

CamerasModelImp::CamerasModelImp(Project *project,
                                 QObject *parent)
  : CamerasModel(parent),
    mProject(project),
    bModifiedProject(false),
    mActiveCameraId(0),
    mCameraCache()
{
  init();
}

int CamerasModelImp::addCamera(const tl::Camera &camera)
{
  return mProject->addCamera(camera);
}

int CamerasModelImp::cameraID(const tl::Camera &camera) const
{
  return cameraID(camera.make().c_str(), camera.model().c_str());
}

int CamerasModelImp::cameraID(const QString &make, 
                              const QString &model) const
{
  int id_camera = 0;
  for (auto it = begin(); it != end(); it++){
    QString camera_make = it->second.make().c_str();
    QString camera_model = it->second.model().c_str();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      id_camera = it->first;
      break;
    }
  }
  return id_camera;
}

tl::Camera CamerasModelImp::camera(int id) const
{
  tl::Camera camera;

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

tl::Camera CamerasModelImp::camera(const QString &make,
                                   const QString &model) const
{
  int camera_id = cameraID(make, model);
  return camera(camera_id);
}

int CamerasModelImp::currentCameraID() const
{
  return mActiveCameraId;
}

bool CamerasModelImp::updateCamera(int id, const tl::Camera &camera)
{
  return mProject->updateCamera(id, camera);
}

void CamerasModelImp::updateCurrentCameraMake(const QString &make)
{
  auto it = mCameraCache.find(mActiveCameraId);
  if (it != mCameraCache.end()) {
    mCameraCache[mActiveCameraId].setMake(make.toStdString());
    bModifiedProject = true;
  }
}

void CamerasModelImp::updateCurrentCameraModel(const QString &model)
{
  auto it = mCameraCache.find(mActiveCameraId);
  if (it != mCameraCache.end()) {
    mCameraCache[mActiveCameraId].setModel(model.toStdString());
    bModifiedProject = true;
  }
}

//void CamerasModelImp::updateCurrentCameraWidth(int width)
//{
//  mCameraCache[ mActiveCameraId ].setWidth(width);
//  bModifiedProject = true;
//}
//
//void CamerasModelImp::updateCurrentCameraHeight(int height)
//{
//  mCameraCache[ mActiveCameraId ].setHeight(height);
//  bModifiedProject = true;
//}

void CamerasModelImp::updateCurrentCameraSensorSize(const QString &sensorSize)
{
  auto it = mCameraCache.find(mActiveCameraId);
  if (it != mCameraCache.end()) {
    mCameraCache[mActiveCameraId].setSensorSize(sensorSize.toDouble());
    bModifiedProject = true;
  }
}

void CamerasModelImp::updateCurrentCameraFocal(const QString &focal)
{
  auto it = mCameraCache.find(mActiveCameraId);
  if (it != mCameraCache.end()) {
    mCameraCache[mActiveCameraId].setFocal(focal.toDouble());
    bModifiedProject = true;
  }
}

void CamerasModelImp::updateCurrentCameraType(const QString &type)
{
  try {

    auto it = mCameraCache.find(mActiveCameraId);
    if (it != mCameraCache.end()) {
      mCameraCache[mActiveCameraId].setType(type.toStdString());
      bModifiedProject = true;
      if (std::shared_ptr<tl::Calibration> calibration_old = mCameraCache[mActiveCameraId].calibration()) {

        std::shared_ptr<tl::Calibration> calibration = tl::CalibrationFactory::create(type.toStdString());

        for (auto param = calibration->parametersBegin();
             param != calibration->parametersEnd();
             param++) {

          tl::Calibration::Parameters param_name = param->first;
          double param_value = param->second;

          if (param_name == tl::Calibration::Parameters::focal && !calibration_old->existParameter(param_name)) {
            double focal = calibration_old->parameter(tl::Calibration::Parameters::focalx) + calibration_old->parameter(tl::Calibration::Parameters::focaly) / 2.;
            calibration->setParameter(param_name, focal);
          } else if ((param_name == tl::Calibration::Parameters::focalx || param_name == tl::Calibration::Parameters::focaly) &&
                     calibration_old->existParameter(tl::Calibration::Parameters::focal)) {
            calibration->setParameter(param_name, calibration_old->parameter(tl::Calibration::Parameters::focal));
          } else if (calibration_old->existParameter(param_name)) {
            calibration->setParameter(param_name, calibration_old->parameter(param_name));
          }

        }

        mCameraCache[mActiveCameraId].setCalibration(calibration);
        
      }
    }

  } catch (std::exception &e) {

  }
}

void CamerasModelImp::updateCurrentCameraCalibCx(double cx)
{
  updateCalibrationParameter(tl::Calibration::Parameters::cx, cx);
}

void CamerasModelImp::updateCurrentCameraCalibCy(double cy)
{
  updateCalibrationParameter(tl::Calibration::Parameters::cy, cy);
}

void CamerasModelImp::updateCurrentCameraCalibF(double f)
{
  updateCalibrationParameter(tl::Calibration::Parameters::focal, f);
}

void CamerasModelImp::updateCurrentCameraCalibFx(double fx)
{
  updateCalibrationParameter(tl::Calibration::Parameters::focalx, fx);
}

void CamerasModelImp::updateCurrentCameraCalibFy(double fy)
{
  updateCalibrationParameter(tl::Calibration::Parameters::focaly, fy);
}

void CamerasModelImp::updateCurrentCameraCalibK1(double k1)
{
  updateCalibrationParameter(tl::Calibration::Parameters::k1, k1);
}

void CamerasModelImp::updateCurrentCameraCalibK2(double k2)
{
  updateCalibrationParameter(tl::Calibration::Parameters::k2, k2);
}

void CamerasModelImp::updateCurrentCameraCalibK3(double k3)
{
  updateCalibrationParameter(tl::Calibration::Parameters::k3, k3);
}

void CamerasModelImp::updateCurrentCameraCalibK4(double k4)
{
  updateCalibrationParameter(tl::Calibration::Parameters::k4, k4);
}

void CamerasModelImp::updateCurrentCameraCalibK5(double k5)
{
  updateCalibrationParameter(tl::Calibration::Parameters::k5, k5);
}

void CamerasModelImp::updateCurrentCameraCalibK6(double k6)
{
  updateCalibrationParameter(tl::Calibration::Parameters::k6, k6);
}

void CamerasModelImp::updateCurrentCameraCalibP1(double p1)
{
  updateCalibrationParameter(tl::Calibration::Parameters::p1, p1);
}

void CamerasModelImp::updateCurrentCameraCalibP2(double p2)
{
  updateCalibrationParameter(tl::Calibration::Parameters::p2, p2);
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
        
          /// Hay que ver como es el formato si no se conoce la cámara
          /// ¿en lugar de mm se pone px?
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

            std::shared_ptr<tl::Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
            if (!calibration) {
              calibration = tl::CalibrationFactory::create(mCameraCache[mActiveCameraId].type());
              mCameraCache[mActiveCameraId].setCalibration(calibration);
            }

            while (stream.readNextStartElement()) {

              if (stream.name() == "projection") {
                stream.readElementText();
                TL_TODO("Comprobar tipo de cámara")
              } else if (stream.name() == "width") {
                mCameraCache[mActiveCameraId].setWidth(stream.readElementText().toInt());
              } else if (stream.name() == "height") {
                mCameraCache[mActiveCameraId].setHeight(stream.readElementText().toInt());
              } else if (stream.name() == "f") {
                calibration->setParameter(tl::Calibration::Parameters::focal, stream.readElementText().toDouble());
                calibration->setParameter(tl::Calibration::Parameters::focalx, stream.readElementText().toDouble());
                calibration->setParameter(tl::Calibration::Parameters::focaly, stream.readElementText().toDouble());
              } else if (stream.name() == "cx") {
                calibration->setParameter(tl::Calibration::Parameters::cx, stream.readElementText().toDouble());
              } else if (stream.name() == "cy") {
                calibration->setParameter(tl::Calibration::Parameters::cy, stream.readElementText().toDouble());
              } else if (stream.name() == "k1") {
                calibration->setParameter(tl::Calibration::Parameters::k1, stream.readElementText().toDouble());
              } else if (stream.name() == "k2") {
                calibration->setParameter(tl::Calibration::Parameters::k2, stream.readElementText().toDouble());
              } else if (stream.name() == "k3") {
                calibration->setParameter(tl::Calibration::Parameters::k3, stream.readElementText().toDouble());
              } else if (stream.name() == "p1") {
                calibration->setParameter(tl::Calibration::Parameters::p1, stream.readElementText().toDouble());
              } else if (stream.name() == "p2") {
                calibration->setParameter(tl::Calibration::Parameters::p2, stream.readElementText().toDouble());
              } else if (stream.name() == "date") {
              } else
                stream.skipCurrentElement();
            }

          } else {
            //stream.raiseError(QObject::tr("Incorrect Agisoft Calibration file"));
            msgError("Incorrect Agisoft Calibration file");
          }

        } else {
          //stream.raiseError(QObject::tr("Incorrect Agisoft Calibration file"));
          msgError("Incorrect Agisoft Calibration file");
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
                    QStringList list = text.trimmed().split(" ", QString::SkipEmptyParts);

                    std::shared_ptr<tl::Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
                    if (!calibration) {
                      calibration = tl::CalibrationFactory::create(mCameraCache[mActiveCameraId].type());
                      mCameraCache[mActiveCameraId].setCalibration(calibration);
                    }
                    calibration->setParameter(tl::Calibration::Parameters::focal, list[0].toDouble());
                    calibration->setParameter(tl::Calibration::Parameters::focalx, list[0].toDouble());
                    calibration->setParameter(tl::Calibration::Parameters::focaly, list[4].toDouble());
                    calibration->setParameter(tl::Calibration::Parameters::cx, list[2].toDouble() - 0.5);
                    calibration->setParameter(tl::Calibration::Parameters::cy, list[5].toDouble() - 0.5);

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
                    QStringList list = text.trimmed().split(" ", QString::SkipEmptyParts);

                    std::shared_ptr<tl::Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
                    calibration->setParameter(tl::Calibration::Parameters::k1, list[0].toDouble());
                    calibration->setParameter(tl::Calibration::Parameters::k2, list[1].toDouble());
                    calibration->setParameter(tl::Calibration::Parameters::p1, list[2].toDouble());
                    calibration->setParameter(tl::Calibration::Parameters::p2, list[3].toDouble());
                    calibration->setParameter(tl::Calibration::Parameters::k3, list[4].toDouble());

                  } else
                    stream.skipCurrentElement();
                }

              } else
                stream.skipCurrentElement();
            }

          } else {
            //stream.raiseError(QObject::tr("Incorrect OpenCV Calibration file"));
            msgError("Incorrect OpenCV Calibration file");
          }

        } else {
          //stream.raiseError(QObject::tr("Incorrect OpenCV Calibration file"));
          msgError("Incorrect OpenCV Calibration file");
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
  TL_TODO("Extraer a clases para la importación exportación")

  QFileInfo file_info(file);
  QDir parent_path(file_info.absolutePath());
  //tl::Path path(file.toStdString());
  //tl::Path parent_path = path.parentPath();

  if (!parent_path.exists()/*parent_path.exists()*/) {
    if (!parent_path.mkpath(".")/*path.createDirectories()*/) {
      std::string err = "The output directory cannot be created: ";
      err.append(file_info.absolutePath().toStdString());
      throw std::runtime_error(err);
    }
  }

  std::ofstream stream(file.toStdString(), std::ios::trunc);
  if (stream.is_open()) {

    if (format.compare("Pix4D") == 0) {

      tl::Camera camera = mCameraCache[mActiveCameraId];

      QString model_id = camera.type().c_str();
      if (model_id.compare("Pinhole 1") == 0 ||
          model_id.compare("Radial 1") == 0 ||
          model_id.compare("Radial 2") == 0 ||
          model_id.compare("Radial 3") == 0) {

        size_t sensor_width_px = std::max(camera.width(), camera.height());
        size_t sensor_width_mm = camera.sensorSize();
        double scale = sensor_width_mm == 1 ? 1. : static_cast<double>(sensor_width_mm) / static_cast<double>(sensor_width_px);
        double w = camera.width() * scale;
        double h = camera.height() * scale;

        std::shared_ptr<tl::Calibration> calibration = camera.calibration();
        double focal = calibration->parameter(tl::Calibration::Parameters::focal) * scale;

        ///TODO: ¿El 0 se refiere a un identificador de cámara??
        stream << "Pix4D camera calibration file 0\n";
        stream << "#Focal Length mm assuming a sensor width of " << w << "x" << h << "mm\n";
        stream << "F " << focal << "\n";
        stream << "#Principal Point mm" << "\n";
        stream << "Px " << calibration->parameter(tl::Calibration::Parameters::cx) * scale << "\n";
        stream << "Py " << calibration->parameter(tl::Calibration::Parameters::cy) * scale << "\n";
        stream << "#Symmetrical Lens Distortion Coeffs" << std::endl;
        stream << "K1 " << (calibration->existParameter(tl::Calibration::Parameters::k1) ? calibration->parameter(tl::Calibration::Parameters::k1) : 0.0) << "\n";
        stream << "K2 " << (calibration->existParameter(tl::Calibration::Parameters::k2) ? calibration->parameter(tl::Calibration::Parameters::k2) : 0.0) << "\n";
        stream << "K3 " << (calibration->existParameter(tl::Calibration::Parameters::k3) ? calibration->parameter(tl::Calibration::Parameters::k3) : 0.0) << "\n";
        stream << "#Tangential Lens Distortion Coeffs\n";
        stream << "T1 " << (calibration->existParameter(tl::Calibration::Parameters::p1) ? calibration->parameter(tl::Calibration::Parameters::p1) : 0.0) << "\n";
        stream << "T2 " << (calibration->existParameter(tl::Calibration::Parameters::p2) ? calibration->parameter(tl::Calibration::Parameters::p2) : 0.0) << "\n" << std::endl;
      } else {
        ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
      }

    } else if (format.compare("Agisoft") == 0) {

      tl::Camera camera = mCameraCache[mActiveCameraId];

      QString model_id = camera.type().c_str();
      if (model_id.compare("Pinhole 1") == 0 ||
          model_id.compare("Radial 1") == 0 ||
          model_id.compare("Radial 2") == 0 ||
          model_id.compare("Radial 3") == 0) {

        std::shared_ptr<tl::Calibration> calibration = camera.calibration();

        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        stream << "<calibration>\n";
        stream << "  <projection>frame</projection>\n";
        stream << "  <width>" << camera.width() << "</width>\n";
        stream << "  <height>" << camera.height() << "</height>\n";
        stream << "  <f>" << calibration->parameter(tl::Calibration::Parameters::focal) << "</f>\n";
        stream << "  <cx>" << static_cast<double>(camera.width()) / 2. - calibration->parameter(tl::Calibration::Parameters::cx) << "</cx>\n";
        stream << "  <cy>" << static_cast<double>(camera.height()) / 2. - calibration->parameter(tl::Calibration::Parameters::cy) << "</cy>\n";
        if (calibration->existParameter(tl::Calibration::Parameters::k1))
          stream << "  <k1>" << calibration->parameter(tl::Calibration::Parameters::k1) << "</k1>\n";
        if (calibration->existParameter(tl::Calibration::Parameters::k2))
          stream << "  <k2>" << calibration->parameter(tl::Calibration::Parameters::k2) << "</k2>\n";
        if (calibration->existParameter(tl::Calibration::Parameters::k3))
          stream << "  <k3>" << calibration->parameter(tl::Calibration::Parameters::k3) << "</k3>";
        if (calibration->existParameter(tl::Calibration::Parameters::p1))
          stream << "  <p1>" << calibration->parameter(tl::Calibration::Parameters::p1) << "</p1>\n";
        if (calibration->existParameter(tl::Calibration::Parameters::p2))
          stream << "  <p2>" << calibration->parameter(tl::Calibration::Parameters::p2) << "</p2>\n";
        stream << "  <date></date>\n";
        stream << "</calibration>\n";
      } else {
        ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
      }

    } else if (format.compare("OpenCV") == 0) {

      tl::Camera camera = mCameraCache[mActiveCameraId];

      QString model_id = camera.type().c_str();
      if (model_id.compare("Pinhole 1") == 0 ||
          model_id.compare("Radial 1") == 0 ||
          model_id.compare("Radial 2") == 0 ||
          model_id.compare("Radial 3") == 0) {

        std::shared_ptr<tl::Calibration> calibration = camera.calibration();

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
        stream << "    " << QString::number(calibration->parameter(tl::Calibration::Parameters::focal), 'e', 8).toStdString() << " 0. " << QString::number(calibration->parameter(tl::Calibration::Parameters::cx) + 0.5, 'e', 8).toStdString() << " 0. \n";
        stream << "    " << QString::number(calibration->parameter(tl::Calibration::Parameters::focal), 'e', 8).toStdString() << " " << QString::number(calibration->parameter(tl::Calibration::Parameters::cy) + 0.5, 'e', 8).toStdString() << " 0. 0. 1.\n";
        stream << "  </data>\n";
        stream << "</Camera_Matrix>\n";
        stream << "<Distortion_Coefficients type_id=\"opencv-matrix\">\n";
        stream << "  <rows>5</rows>\n";
        stream << "  <cols>1</cols>\n";
        stream << "  <dt>d</dt>\n";
        stream << "  <data>\n";
        stream << "    " << (calibration->existParameter(tl::Calibration::Parameters::k1) ? QString::number(calibration->parameter(tl::Calibration::Parameters::k1)).toStdString() : "0.0") << " "
          << (calibration->existParameter(tl::Calibration::Parameters::k2) ? QString::number(calibration->parameter(tl::Calibration::Parameters::k2), 'e', 8).toStdString() : "0.0") << " "
          << (calibration->existParameter(tl::Calibration::Parameters::p1) ? QString::number(calibration->parameter(tl::Calibration::Parameters::p1), 'e', 8).toStdString() : "0.0") << " "
          << (calibration->existParameter(tl::Calibration::Parameters::p2) ? QString::number(calibration->parameter(tl::Calibration::Parameters::p2), 'e', 8).toStdString() : "0.0") << " \n    "
          << (calibration->existParameter(tl::Calibration::Parameters::k3) ? QString::number(calibration->parameter(tl::Calibration::Parameters::k3), 'e', 8).toStdString() : "0.0") << "</data>";
        stream << "</Distortion_Coefficients>\n";
        stream << "</opencv_storage>\n";

      } else {
        ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
      }

    } else {

    }

    stream.close();
  }

}

bool CamerasModelImp::removeCamera(int id)
{
  return mProject->removeCamera(id);
}

bool CamerasModelImp::removeCamera(const tl::Camera &camera)
{
  int id_camera = cameraID(camera.make().c_str(), camera.model().c_str());
  return mProject->removeCamera(id_camera);
}

QStringList CamerasModelImp::imagesFromCamera(int id) const
{
  QStringList images;
  for(auto image = mProject->imageBegin(); image != mProject->imageEnd(); image++){
    if (image->cameraId() == id){
      images.push_back(image->path());
    }
  }

  return images;
}

CamerasModel::camera_iterator CamerasModelImp::begin()
{
  return mProject->cameraBegin();
}

CamerasModel::camera_const_iterator CamerasModelImp::begin() const
{
  return mProject->cameraBegin();
}

CamerasModel::camera_iterator CamerasModelImp::end()
{
  return mProject->cameraEnd();
}

CamerasModel::camera_const_iterator CamerasModelImp::end() const
{
  return mProject->cameraEnd();
}

void CamerasModelImp::save()
{
  if (bModifiedProject) {

    for (auto &camera : mCameraCache) {
      mProject->updateCamera(camera.first, camera.second);
      
      colmap::camera_t camera_id = static_cast<colmap::camera_t>(camera.first);
      colmap::Database database(mProject->database().toStdString());
      colmap::Camera camera_colmap = database.ReadCamera(camera_id);
      QString colmap_camera_type = cameraToColmapType(camera.second);
      //int camera_model_id = colmap::CameraModelNameToId(colmap_camera_type.toStdString());
      camera_colmap.SetModelIdFromName(colmap_camera_type.toStdString());
      database.UpdateCamera(camera_colmap);
    }

    bModifiedProject = false;
  }
}

//CamerasModel::image_iterator CamerasModelImp::imageBegin()
//{
//  return mProject->imageBegin();
//}
//
//CamerasModel::image_const_iterator CamerasModelImp::imageBegin() const
//{
//  return mProject->imageEnd();
//}
//
//CamerasModel::image_iterator CamerasModelImp::imageEnd()
//{
//  return mProject->imageEnd();
//}
//
//CamerasModel::image_const_iterator CamerasModelImp::imageEnd() const
//{
//  return mProject->imageEnd();
//}

void CamerasModelImp::init()
{

}

void CamerasModelImp::updateCalibrationParameter(tl::Calibration::Parameters param, double value)
{
  auto it = mCameraCache.find(mActiveCameraId);
  if (it != mCameraCache.end()) {
    std::shared_ptr<tl::Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
    if (calibration) {
      calibration->setParameter(param, value);
      bModifiedProject = true;
    }
  }
}

void CamerasModelImp::clear()
{
  mActiveCameraId = 0;
  mCameraCache.clear();
}

} // namespace graphos


