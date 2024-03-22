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

#include "graphos/core/project.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <tidop/core/log.h>

namespace graphos
{

CreateProjectCommand::CreateProjectCommand()
  : Command("createproj", "Create Graphos project")
{
    this->addArgument<std::string>("name", 'n', "Project name or project file (.xml)");
    this->addArgument<std::string>("description", 'd', "Project description", "");
    this->addArgument<bool>("overwrite", 'o', "Force project overwrite (default = false)", false);

    this->addExample("createproj --name inspector ");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

CreateProjectCommand::~CreateProjectCommand() = default;

bool CreateProjectCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path;
        tl::Path project_folder_path;
        QString base_name;

        auto project_name = this->value<std::string>("name");
        auto project_description = this->value<std::string>("description");
        bool force_overwrite = this->value<bool>("overwrite");

        QFileInfo file_info(project_name.c_str());

        if (file_info.isRelative()) {

            project_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdWString();
            project_path.append("graphos").append("Projects");

            QString extension = file_info.suffix();
            QString file_name;
            if (extension.compare(".xml", Qt::CaseInsensitive) == 0) {
                base_name = file_info.baseName();
                file_name = file_info.fileName();
            } else {
                file_name = base_name = file_info.baseName();
                file_name.append(".xml");
            }
            project_folder_path = project_path.append(base_name.toStdWString());
            project_path.append(file_name.toStdWString());

        } else {

            base_name = file_info.baseName();
            project_path = project_name;
            project_folder_path = file_info.path().toStdWString();

        }
        project_path.normalize();

        tl::Path database_path = project_path;
        database_path.replaceExtension(".db");

        if (project_folder_path.exists()) {
            if (force_overwrite) {
                tl::Path::removeDirectory(project_folder_path);
            } else {
                throw std::runtime_error("The project already exists. Use '--overwrite' for delete previous project.");
            }
        }

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path.toString());

        if (!project_folder_path.createDirectories()) {
            throw std::runtime_error("Project directory cannot be created: " + project_folder_path.toString());
        }

        ProjectImp project;

        project.setName(base_name);
        project.setProjectFolder(project_folder_path);
        project.setDescription(QString::fromStdString(project_description));
        project.setDatabase(database_path);
        project.save(project_path);

        tl::Message::success("Project created at {}", project_path.toString());
        tl::Message::info("- Name: {}", base_name.toStdString());
        tl::Message::info("- Description: {}", project_description);

    } catch (const std::exception &e) {
        tl::printException(e);
        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos