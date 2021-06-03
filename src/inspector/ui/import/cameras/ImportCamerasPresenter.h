#ifndef INSPECTOR_IMPORT_CAMERAS_PRESENTER_INTERFACE_H
#define INSPECTOR_IMPORT_CAMERAS_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class ImportCamerasPresenter
  : public Presenter
{

  Q_OBJECT

public:

  ImportCamerasPresenter(){}
  ~ImportCamerasPresenter() override = default;

signals:

  void importedCameras();

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMPORT_CAMERAS_PRESENTER_INTERFACE_H
