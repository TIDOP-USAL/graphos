#ifndef INSPECTOR_EXPORT_ORIENTATIONS_COMPONENT_H
#define INSPECTOR_EXPORT_ORIENTATIONS_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
{

class Project;

namespace ui
{


class ExportOrientationsComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  ExportOrientationsComponent(Project *project);
  ~ExportOrientationsComponent();

private slots:

  //void onImportedCameras();

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


#endif // INSPECTOR_EXPORT_ORIENTATIONS_COMPONENT_H
