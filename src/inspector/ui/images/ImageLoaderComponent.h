#ifndef INSPECTOR_IMAGE_LOADER_COMPONENT_H
#define INSPECTOR_IMAGE_LOADER_COMPONENT_H

#include "inspector/ui/Component.h"


namespace inspector
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

} // namespace inspector


#endif // INSPECTOR_IMAGE_LOADER_COMPONENT_H
