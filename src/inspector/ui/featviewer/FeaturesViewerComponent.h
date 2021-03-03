#ifndef INSPECTOR_FEATURES_VIEWER_COMPONENT_H
#define INSPECTOR_FEATURES_VIEWER_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
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


#endif // INSPECTOR_FEATURES_VIEWER_COMPONENT_H
