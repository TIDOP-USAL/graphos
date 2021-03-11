#ifndef INSPECTOR_ORIENTATION_COMPONENT_H
#define INSPECTOR_ORIENTATION_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
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

} // namespace inspector


#endif // INSPECTOR_ORIENTATION_COMPONENT_H
