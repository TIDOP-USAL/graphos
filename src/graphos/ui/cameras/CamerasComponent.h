#ifndef GRAPHOS_CAMERAS_COMPONENT_H
#define GRAPHOS_CAMERAS_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
{

class Project;

namespace ui
{


class DtmProcess;

class CamerasComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  CamerasComponent(Project *project);
  ~CamerasComponent();

private slots:

  void onUpdateCameras();

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

} // namespace graphos


#endif // GRAPHOS_CAMERAS_COMPONENT_H
