#ifndef INSPECTOR_CAMERAS_COMPONENT_H
#define INSPECTOR_CAMERAS_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
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

} // namespace inspector


#endif // INSPECTOR_CAMERAS_COMPONENT_H
