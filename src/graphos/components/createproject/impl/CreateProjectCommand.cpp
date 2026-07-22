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


#include "CreateProjectCommand.h"

#include "graphos/core/project/Project.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <tidop/core/app/Logger.h>

namespace graphos
{

CreateProjectCommand::CreateProjectCommand()
  : Command("createproj", "Create Graphos project")
{
    this->addArgument<tl::Path>("name", 'n', "Project name or project file (.xml)");
    this->addArgument<std::string>("description", 'd', "Project description", "");
    this->addArgument<bool>("overwrite", 'o', "Force project overwrite (default = false)", false);

    this->addExample("createproj --name inspector ");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

CreateProjectCommand::~CreateProjectCommand() = default;

bool CreateProjectCommand::run()
{
    bool r = false;

    tl::Logger &log = tl::Logger::instance();

    try {

        auto project_name = this->value<tl::Path>("name");
        auto project_description = this->value<std::string>("description");
        bool force_overwrite = this->value<bool>("overwrite");

        tl::Path project_path = project_name;
        

        if (!project_path.isAbsolutePath()) {

            tl::Path project_folder_path = tl::Path(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString());
            project_folder_path.append("graphos").append("Projects");

            auto base_name = project_path.baseName().toUtf8();
            auto extension = project_path.extension().toString();

            tl::Path file_name;
            if (tl::compareInsensitiveCase(extension, ".xml")){
                file_name = project_path.fileName();
            } else {
                file_name = tl::Path(std::string(base_name).append(".xml"));
            }
            
            project_folder_path.append(base_name);
            project_path = project_folder_path;
            project_path.append(file_name);

        }

        ProjectInfo projectInfo(project_path, project_description);

        auto project_folder = projectInfo.projectFolder();

        if (project_folder.exists()) {
            if (force_overwrite) {

                tl::Path::removeDirectory(project_folder);

            } else {
                throw std::runtime_error("The project already exists. Use '--overwrite' for delete previous project.");
            }
        }

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        if (!project_folder.exists() && !project_folder.createDirectories()) {
            throw std::runtime_error("Project directory cannot be created: " + project_folder.toUtf8());
        }

        Project project;
        project.info() = projectInfo;
        project.save(project_path);

        tl::Message::success("Project created at {}", project_path.toUtf8());
        tl::Message::info("- Name: {}", project_path.baseName().toUtf8());
        tl::Message::info("- Description: {}", project_description);

    } catch (const std::exception &e) {
        tl::printException(e);
        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos