#include "service.h"

#include "inspector/microservices/createproject/model/CreateProjectModel.h"
#include "inspector/core/project.h"


#include <QDir>
#include <QStandardPaths>

namespace inspector
{

namespace microservice
{

Service::Service()
{
}

Service::~Service()
{
}

void Service::createProject(const QString &name, 
                            const QString &description,
                            bool overwrite)
{
  QString file_path;
  QString base_name;
  QString project_path;

  QFileInfo file_info(name);
  if (file_info.isRelative()) {
    file_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    file_path.append("/Inspector/Projects/");

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
    file_path = name;
    project_path = file_info.path();
  }

  QString database_path = project_path;
  database_path.append(base_name).append(".db");

  QDir dir(project_path);
  if (!dir.exists()) {
    if (dir.mkpath(".") == false) {
      std::string error = "Project directory cannot be created: " + project_path.toStdString();
      throw std::runtime_error(error);
    }
  }

  /* Chequeos previos para comprobar que todo este bien */

  if (QFileInfo(project_path).exists()) {
    if (overwrite) {
      QFile(project_path).remove();
      QFile(database_path).remove();
    } else {
      throw std::runtime_error("The project already exists. Use 'overwrite' parameter");
    }
  }

  ProjectImp project;

  CreateProjectModelImpl project_model(&project);
  project_model.setProjectName(base_name);
  project_model.setProjectPath(project_path);
  project_model.setProjectDescription(description);
  project_model.setDatabase(database_path);
  project_model.save(file_path);

}

} // namespace microservice

} // namespace inspector