#ifndef GRAPHOS_MATCHVIEWER_COMPONENT_H
#define GRAPHOS_MATCHVIEWER_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
{

class Project;

namespace ui
{


class DtmProcess;

class MatchViewerComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  MatchViewerComponent(Project *project);
  ~MatchViewerComponent();

public slots:

  void openMatchesViewer(const QString &file);

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


#endif // GRAPHOS_MATCHVIEWER_COMPONENT_H
