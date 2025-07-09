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

#include "ExportCameraPosesCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project.h"
#include "graphos/components/export/cameraposes/impl/ExportCameraPosesTask.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/log.h>


using namespace tl;

namespace graphos
{


ExportCameraPosesCommand::ExportCameraPosesCommand()
  : Command("export_camera_poses", "Export camera poses")
{
    this->addArgument<Path>("prj", 'p', "Path to the project file");
    this->addArgument<Path>("file", "Export file [TXT (*.txt), ODM (*.json)]");
    std::vector<std::string> formats{"TXT", "ODM"};
    auto format_arg = tl::Argument::make<std::string>("format", 'f', "Calibration file format", formats.at(0));
    format_arg->setValidator(std::make_shared<tl::ValuesValidator<std::string>>(formats));
    this->addArgument(format_arg);
    this->addOption("quaternions", 'q', "Rotations as quaternions (text format only)", false);

    this->addExample("export_camera_poses -p 253/253.xml --file poses.txt --format TXT -q");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

ExportCameraPosesCommand::~ExportCameraPosesCommand()
{
}

bool ExportCameraPosesCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        auto project_path = this->value<Path>("prj");
        auto file =  this->value<Path>("file");
        auto format = this->value<std::string>("format");
        auto quaternions = this->value<bool>("quaternions");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        TL_ASSERT(file.extension() == tl::Path(".txt") && format == "TXT" ||
                  file.extension() == tl::Path(".json") && format == "ODM", 
                  "Invalid export file format. Supported formats are TXT (*.txt) and ODM (*.json).");

        ProjectImp project;
        project.load(project_path);

        auto export_task = std::make_unique<ExportCameraPosesTask>(file,
                                                                   project.images(),
                                                                   project.poses(),
                                                                   project.cameras(),
                                                                   project.enuCrs(),
                                                                   QString::fromStdString(format));

        if (format == "TXT" && quaternions) {
            dynamic_cast<ExportCameraPosesTask *>(export_task.get())->setQuaternionRotation(true);
        }

        export_task->run();

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos