#ifndef INSPECTOR_DTM_COMPONENT_H
#define INSPECTOR_DTM_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
{

class Project;

namespace ui
{


class DtmProcess;

class DTMComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  DTMComponent(Project *project);
  ~DTMComponent();

// ComponentBase

protected:

  void createModel() override;
  void createView() override;
  void createPresenter() override;

protected:

  DtmProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_DTM_COMPONENT_H
