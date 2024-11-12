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
#include "graphos/core/sfm/posesio.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/log.h>

#include <QFileInfo>
#include <tidop/geospatial/util.h>



using namespace tl;

namespace graphos
{


ExportPointCloudCommand::ExportPointCloudCommand()
  : Command("export_point_cloud", "Export point cloud"),
    mProject(nullptr)
{
    this->addArgument<Path>("prj", 'p', "Project file");
    this->addArgument<Path>("file", 'f', "Export file");
    auto ply_format = Argument::make<std::string>("ply:format", "PLY format", "binary");
    ply_format->setValidator(ValuesValidator<std::string>::create({"binary", "text"}));
    this->addArgument(ply_format);
    this->addOption("ply:colors", "Export point cloud colors", true);
    this->addOption("ply:normals", "Export point cloud normals", true);
    this->addArgument<std::string>("crs", "CRS of the point cloud", "");

    this->addExample("export_point_cloud -p 253/253.xml --file point_cloud.ply");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

ExportPointCloudCommand::~ExportPointCloudCommand()
{
    if (mProject) {
        delete mProject;
        mProject = nullptr;
    }
}

auto ExportPointCloudCommand::crs() const -> std::string
{
    std::string epsg_code;

    try {

        auto enu_crs = mProject->enuCrs().toStdString();
        auto v = tl::split<std::string>(enu_crs, ';');
        auto zone = tl::utmZoneFromLonLat(tl::stringToNumber<double>(v.at(1)), tl::stringToNumber<double>(v.at(2)));
        epsg_code = "EPSG:326";
        epsg_code.append(std::to_string(zone.first));

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return epsg_code;
}

bool ExportPointCloudCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        auto project_path = this->value<Path>("prj");
        auto file =  this->value<Path>("file");
        auto crs = this->value<std::string>("crs");
        auto ply_format = this->value<std::string>("ply:format");
        auto colors = this->value<bool>("ply:colors");
        auto normals = this->value<bool>("ply:normals");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        mProject = new ProjectImp;
        mProject->load(project_path);

        ExportPointCloudTask export_point_cloud_task(mProject->denseModel(),
                                                     file,
                                                     mProject->enuCrs().toStdString(),
                                                     crs.empty() ? this->crs() : crs,
                                                     ply_format == "binary",
                                                     colors, 
                                                     normals);

        size_t size = static_cast<size_t>(mProject->denseReport().points / 80.) * 20 + mProject->denseReport().points;

        ProgressBarColor progress(0, size);
        export_point_cloud_task.run(&progress);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos