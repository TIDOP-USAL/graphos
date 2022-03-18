/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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

#include "ExportOrientationsModel.h"

#include "graphos/core/project.h"

#include <fstream>

namespace graphos
{

ExportOrientationsModelImp::ExportOrientationsModelImp(Project *project,
                                                       QObject *parent)
  : ExportOrientationsModel(parent),
    mProject(project)
{
  this->init();
}


void ExportOrientationsModelImp::init()
{
}

void ExportOrientationsModelImp::clear()
{
}

QString ExportOrientationsModelImp::reconstruction() const
{
  return mProject->reconstructionPath();
}

tl::Point3D ExportOrientationsModelImp::offset() const
{
  tl::Point3D offset(0., 0., 0.);

  std::ifstream ifs;
  ifs.open(mProject->offset().toStdString(), std::ifstream::in);
  if (ifs.is_open()) {

    ifs >> offset.x >> offset.y >> offset.z;

    ifs.close();
  }

  return offset;
}

} // namespace graphos
