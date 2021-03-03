#ifndef INSPECTOR_DENSIFICATION_COMPONENT_H
#define INSPECTOR_DENSIFICATION_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
{

class Project;

namespace ui
{


class DensificationProcess;

class DensificationComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  DensificationComponent(Project *project);
  ~DensificationComponent();

// ComponentBase

protected:

  void createModel() override;
  void createView() override;
  void createPresenter() override;

protected:

  void update() override;

protected:

  DensificationProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_DENSIFICATION_COMPONENT_H
