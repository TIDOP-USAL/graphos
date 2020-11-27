#ifndef INSPECTOR_DTM_PRESENTER_INTERFACE_H
#define INSPECTOR_DTM_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"


namespace inspector
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

} // namespace inspector

#endif // INSPECTOR_DTM_PRESENTER_INTERFACE_H
