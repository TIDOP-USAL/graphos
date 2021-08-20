#ifndef GRAPHOS_ORIENTATION_COMPONENT_H
#define GRAPHOS_ORIENTATION_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
{

class Project;

namespace ui
{


class OrientationProcess;

class OrientationComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  OrientationComponent(Project *project);
  ~OrientationComponent();

signals: 

  void orientationFinished();
  void orientationDeleted();

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

  OrientationProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace graphos


#endif // GRAPHOS_ORIENTATION_COMPONENT_H
