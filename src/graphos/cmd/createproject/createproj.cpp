#include <graphos/core/utils.h>
#include <graphos/core/project.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

#include <QDir>
#include <QTextStream>
#include <QStandardPaths>

#include <cstdlib>
#include <iostream>


using namespace graphos;
using namespace tl;


int main(int argc, char** argv)
{

/* Configuración de consola */

  std::string cmd_description = "Create Project";

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

/* Parseo de argumentos */

  std::string project_name;
  std::string project_description;
  bool force_overwrite = false;

  Command cmd("createproj", cmd_description);
  cmd.push_back(std::make_shared<ArgumentStringRequired>("name", 'n', "Project name or project file (.xml)", &project_name));
  cmd.push_back(std::make_shared<ArgumentStringOptional>("description", 'd', "Project description", &project_description));
  cmd.push_back(std::make_shared<ArgumentBooleanOptional>("overwrite", 'o', "Force project overwrite (default = false)", &force_overwrite));

  cmd.addExample("createproj --name inspector ");

  // Parseo de los argumentos y comprobación de los mismos
  Command::Status status = cmd.parse(argc, argv);
  if (status == Command::Status::parse_error ) {
    return 1;
  } else if (status == Command::Status::show_help) {
    return 0;
  } else if (status == Command::Status::show_licence) {
    return 0;
  } else if (status == Command::Status::show_version) {
    return 0;
  }

  QString file_path;
  QString base_name;
  QString project_path;

  try 	{

    QFileInfo file_info(project_name.c_str());
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
      file_path = project_name.c_str();
      project_path = file_info.path();
    }

    QString database_path = project_path;
    database_path.append(base_name).append(".db");

    std::string s_path = project_path.toStdString();
    QDir dir(project_path);
    if (dir.exists()) {
      if (force_overwrite) {
        dir.removeRecursively();
      } else {
        throw std::runtime_error("The project already exists. Use '--overwrite' for delete previous project.");
      }
    } 

    if (dir.mkpath(".") == false) {
      throw std::runtime_error("Project directory cannot be created: " + s_path);
    }

    /* Fichero log */

    QString log_file = project_path + base_name + ".log";
    tl::Log &log = tl::Log::instance();
    log.setMessageLevel(tl::MessageLevel::msg_verbose);
    log.setLogFile(log_file.toStdString());
    tl::MessageManager::instance().addListener(&log);

    ProjectImp project;

    project.setName(base_name);
    project.setProjectFolder(project_path);
    project.setDescription(project_description.c_str());
    project.setDatabase(database_path);
    project.save(file_path);

    msgInfo("Project created");
    msgInfo("Project name: %s", base_name.toStdString().c_str());
    msgInfo("Project Description: %s", project_description.c_str());

  } catch (const std::exception &e) 	{
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
  
  return 0;
}
