#ifndef INSPECTOR_ORIENTATION_PRESENTER_INTERFACE_H
#define INSPECTOR_ORIENTATION_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
{

namespace ui
{

class OrientationPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  OrientationPresenter() {}
  ~OrientationPresenter() override = default;

signals:

  void orientationFinished();
  void orientationDeleted();
};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_PRESENTER_INTERFACE_H
