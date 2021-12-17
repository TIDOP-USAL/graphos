/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
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
  : Command("createproj", "Create Graphos project"),
    mProjectName(""),
    mProjectDescription(""),
    mForceOverwrite(false)
{
  this->push_back(CreateArgumentStringRequired("name", 'n', "Project name or project file (.xml)", &mProjectName));
  this->push_back(CreateArgumentStringOptional("description", 'd', "Project description", &mProjectDescription));
  this->push_back(CreateArgumentBooleanOptional("overwrite", 'o', "Force project overwrite (default = false)", &mForceOverwrite));

  this->addExample("createproj --name inspector ");

  this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

CreateProjectCommand::~CreateProjectCommand()
{
}

bool CreateProjectCommand::run()
{
  bool r = false;

  QString file_path;
  QString base_name;
  QString project_path;

  try {

    QFileInfo file_info(mProjectName.c_str());
    if (file_info.isRelative()) {
      file_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
      file_path.append("/graphos/Projects/");

      QString extension = file_info.suffix();
      QString file_name;
      if (extension.compare(".xml", Qt::CaseInsensitive) == 0) {
        base_name = file_info.baseName();
        file_name = file_info.fileName();
      } else {
        file_name = base_name = file_info.baseName();
        file_name.append(".xml");
      }
      project_path = file_path.append(base_name).append("/");
      file_path.append(file_name);
    } else {
      base_name = file_info.baseName();
      file_path = mProjectName.c_str();
      project_path = file_info.path();
    }

    QString database_path = project_path;
    database_path.append(base_name).append(".db");

    std::string s_path = project_path.toStdString();
    QDir dir(project_path);
    if (dir.exists()) {
      if (mForceOverwrite) {
        dir.removeRecursively();
      } else {
        throw std::runtime_error("The project already exists. Use '--overwrite' for delete previous project.");
      }
    }

    if (dir.mkpath(".") == false) {
      throw std::runtime_error("Project directory cannot be created: " + s_path);
    }

    /* Fichero log */

    //QString log_file = project_path + base_name + ".log";
    //tl::Log &log = tl::Log::instance();
    //log.setMessageLevel(tl::MessageLevel::msg_verbose);
    //log.setLogFile(log_file.toStdString());
    //tl::MessageManager::instance().addListener(&log);

    ProjectImp project;

    project.setName(base_name);
    project.setProjectFolder(project_path);
    project.setDescription(mProjectDescription.c_str());
    project.setDatabase(database_path);
    project.save(file_path);

    msgInfo("Project created");
    msgInfo("Project name: %s", base_name.toStdString().c_str());
    msgInfo("Project Description: %s", mProjectDescription.c_str());

  } catch (const std::exception& e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
    r = true;
  }

  return r;
}

} // namespace graphos