#ifndef GRAPHOS_DTM_COMPONENT_H
#define GRAPHOS_DTM_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
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

signals:

  //void finished();

// ComponentBase

protected:

  void createModel() override;
  void createView() override;
  void createPresenter() override;

protected:

  void update() override;

  // ProcessComponent

protected slots:

  void onRunning() override;
  void onFinished() override;
  void onFailed() override;

protected:

  DtmProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace graphos


#endif // GRAPHOS_DTM_COMPONENT_H
