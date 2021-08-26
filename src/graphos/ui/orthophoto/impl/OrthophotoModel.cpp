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

  QString undistort_images_path = mProject->projectFolder();
  std::shared_ptr<Densification> densification = mProject->densification();
  if (densification->method() == Densification::Method::smvs) {
    undistort_images_path.append("/dense/pmvs/visualize/");
  } else if (densification->method() == Densification::Method::cmvs_pmvs) {
    //undistort_images_path.append("/dense/");
  } else {
    /// Devolver error o volver a corregir las imagenes
  }

  for (auto image = mProject->imageBegin(); image != mProject->imageEnd(); image++) {

    QFileInfo file_info(image->path());
    QString image_path = undistort_images_path;
    image_path.append(file_info.fileName());

    tl::Photo photo(image_path.toStdString());

    CameraPose photoOrientation = mProject->photoOrientation(image->name());
    photoOrientation.rotation.at(1, 0) = -photoOrientation.rotation.at(1, 0);
    photoOrientation.rotation.at(1, 1) = -photoOrientation.rotation.at(1, 1);
    photoOrientation.rotation.at(1, 2) = -photoOrientation.rotation.at(1, 2);
    photoOrientation.rotation.at(2, 0) = -photoOrientation.rotation.at(2, 0);
    photoOrientation.rotation.at(2, 1) = -photoOrientation.rotation.at(2, 1);
    photoOrientation.rotation.at(2, 2) = -photoOrientation.rotation.at(2, 2);

    photoOrientation.position += offset;

    tl::Photo::Orientation orientation(photoOrientation.position, photoOrientation.rotation);
    photo.setOrientation(orientation);

    int camera_id = image->cameraId();
    tl::Camera camera = mProject->findCamera(camera_id);
    photo.setCamera(camera);
    
    photos.push_back(photo);

    
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

void OrthophotoModelImp::init()
{
  mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void OrthophotoModelImp::clear()
{

}

} // namespace graphos
