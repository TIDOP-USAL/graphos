#include <inspector/core/utils.h>
#include <inspector/interfaces/CreateProject.h>
#include <inspector/core/project.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

//#include <colmap/base/database.h>

#include <QDir>
#include <QTextStream>
#include <QStandardPaths>

#include <cstdlib>
#include <iostream>


using namespace inspector;
using namespace tl;

namespace inspector
{


class CreateProjectModelImpl
  : public CreateProjectModel
{

public:

  CreateProjectModelImpl(Project *project)
    : mProject(project)
{
  init();
}
  ~CreateProjectModelImpl() override = default;

// IModel interface

private:

  void init() override
  {
  }

// CreateProjectModel interface

public:

  void create() override
  {
  }

  void save(const QString &file) override
  {
    mProject->save(file);
  }

  void setProjectName(const QString &name) override
  {
    mProject->setName(name);
  }

  void setProjectPath(const QString &path) override
  {
    mProject->setProjectFolder(path);
  }

  void setProjectDescription(const QString &description) override
  {
    mProject->setDescription(description);
  }

  void setDatabase(const QString &database) override
  {
    mProject->setDatabase(database);
  }

protected:

  Project *mProject;
};

}





int main(int argc, char** argv)
{

/* Configuración de consola */

  std::string cmd_description = "Create Project";

  tl::Console &console = tl::Console::instance();
  console.setLogLevel(tl::MessageLevel::msg_verbose);
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

  QFileInfo file_info(project_name.c_str());
  if (file_info.isRelative()){
    file_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    file_path.append("/Inspector/Projects/");

    QString extension = file_info.suffix();
    QString file_name;
    if (extension.compare(".xml", Qt::CaseInsensitive) == 0){
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
  if (!dir.exists()){
    if (dir.mkpath(".") == false){
      msgError("Project directory cannot be created: %s", s_path.c_str());
      return 1;
    }
  }

/* Fichero log */

  QString log_file = project_path + base_name + ".log";

  tl::Log &log = tl::Log::instance();
  log.setLogLevel(tl::MessageLevel::msg_verbose);
  //QString log_file = dir.filePath("featextract.log");
  log.setLogFile(log_file.toStdString());

  /// Se subscribe el log al gestor de mensajes
  tl::MessageManager::instance().addListener(&log);

/* Chequeos previos para comprobar que todo este bien */

  if (QFileInfo(project_path).exists()){
    if (force_overwrite){
      QFile(project_path).remove();
      QFile(database_path).remove();
    } else {
      msgError("The project already exists");
      return 1;
    }
  }


  ProjectImp project;

  CreateProjectModelImpl project_model(&project);
  project_model.setProjectName(base_name);
  project_model.setProjectPath(project_path);
  project_model.setProjectDescription(project_description.c_str());
  project_model.setDatabase(database_path);
  project_model.save(file_path);

  msgInfo("Project created");

  //colmap::Database database(database_path.toStdString());

  return 0;
}
