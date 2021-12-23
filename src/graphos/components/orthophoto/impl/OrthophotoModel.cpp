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

#include "OrthophotoModel.h"

#include "graphos/core/Orthophoto.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QFileInfo>

/// TODO: mover
#include <colmap/base/reconstruction.h>
#include <opencv2/calib3d.hpp>

namespace graphos
{

OrthophotoModelImp::OrthophotoModelImp(Project *project, QObject *parent)
  : OrthophotoModel(parent),
    mSettings(new QSettings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Graphos")),
    mParameters(new OrthophotoParameters),
    mProject(project)
{
  this->init();
}

OrthophotoModelImp::~OrthophotoModelImp()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }

  if (mParameters) {
    delete mParameters;
    mParameters = nullptr;
  }
}

void OrthophotoModelImp::loadSettings()
{
  if (mReadSettings) {
	  
    mParameters->setResolution(mSettings->value("ORTHOPHOTO/Resolution", mParameters->resolution()).toDouble());

	/* Read Settings here
	
	Example (replace PropertyName):
	
    mParameters->setPropertyName(mSettings->value("ORTHOPHOTO/PropertyName", mParameters->propertyName()).toInt());
  
    */

  }
}

void OrthophotoModelImp::saveSettings()
{
  if (mReadSettings) {
	
    mSettings->setValue("ORTHOPHOTO/Resolution", mParameters->resolution());

	/* Write Settings here

	Example:
	
    mSettings->setValue("ORTHOPHOTO/PropertyName", mParameters->propertyName());
  
    */
    
  }
}

OrthophotoParameters *OrthophotoModelImp::parameters() const
{
  return mParameters;
}

//std::vector<tl::Photo> OrthophotoModelImp::photos() const
//{
//  std::vector<tl::Photo> photos;
//
//  tl::Point3D offset;
//
//  std::ifstream ifs;
//  ifs.open(mProject->offset().toStdString(), std::ifstream::in);
//  if (ifs.is_open()) {
//
//    ifs >> offset.x >> offset.y >> offset.z;
//
//    ifs.close();
//  }
//
//  QString undistort_images_path = mProject->projectFolder();
//  std::shared_ptr<Densification> densification = mProject->densification();
//  std::map<int, tl::Camera> cameras;
//  if (densification->method() == Densification::Method::smvs) {
//    //undistort_images_path.append("/dense/");
//  } else if (densification->method() == Densification::Method::cmvs_pmvs) {
//    undistort_images_path.append("/dense/pmvs/visualize/");
//
//    for (auto camera = mProject->cameraBegin(); camera != mProject->cameraEnd(); camera++) {
//      if (cameras.find(camera->first) == cameras.end()) {
//
//        std::shared_ptr<tl::Calibration> calibration = camera->second.calibration();
//
//        float focal_x = 1.f;
//        float focal_y = 1.f;
//        {
//          for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++) {
//            tl::Calibration::Parameters parameter = param->first;
//            float value = static_cast<float>(param->second);
//            switch (parameter) {
//              case tl::Calibration::Parameters::focal:
//                focal_x = value;
//                focal_y = value;
//                break;
//              case tl::Calibration::Parameters::focalx:
//                focal_x = value;
//                break;
//              case tl::Calibration::Parameters::focaly:
//                focal_y = value;
//                break;
//              default:
//                break;
//            }
//          }
//        }
//
//        float ppx = static_cast<float>(calibration->parameter(tl::Calibration::Parameters::cx));
//        float ppy = static_cast<float>(calibration->parameter(tl::Calibration::Parameters::cy));
//        std::array<std::array<float, 3>, 3> camera_matrix_data = { focal_x, 0.f, ppx,
//                                                                  0.f, focal_y, ppy,
//                                                                  0.f, 0.f, 1.f };
//        cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32F, camera_matrix_data.data());
//
//        cv::Mat dist_coeffs = cv::Mat::zeros(1, 5, CV_32F);
//
//        for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++) {
//          tl::Calibration::Parameters parameter = param->first;
//          float value = static_cast<float>(param->second);
//          switch (parameter) {
//            case tl::Calibration::Parameters::k1:
//              dist_coeffs.at<float>(0) = value;
//              break;
//            case tl::Calibration::Parameters::k2:
//              dist_coeffs.at<float>(1) = value;
//              break;
//            case tl::Calibration::Parameters::k3:
//              dist_coeffs.at<float>(4) = value;
//              break;
//            case tl::Calibration::Parameters::k4:
//              dist_coeffs.at<float>(5) = value;
//              break;
//            case tl::Calibration::Parameters::k5:
//              dist_coeffs.at<float>(6) = value;
//              break;
//            case tl::Calibration::Parameters::k6:
//              dist_coeffs.at<float>(7) = value;
//              break;
//            case tl::Calibration::Parameters::p1:
//              dist_coeffs.at<float>(2) = value;
//              break;
//            case tl::Calibration::Parameters::p2:
//              dist_coeffs.at<float>(3) = value;
//              break;
//            default:
//              break;
//          }
//        }
//
//        cv::Size imageSize(static_cast<int>(camera->second.width()),
//                           static_cast<int>(camera->second.height()));
//
//        cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, dist_coeffs, imageSize, 1, imageSize, nullptr);
//
//        tl::Camera camera_dist = camera->second;
//        camera_dist.setFocal((optCameraMat.at<float>(0, 0)+ optCameraMat.at<float>(1, 1))/2.);
//        std::shared_ptr<tl::Calibration> calibration_dist = tl::CalibrationFactory::create(calibration->cameraModel());
//        calibration_dist->setParameter(tl::Calibration::Parameters::focal, (optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.);
//        calibration_dist->setParameter(tl::Calibration::Parameters::focalx, optCameraMat.at<float>(0, 0));
//        calibration_dist->setParameter(tl::Calibration::Parameters::focaly, optCameraMat.at<float>(1, 1));
//        calibration_dist->setParameter(tl::Calibration::Parameters::cx, optCameraMat.at<float>(0, 2));
//        calibration_dist->setParameter(tl::Calibration::Parameters::cy, optCameraMat.at<float>(1, 2));
//        camera_dist.setCalibration(calibration_dist);
//        cameras[camera->first] = camera_dist;
//
//      }
//    }
//  } else {
//    /// Devolver error o volver a corregir las imagenes
//  }
//
//  for (auto image = mProject->imageBegin(); image != mProject->imageEnd(); image++) {
//
//    QFileInfo file_info(image->path());
//    QString image_path = undistort_images_path;
//    image_path.append(file_info.fileName());
//
//    tl::Photo photo(image_path.toStdString());
//
//    if (mProject->isPhotoOriented(image->name())) {
//      CameraPose photoOrientation = mProject->photoOrientation(image->name());
//      auto rotation_matrix = photoOrientation.rotationMatrix();
//      rotation_matrix.at(1, 0) = -photoOrientation.rotationMatrix().at(1, 0);
//      rotation_matrix.at(1, 1) = -photoOrientation.rotationMatrix().at(1, 1);
//      rotation_matrix.at(1, 2) = -photoOrientation.rotationMatrix().at(1, 2);
//      rotation_matrix.at(2, 0) = -photoOrientation.rotationMatrix().at(2, 0);
//      rotation_matrix.at(2, 1) = -photoOrientation.rotationMatrix().at(2, 1);
//      rotation_matrix.at(2, 2) = -photoOrientation.rotationMatrix().at(2, 2);
//      photoOrientation.setRotationMatrix(rotation_matrix);
//
//      photoOrientation.setPosition(photoOrientation.position() + offset);
//
//      photo.setCameraPose(photoOrientation);
//
//      int camera_id = image->cameraId();
//      auto camera = cameras.find(camera_id);
//      if (camera != cameras.end()) {
//        photo.setCamera(camera->second);
//      } else continue;
//
//      photos.push_back(photo);
//    }
//
//  }
//
//  return photos;
//}

