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


#include "ExportPointCloudCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project.h"
#include "graphos/components/export/pointcloud/impl/ExportPointCloudTask.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/log.h>

#include <QFileInfo>

#include "graphos/core/sfm/posesio.h"


using namespace tl;

namespace graphos
{


ExportPointCloudCommand::ExportPointCloudCommand()
  : Command("export_point_cloud", "Export point cloud")
{
    this->addArgument<Path>("prj", 'p', "Project file");
    this->addArgument<Path>("file", 'f', "Export file");
    this->addArgument<std::string>("crs", "CRS of the point cloud (default: CRS of the project )", "");

    this->addExample("export_point_cloud -p 253/253.xml --file point_cloud.ply");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

ExportPointCloudCommand::~ExportPointCloudCommand()
{
}

bool ExportPointCloudCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        auto project_path = this->value<Path>("prj");
        auto file =  this->value<Path>("file");
        auto crs = this->value<std::string>("crs");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(project_path);

        ExportPointCloudTask export_point_cloud_task(project.denseModel(),
                                                     offsetRead(project.offset()),
                                                     file,
                                                     crs.empty() ? project.crs().toStdString() : crs);

        export_point_cloud_task.run();

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos