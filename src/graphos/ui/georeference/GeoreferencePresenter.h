#ifndef GRAPHOS_GEOREFERENCE_PRESENTER_INTERFACE_H
#define GRAPHOS_GEOREFERENCE_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"

namespace graphos
{

namespace ui
{

class GeoreferencePresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  GeoreferencePresenter(){}
  ~GeoreferencePresenter() override = default;

public slots:

  virtual void setImageActive(const QString &image) = 0;

signals:

  void georeferenceFinished();

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_PRESENTER_INTERFACE_H
