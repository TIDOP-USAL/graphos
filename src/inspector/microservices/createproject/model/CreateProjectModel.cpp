#include "CreateProjectModel.h"

#include <inspector/interfaces/CreateProject.h>
#include <inspector/core/project.h>

namespace inspector
{

namespace microservice
{

CreateProjectModelImpl::CreateProjectModelImpl(Project *project)
  : mProject(project)
{
  init();
}

void CreateProjectModelImpl::init()
{
}

void CreateProjectModelImpl::create()
{
}

void CreateProjectModelImpl::save(const QString &file)
{
  mProject->save(file);
}

void CreateProjectModelImpl::setProjectName(const QString &name)
{
  mProject->setName(name);
}

void CreateProjectModelImpl::setProjectPath(const QString &path)
{
  mProject->setProjectFolder(path);
}

void CreateProjectModelImpl::setProjectDescription(const QString &description)
{
  mProject->setDescription(description);
}

void CreateProjectModelImpl::setDatabase(const QString &database)
{
  mProject->setDatabase(database);
}

} // namespace microservice

} // namespace inspector