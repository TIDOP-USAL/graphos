#ifndef INSPECTOR_CREATE_PROJECT_MODEL_H
#define INSPECTOR_CREATE_PROJECT_MODEL_H

#include "inspector/interfaces/CreateProject.h"

namespace inspector
{

class Project;

namespace microservice
{

class CreateProjectModelImpl
  : public CreateProjectModel
{

public:

  CreateProjectModelImpl(Project *project);
  ~CreateProjectModelImpl() override = default;

// IModel interface

private:

  void init() override;

// CreateProjectModel interface

public:

  void create() override;
  void save(const QString &file) override;
  void setProjectName(const QString &name) override;
  void setProjectPath(const QString &path) override;
  void setProjectDescription(const QString &description) override;
  void setDatabase(const QString &database) override;

protected:

  Project *mProject;
};

} // namespace microservice

} // namespace inspector

#endif // INSPECTOR_CREATE_PROJECT_MODEL_H