std::vector<tl::Photo> OrthophotoModelImp::photos() const
{
  std::vector<tl::Photo> photos;

  tl::Point3D offset;

  std::ifstream ifs;
  ifs.open(mProject->offset().toStdString(), std::ifstream::in);
  if (ifs.is_open()) {

    ifs >> offset.x >> offset.y >> offset.z;

    ifs.close();
  }

  //QString undistort_images_path = mProject->projectFolder();
  std::map<int, tl::Camera> cameras = mProject->cameras();
  //undistort_images_path.append("/dense/pmvs/visualize/");

  //for (auto camera = mProject->cameraBegin(); camera != mProject->cameraEnd(); camera++) {
  //  if (cameras.find(camera->first) == cameras.end()) {

  //    std::shared_ptr<tl::Calibration> calibration = camera->second.calibration();

  //    float focal_x = 1.f;
  //    float focal_y = 1.f;
  //    {
  //      for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++) {
  //        tl::Calibration::Parameters parameter = param->first;
  //        float value = static_cast<float>(param->second);
  //        switch (parameter) {
  //          case tl::Calibration::Parameters::focal:
  //            focal_x = value;
  //            focal_y = value;
  //            break;
  //          case tl::Calibration::Parameters::focalx:
  //            focal_x = value;
  //            break;
  //          case tl::Calibration::Parameters::focaly:
  //            focal_y = value;
  //            break;
  //          default:
  //            break;
  //        }
  //      }
  //    }

  //    float ppx = static_cast<float>(calibration->parameter(tl::Calibration::Parameters::cx));
  //    float ppy = static_cast<float>(calibration->parameter(tl::Calibration::Parameters::cy));
  //    std::array<std::array<float, 3>, 3> camera_matrix_data = { focal_x, 0.f, ppx,
  //                                                              0.f, focal_y, ppy,
  //                                                              0.f, 0.f, 1.f };
  //    cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32F, camera_matrix_data.data());

  //    cv::Mat dist_coeffs = cv::Mat::zeros(1, 5, CV_32F);

  //    for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++) {
  //      tl::Calibration::Parameters parameter = param->first;
  //      float value = static_cast<float>(param->second);
  //      switch (parameter) {
  //        case tl::Calibration::Parameters::k1:
  //          dist_coeffs.at<float>(0) = value;
  //          break;
  //        case tl::Calibration::Parameters::k2:
  //          dist_coeffs.at<float>(1) = value;
  //          break;
  //        case tl::Calibration::Parameters::k3:
  //          dist_coeffs.at<float>(4) = value;
  //          break;
  //        case tl::Calibration::Parameters::k4:
  //          dist_coeffs.at<float>(5) = value;
  //          break;
  //        case tl::Calibration::Parameters::k5:
  //          dist_coeffs.at<float>(6) = value;
  //          break;
  //        case tl::Calibration::Parameters::k6:
  //          dist_coeffs.at<float>(7) = value;
  //          break;
  //        case tl::Calibration::Parameters::p1:
  //          dist_coeffs.at<float>(2) = value;
  //          break;
  //        case tl::Calibration::Parameters::p2:
  //          dist_coeffs.at<float>(3) = value;
  //          break;
  //        default:
  //          break;
  //      }
  //    }

  //    cv::Size imageSize(static_cast<int>(camera->second.width()),
  //                       static_cast<int>(camera->second.height()));

  //    cv::Mat optCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, dist_coeffs, imageSize, 1, imageSize, nullptr);

  //    tl::Camera camera_dist = camera->second;
  //    camera_dist.setFocal((optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.);
  //    std::shared_ptr<tl::Calibration> calibration_dist = tl::CalibrationFactory::create(calibration->cameraModel());
  //    calibration_dist->setParameter(tl::Calibration::Parameters::focal, (optCameraMat.at<float>(0, 0) + optCameraMat.at<float>(1, 1)) / 2.);
  //    calibration_dist->setParameter(tl::Calibration::Parameters::focalx, optCameraMat.at<float>(0, 0));
  //    calibration_dist->setParameter(tl::Calibration::Parameters::focaly, optCameraMat.at<float>(1, 1));
  //    calibration_dist->setParameter(tl::Calibration::Parameters::cx, optCameraMat.at<float>(0, 2));
  //    calibration_dist->setParameter(tl::Calibration::Parameters::cy, optCameraMat.at<float>(1, 2));
  //    camera_dist.setCalibration(calibration_dist);
  //    cameras[camera->first] = camera_dist;

  //  }
  //}

  for (auto image = mProject->imageBegin(); image != mProject->imageEnd(); image++) {

    //QFileInfo file_info(image->path());
    //QString image_path = undistort_images_path;
    //image_path.append(file_info.fileName());

    tl::Photo photo(image->path().toStdString());

    if (mProject->isPhotoOriented(image->name())) {
      CameraPose photoOrientation = mProject->photoOrientation(image->name());
      auto rotation_matrix = photoOrientation.rotationMatrix();
      rotation_matrix.at(1, 0) = -photoOrientation.rotationMatrix().at(1, 0);
      rotation_matrix.at(1, 1) = -photoOrientation.rotationMatrix().at(1, 1);
      rotation_matrix.at(1, 2) = -photoOrientation.rotationMatrix().at(1, 2);
      rotation_matrix.at(2, 0) = -photoOrientation.rotationMatrix().at(2, 0);
      rotation_matrix.at(2, 1) = -photoOrientation.rotationMatrix().at(2, 1);
      rotation_matrix.at(2, 2) = -photoOrientation.rotationMatrix().at(2, 2);
      photoOrientation.setRotationMatrix(rotation_matrix);

      photoOrientation.setPosition(photoOrientation.position() + offset);

      photo.setCameraPose(photoOrientation);

      int camera_id = image->cameraId();
      auto camera = cameras.find(camera_id);
      if (camera != cameras.end()) {
        photo.setCamera(camera->second);
      } else continue;

      photos.push_back(photo);
    }

  }

  return photos;
}

QString OrthophotoModelImp::orthoPath() const
{
  return mProject->projectFolder() + "/ortho";
}

QString OrthophotoModelImp::dtmPath() const
{
  return mProject->dtmPath();
}

QString OrthophotoModelImp::epsCode() const
{
  return mProject->crs();
}

void OrthophotoModelImp::clearProject()
{
  /// TODO: 
  //mProject->clearOrtho();
}

bool OrthophotoModelImp::useCuda() const
{
  return mSettings->value("General/UseCuda", true).toBool();
}

void OrthophotoModelImp::init()
{
  mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void OrthophotoModelImp::clear()
{

}

} // namespace graphos
