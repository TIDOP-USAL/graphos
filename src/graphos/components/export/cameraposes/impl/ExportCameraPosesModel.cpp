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

#include "ExportCameraPosesModel.h"

#include "graphos/core/project/Project.h"
#include "graphos/core/project/io/ProjectWriter.h"
#include "graphos/core/camera/CameraRepository.h"
#include "graphos/core/image/ImageRepository.h"
#include "graphos/core/orientation/CameraPosesRepository.h"

namespace graphos
{

ExportCameraPosesModelImp::ExportCameraPosesModelImp(Project *project,
                                                     QObject *parent)
  : ExportCameraPosesModel(parent),
    mProject(project)
{
    ExportCameraPosesModelImp::init();
}

auto ExportCameraPosesModelImp::images() const -> const ImageRepository &
{
    return mProject->images();
}

auto ExportCameraPosesModelImp::poses() const -> const CameraPosesRepository &
{
    return mProject->cameraPoses();
}

auto ExportCameraPosesModelImp::cameras() const -> const CameraRepository &
{
    return mProject->cameras();
}

auto ExportCameraPosesModelImp::enuCrs() const -> QString
{
    return QString::fromStdString(mProject->info().enuCrs());
}

void ExportCameraPosesModelImp::graphosExport(const tl::Path &file) const
{
    ProjectWriter writer;
    writer.exportCameras(*mProject, file);
}

void ExportCameraPosesModelImp::init()
{
}

void ExportCameraPosesModelImp::clear()
{
}

} // namespace graphos
