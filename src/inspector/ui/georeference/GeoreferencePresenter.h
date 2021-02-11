#ifndef INSPECTOR_GEOREFERENCE_PRESENTER_INTERFACE_H
#define INSPECTOR_GEOREFERENCE_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
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

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_PRESENTER_INTERFACE_H
