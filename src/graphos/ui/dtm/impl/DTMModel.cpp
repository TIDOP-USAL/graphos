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

#include "DTMModel.h"

#include "graphos/core/project.h"

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

namespace graphos
{
	
DtmModelImp::DtmModelImp(Project *project,
                         QObject *parent)
  : DtmModel(parent),
    mProject(project)
{
  this->init();
}

QString DtmModelImp::projectPath() const
{
  return mProject->projectFolder();
}

QString DtmModelImp::denseModel() const
{
  return mProject->denseModel();
}

QString DtmModelImp::crs() const
{
  return mProject->crs();
}

std::shared_ptr<Dtm> DtmModelImp::dtmMethod() const
{
  return mProject->dtmMethod();
}

void DtmModelImp::setDtmMethod(const std::shared_ptr<Dtm> &dtm)
{
  mProject->setDtmMethod(dtm);
}

QString DtmModelImp::dtmPath() const
{
  return mProject->dtmPath();
}

void DtmModelImp::setDtmPath(const QString &dtmPath)
{
  mProject->setDtmPath(dtmPath);
}

std::array<double, 3> DtmModelImp::offset() const
{
  std::array<double, 3> offset;
  offset.fill(0.);

  try {
    QString path = mProject->reconstructionPath();
    path.append("/offset.txt");
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)){
      QTextStream stream(&file);
      QString line = stream.readLine();
      QStringList reg = line.split(" ");
      offset[0] = reg[0].toDouble();
      offset[1] = reg[1].toDouble();
      offset[2] = reg[2].toDouble();
      file.close();
    }
  } catch (...) {

  }

  return offset;
}

void DtmModelImp::init()
{
}

void DtmModelImp::clear()
{
}

} // namespace graphos

