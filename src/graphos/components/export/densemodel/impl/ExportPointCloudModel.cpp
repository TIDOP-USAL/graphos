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

#include "ExportPointCloudModel.h"

#include "graphos/core/project.h"

#include <QFile>
#include <QTextStream>

namespace graphos
{

ExportPointCloudModelImp::ExportPointCloudModelImp(Project *project,
                                                   QObject *parent)
  : ExportPointCloudModel(parent),
    mProject(project)
{
  this->init();
}

std::array<double, 3> ExportPointCloudModelImp::offset() const
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

QString ExportPointCloudModelImp::denseModel() const
{
  return mProject->denseModel();
}

void ExportPointCloudModelImp::init()
{
}

void ExportPointCloudModelImp::clear()
{

}

} // namespace graphos
