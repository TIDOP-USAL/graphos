#ifndef INSPECTOR_CREATE_PROJECT_COMPONENT_H
#define INSPECTOR_CREATE_PROJECT_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
{

class Project;

namespace ui
{

class CreateProjectComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  CreateProjectComponent(Project *project);
  ~CreateProjectComponent();

private slots:

  void onProjectCreated();

signals:

  void projectCreated();

// ComponentBase

protected:

  void createModel() override;
  void createView() override;
  void createPresenter() override;

protected:

  void update() override;

protected:

  Project *mProject;

};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_CREATE_PROJECT_COMPONENT_H