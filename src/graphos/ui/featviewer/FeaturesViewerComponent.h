#ifndef GRAPHOS_FEATURES_VIEWER_COMPONENT_H
#define GRAPHOS_FEATURES_VIEWER_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
{

class Project;

namespace ui
{


class DtmProcess;

class FeaturesViewerComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  FeaturesViewerComponent(Project *project);
  ~FeaturesViewerComponent();

public slots:

  void openKeypointsViewer(const QString &file);

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


#endif // GRAPHOS_FEATURES_VIEWER_COMPONENT_H
