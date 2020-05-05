#include <inspector/core/utils.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

#include <QDir>
#include <QTextStream>
#include <QStandardPaths>

#include <boost/algorithm/string.hpp>

#include <cstdlib>
#include <iostream>


using namespace inspector;
using namespace tl;

#if (__cplusplus >= 201703L)
namespace fs = std::filesystem;
#else
namespace fs = boost::filesystem;
#endif

int main(int argc, char** argv)
{

/* Configuración de consola */

  std::string cmd_description = "New Project";

  tl::Console &console = tl::Console::instance();
  console.setLogLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

/* Parseo de argumentos */

  fs::path project;
  size_t idx_camera = 2; /// Camera RADIAL
  std::vector<std::string> cameras {
    "SIMPLE_PINHOLE",
    "SIMPLE_RADIAL",
    "RADIAL",
    "FULL_RADIAL"};
  bool force_overwrite = false;

  Command cmd("createproj", cmd_description);
  cmd.push_back(std::make_shared<ArgumentPathRequired>("name", 'n', "Project name or project file (.xml)", &project));
  cmd.push_back(std::make_shared<ArgumentListStringOptional>("camera_model", 'c', "Set camera model. Available cameras: SIMPLE_PINHOLE, SIMPLE_RADIAL, RADIAL (default), FULL_RADIAL", cameras, &idx_camera));
  cmd.push_back(std::make_shared<ArgumentBooleanOptional>("overwrite", 'o', "Force project overwrite (default = false)", force_overwrite));

  cmd.addExample("createproj --name inspector -c RADIAL");

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

  fs::path project_path;

  if (project.is_relative()){
    project_path.append(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString());
    project_path.append("Inspector/Projects");

    std::string extension = fs::path(project).extension().string();
    std::string file_name;
    std::string file_name_ext;
    if (boost::iequals(extension, ".xml")){
      file_name = fs::path(project).stem().string();
      file_name_ext = fs::path(project).filename().string();
    } else {
      file_name = project.string();
      file_name_ext = std::string(file_name).append(".xml");
    }
    project_path.append(file_name);
    project_path.append(file_name_ext);
  } else {
    project_path = project;
  }

  fs::path database_path(project_path);
  database_path.replace_extension(".db");

  std::string s_path = project_path.string();
  QDir dir(s_path.c_str());
  if (!dir.exists()){
    if (dir.mkpath(".") == false){
      msgError("Project directory cannot be created: %s", s_path.c_str());
      return 1;
    }
  }

/* Fichero log */

  tl::Log &log = tl::Log::instance();
  log.setLogLevel(tl::MessageLevel::msg_verbose);
  QString log_file = dir.filePath("featextract.log");
  log.setLogFile(log_file.toLatin1());

  /// Se subscribe el log al gestor de mensajes
  tl::MessageManager::instance().addListener(&log);

/* Chequeos previos para comprobar que todo este bien */

  if (fs::exists(project_path)){
    if (force_overwrite){
      fs::remove(project_path);
      fs::remove(database_path);
    } else {
      msgError("Project %s already exists", s_path.c_str());
      return 1;
    }
  }

//  if (boost::iequals(database_path.extension().string(), ".db")) {
//    msgInfo("Set database: %s", database_path.string().c_str());
//  } else {
//    msgError("Invalid database: %s", database_path.string().c_str());
//    return 1;
//  }



  return 0;
}
