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

CreateProjectCommand::~CreateProjectCommand()
{
}

bool CreateProjectCommand::run()
{
    bool r = false;

    tl::Path file_path;
    QString base_name;
    tl::Path project_path;

    try {

        std::string projectName = this->value<std::string>("name");
        std::string projectDescription = this->value<std::string>("description");
        bool forceOverwrite = this->value<bool>("overwrite");

        QFileInfo file_info(projectName.c_str());

        if (file_info.isRelative()) {

            file_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdWString();
            file_path.append("graphos").append("Projects");

            QString extension = file_info.suffix();
            QString file_name;
            if (extension.compare(".xml", Qt::CaseInsensitive) == 0) {
                base_name = file_info.baseName();
                file_name = file_info.fileName();
            } else {
                file_name = base_name = file_info.baseName();
                file_name.append(".xml");
            }
            project_path = file_path.append(base_name.toStdWString());
            file_path.append(file_name.toStdWString());

        } else {

            base_name = file_info.baseName();
            file_path = projectName;
            project_path = file_info.path().toStdWString();

        }

        tl::Path database_path = file_path;
        database_path.replaceExtension(".db");

        if (project_path.exists()) {
            if (forceOverwrite) {
                tl::Path::removeDirectory(project_path);
            } else {
                throw std::runtime_error("The project already exists. Use '--overwrite' for delete previous project.");
            }
        }

        if (!project_path.createDirectories()) {
            throw std::runtime_error("Project directory cannot be created: " + project_path.toString());
        }

        ProjectImp project;

        project.setName(base_name);
        project.setProjectFolder(project_path);
        project.setDescription(projectDescription.c_str());
        project.setDatabase(database_path);
        project.save(file_path);

        tl::Message::info("Project created");
        tl::Message::info("- Name: {}", base_name.toStdString());
        tl::Message::info("- Description: {}", projectDescription);

    } catch (const std::exception &e) {
        tl::printException(e);
        r = true;
    }

    return r;
}

} // namespace graphos