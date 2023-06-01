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

#include "ScaleModel.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>

namespace graphos
{

ScaleModelImp::ScaleModelImp(Project *project, QObject *parent)
  : ScaleModel(parent),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope, 
                            qApp->organizationName(), 
                            qApp->applicationName())),
    mProject(project)
{
  this->init();
}

ScaleModelImp::~ScaleModelImp()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }
}

void ScaleModelImp::loadSettings()
{
  if (mReadSettings) {
	  
	/* Read Settings here
	
	Example (replace PropertyName):
	
    mParameters->setPropertyName(mSettings->value("SCALE/PropertyName", mParameters->propertyName()).toInt());
  
    */

  }
}

void ScaleModelImp::setTransform(const tl::math::Matrix<double, 4, 4> &transform)
{
  mProject->setTransform(transform);
}

void ScaleModelImp::saveSettings()
{
  if (mReadSettings) {
	
	/* Write Settings here
	
	Example:
	
    mSettings->setValue("SCALE/PropertyName", mParameters->propertyName());
  
    */
    
  }
}

void ScaleModelImp::init()
{
  mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void ScaleModelImp::clear()
{

}

//QVector3D ScaleModelImp::offset() const
//{
//  QVector3D offset(0,0,0);
//
//  try {
//
//    tl::Path path = mProject->offset();
//    QFile file(QString::fromStdWString(path.toWString()));
//    if (file.open(QFile::ReadOnly | QFile::Text)){
//      QTextStream stream(&file);
//      QString line = stream.readLine();
//      QStringList reg = line.split(" ");
//      offset[0] = reg[0].toDouble();
//      offset[1] = reg[1].toDouble();
//      offset[2] = reg[2].toDouble();
//      file.close();
//    }
//  } catch (...) {
//    TL_THROW_EXCEPTION_WITH_NESTED("");
//  }
//
//  return offset;
//}

} // namespace graphos
