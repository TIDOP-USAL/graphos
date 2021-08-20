#ifndef GRAPHOS_IMPORT_CAMERAS_PRESENTER_INTERFACE_H
#define GRAPHOS_IMPORT_CAMERAS_PRESENTER_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
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

} // namespace graphos

#endif // GRAPHOS_IMPORT_CAMERAS_PRESENTER_INTERFACE_H
