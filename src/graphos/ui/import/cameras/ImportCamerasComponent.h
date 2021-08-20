#ifndef GRAPHOS_IMPORT_CAMERAS_COMPONENT_H
#define GRAPHOS_IMPORT_CAMERAS_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
{

class Project;

namespace ui
{


class ImportCamerasComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  ImportCamerasComponent(Project *project);
  ~ImportCamerasComponent();

private slots:

  void onImportedCameras();

signals:


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


#endif // GRAPHOS_IMPORT_CAMERAS_COMPONENT_H
