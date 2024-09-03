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

#include "CoordinateReferenceSystemModel.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>

namespace graphos
{

CoordinateReferenceSystemModelImp::CoordinateReferenceSystemModelImp(Project *project, QObject *parent)
  : CoordinateReferenceSystemModel(parent),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope, 
                            qApp->organizationName(), 
                            qApp->applicationName())),
    mProject(project),
    mSearch("")
{
    this->init();
}

CoordinateReferenceSystemModelImp::~CoordinateReferenceSystemModelImp()
{
    if (mSettings){
        delete mSettings;
        mSettings = nullptr;
    }
}

void CoordinateReferenceSystemModelImp::loadSettings()
{
    if (mReadSettings) {
	    
        /* Read Settings here
	
        Example (replace PropertyName):
	
        mParameters->setPropertyName(mSettings->value("COORDINATEREFERENCESYSTEM/PropertyName", mParameters->propertyName()).toInt());
  
        */

    }
}

void CoordinateReferenceSystemModelImp::saveSettings()
{
    if (mReadSettings) {
	
       /* Write Settings here
	
        Example:
	
        mSettings->setValue("COORDINATEREFERENCESYSTEM/PropertyName", mParameters->propertyName());
  
        */
    
    }
}

QString CoordinateReferenceSystemModelImp::search() const 
{
  return mSearch;
}


void CoordinateReferenceSystemModelImp::setSearch(const QString &search) 
{
  mSearch = search;
}



void CoordinateReferenceSystemModelImp::init()
{
    mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void CoordinateReferenceSystemModelImp::clear()
{

}

} // namespace graphos
