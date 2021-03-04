#ifndef INSPECTOR_FEATURE_MATCHING_COMPONENT_H
#define INSPECTOR_FEATURE_MATCHING_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
{

class Project;

namespace ui
{


class FeatureMatchingProcess;

class FeatureMatchingComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  FeatureMatchingComponent(Project *project);
  ~FeatureMatchingComponent();

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

  FeatureMatchingProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_FEATURE_MATCHING_COMPONENT_H
