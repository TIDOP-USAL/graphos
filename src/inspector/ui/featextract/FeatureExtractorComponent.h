#ifndef INSPECTOR_FEATURE_EXTRACTOR_COMPONENT_H
#define INSPECTOR_FEATURE_EXTRACTOR_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
{

class Project;

namespace ui
{


class FeatureExtractorProcess;

class FeatureExtractorComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  FeatureExtractorComponent(Project *project);
  ~FeatureExtractorComponent();

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

  FeatureExtractorProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_FEATURE_EXTRACTOR_COMPONENT_H
