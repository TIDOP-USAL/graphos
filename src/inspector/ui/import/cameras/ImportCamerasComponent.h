#ifndef INSPECTOR_IMPORT_CAMERAS_COMPONENT_H
#define INSPECTOR_IMPORT_CAMERAS_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
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

} // namespace inspector


#endif // INSPECTOR_IMPORT_CAMERAS_COMPONENT_H
