#ifndef INSPECTOR_DENSE_PRESENTER_INTERFACE_H
#define INSPECTOR_DENSE_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
{

namespace ui
{

class HelpDialog;

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

} // End namespace inspector


#endif // INSPECTOR_DENSE_PRESENTER_INTERFACE_H
