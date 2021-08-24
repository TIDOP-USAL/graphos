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
	  
	/* Read Settings here
	
	Example (replace PropertyName):
	
    mParameters->setPropertyName(mSettings->value("ORTHOPHOTO/PropertyName", mParameters->propertyName()).toInt());
  
    */

  }
}

void OrthophotoModelImp::saveSettings()
{
  if (mReadSettings) {
	
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

void OrthophotoModelImp::init()
{
  mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void OrthophotoModelImp::clear()
{

}

} // namespace graphos
