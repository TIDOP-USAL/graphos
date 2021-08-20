#ifndef GRAPHOS_EXPORT_ORIENTATIONS_COMPONENT_H
#define GRAPHOS_EXPORT_ORIENTATIONS_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
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

} // namespace graphos


#endif // GRAPHOS_EXPORT_ORIENTATIONS_COMPONENT_H
