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

#include "config_graphos.h"

#include <tidop/math/algebra/rotations/RotationMatrix.h>
#include <tidop/geometry/transform/Affine.h>

#include "graphos/core/project/Project.h"
#include "graphos/core/project/ProjectUtils.h"
#include "graphos/core/project/io/ProjectWriter.h"

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

    try {

        applyProjectTransformations(mProject);

        ProjectWriter writer;
        writer.write(mProject->info().projectPath(), *mProject);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Exception detected when saving the project");
    }
}

void SaveProjectModelImp::init()
{
}

void SaveProjectModelImp::clear()
{
    mProject->clear();
}

} // namespace graphos
