#ifndef GRAPHOS_GEOREFERENCE_COMPONENT_H
#define GRAPHOS_GEOREFERENCE_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
{

class Project;

namespace ui
{

//class DtmProcess;

class GeoreferenceComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  GeoreferenceComponent(Project *project);
  ~GeoreferenceComponent();

signals:

  //void finished();
  void georeferenceFinished();

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

  //DtmProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace graphos


#endif // GRAPHOS_GEOREFERENCE_COMPONENT_H
