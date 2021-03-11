#ifndef INSPECTOR_MATCHVIEWER_COMPONENT_H
#define INSPECTOR_MATCHVIEWER_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
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

} // namespace inspector


#endif // INSPECTOR_MATCHVIEWER_COMPONENT_H
