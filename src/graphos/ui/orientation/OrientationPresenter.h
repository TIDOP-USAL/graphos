#ifndef GRAPHOS_ORIENTATION_PRESENTER_INTERFACE_H
#define GRAPHOS_ORIENTATION_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"

namespace graphos
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

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_PRESENTER_INTERFACE_H
