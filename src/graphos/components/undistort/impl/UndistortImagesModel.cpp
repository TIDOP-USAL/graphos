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

#include "UndistortImagesModel.h"

#include "graphos/core/project.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>

namespace graphos
{

UndistortImagesModelImp::UndistortImagesModelImp(Project *project,
                                                 QObject *parent)
  : UndistortImagesModel(parent),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope, 
                            qApp->organizationName(), 
                            qApp->applicationName())),
    mProject(project)
{
  this->init();
}

UndistortImagesModelImp::~UndistortImagesModelImp()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }
}

const std::unordered_map<size_t, Image> &UndistortImagesModelImp::images() const
{
  return mProject->images();
}

const std::map<int, Camera> &UndistortImagesModelImp::cameras() const
{
  return mProject->cameras();
}

bool UndistortImagesModelImp::useCuda() const
{
  return mSettings->value("UseCuda", true).toBool();
}

QString UndistortImagesModelImp::projectFolder() const
{
  return mProject->projectFolder();
}

void UndistortImagesModelImp::loadSettings()
{
  if (mReadSettings) {
	  
	/* Read Settings here
	
	Example (replace PropertyName):
	
    mParameters->setPropertyName(mSettings->value("UNDISTORTIMAGES/PropertyName", mParameters->propertyName()).toInt());
  
    */

  }
}

void UndistortImagesModelImp::saveSettings()
{
  if (mReadSettings) {
	
	/* Write Settings here
	
	Example:
	
    mSettings->setValue("UNDISTORTIMAGES/PropertyName", mParameters->propertyName());
  
    */
    
  }
}

//UndistortImagesParameters *UndistortImagesModelImp::parameters() const
//{
//  return mParameters;
//}

void UndistortImagesModelImp::init()
{
  mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void UndistortImagesModelImp::clear()
{

}

} // namespace graphos
