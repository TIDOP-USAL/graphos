#ifndef GRAPHOS_LOADER_COMPONENT_H
#define GRAPHOS_LOADER_COMPONENT_H

#include "graphos/ui/Component.h"


namespace graphos
{

class Project;

namespace ui
{


class LoadImagesProcess;

class ImageLoaderComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  ImageLoaderComponent(Project *project);
  ~ImageLoaderComponent();

signals:

  void imageLoaded(QString);

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

  LoadImagesProcess *mProcess;
  Project *mProject;

};


} // namespace ui

} // namespace graphos


#endif // GRAPHOS_LOADER_COMPONENT_H
