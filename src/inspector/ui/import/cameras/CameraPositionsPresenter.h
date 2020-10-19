#ifndef INSPECTOR_CAMERA_POSITIONS_PRESENTER_INTERFACE_H
#define INSPECTOR_CAMERA_POSITIONS_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class CamerasImportPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  CamerasImportPresenter(){}
  ~CamerasImportPresenter() override = default;

signals:

  void projectModified();

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERA_POSITIONS_PRESENTER_INTERFACE_H
