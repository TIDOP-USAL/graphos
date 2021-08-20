#ifndef GRAPHOS_DENSE_PRESENTER_INTERFACE_H
#define GRAPHOS_DENSE_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"

namespace graphos
{

namespace ui
{

class DensificationPresenter
  : public ProcessPresenter

{

  Q_OBJECT

public:

  DensificationPresenter(){}
  ~DensificationPresenter() override = default;

signals:

  void densificationFinished();

public slots:

  virtual void setCurrentDensifier(const QString &densifier) = 0;

};

} // End namespace ui

} // End namespace graphos


#endif // GRAPHOS_DENSE_PRESENTER_INTERFACE_H
