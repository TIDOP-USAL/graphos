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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "LoadFromVideoModel.h"

//#include "graphos/core/LoadFromVideo.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>

namespace graphos
{

LoadFromVideoModelImp::LoadFromVideoModelImp(Project *project, 
                                             QObject *parent)
  : LoadFromVideoModel(parent),
    //mSettings(new QSettings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Graphos")),
    //mParameters(new LoadFromVideoParameters),
    mProject(project)
{
  this->init();
}

LoadFromVideoModelImp::~LoadFromVideoModelImp()
{
  //if (mSettings){
  //  delete mSettings;
  //  mSettings = nullptr;
  //}

  //if (mParameters) {
  //  delete mParameters;
  //  mParameters = nullptr;
  //}
}

QString LoadFromVideoModelImp::imagesPath() const
{
  return QString(mProject->projectFolder()).append("/images");
}

void LoadFromVideoModelImp::addImage(const Image &image)
{
  mProject->addImage(image);
}

const std::map<int, Camera> &LoadFromVideoModelImp::cameras() const
{
  return mProject->cameras();
}

int LoadFromVideoModelImp::addCamera(const Camera &camera)
{
  return mProject->addCamera(camera);
}

int LoadFromVideoModelImp::cameraID(const Camera &camera) const
{
  return cameraID(camera.make().c_str(), camera.model().c_str());
}

int LoadFromVideoModelImp::cameraID(const QString &make,
                                    const QString &model) const
{
  int id_camera = 0;
  for (const auto &camera : mProject->cameras()) {

    QString camera_make = camera.second.make().c_str();
    QString camera_model = camera.second.model().c_str();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0) {
      id_camera = camera.first;
      break;
    }
  }
  return id_camera;
}

//void LoadFromVideoModelImp::loadSettings()
//{
//  if (mReadSettings) {
//	  
//	/* Read Settings here
//	
//	Example (replace PropertyName):
//	
//    mParameters->setPropertyName(mSettings->value("LOADFROMVIDEO/PropertyName", mParameters->propertyName()).toInt());
//  
//    */
//
//  }
//}

//void LoadFromVideoModelImp::saveSettings()
//{
//  if (mReadSettings) {
//	
//	/* Write Settings here
//	
//	Example:
//	
//    mSettings->setValue("LOADFROMVIDEO/PropertyName", mParameters->propertyName());
//  
//    */
//    
//  }
//}

//LoadFromVideoParameters *LoadFromVideoModelImp::parameters() const
//{
//  return mParameters;
//}

void LoadFromVideoModelImp::init()
{
  //mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void LoadFromVideoModelImp::clear()
{

}

} // namespace graphos
