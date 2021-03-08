#ifndef INSPECTOR_OPEN_PROJECT_COMPONENT_H
#define INSPECTOR_OPEN_PROJECT_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
{

class Project;

namespace ui
{

class OpenProjectComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  OpenProjectComponent(Project *project);
  ~OpenProjectComponent();

signals:

  void projectLoaded();

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


#endif // INSPECTOR_OPEN_PROJECT_COMPONENT_H
