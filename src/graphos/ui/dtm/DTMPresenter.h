#ifndef GRAPHOS_DTM_PRESENTER_INTERFACE_H
#define GRAPHOS_DTM_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"


namespace graphos
{

namespace ui
{

class DtmPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  DtmPresenter() {}
  ~DtmPresenter() override = default;

signals:


public slots:

  virtual void setCurrentDtmMethod(const QString &method) = 0;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_DTM_PRESENTER_INTERFACE_H
