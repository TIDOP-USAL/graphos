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

#include "SaveProjectModel.h"

#include "graphos/core/project.h"

#include <QStandardPaths>

namespace graphos
{

SaveProjectModelImp::SaveProjectModelImp(Project *project,
                                         QObject *parent)
  : SaveProjectModel(parent),
    mProject(project)
{
  SaveProjectModelImp::init();
}

SaveProjectModelImp::~SaveProjectModelImp()
{
}

void SaveProjectModelImp::save()
{
  // Se comprueba si se ha escalado o transformado los modelos con el escalado, transformación o georeferencia.
  // Se tiene que escribir algun flag para comprobarlo o directamente una matriz de transformación que se tendrá
  // que aplicar a los modelos sparse, dense, mesh y a los ficheros poses.bin y ground_points.bin
  try {

    auto transform = mProject->transform();
    if (transform != tl::math::Matrix<double, 4, 4>::identity()) {

      auto sparse = mProject->sparseModel();
      if (sparse.exists()) transformModel(transform, sparse.toString());
      auto dense = mProject->denseModel();
      if (dense.exists()) transformModel(transform, dense.toString());
      //auto mesh = mProject->meshPath();
      //if (mesh.exists()) transformModel(transform, mesh.toString());

    }

  } catch (std::exception &e) {
    tl::printException(e);
  }

  mProject->save(mProject->projectPath());
}

void SaveProjectModelImp::init()
{
}

void SaveProjectModelImp::clear()
{
  mProject->clear();
}

} // namespace graphos
