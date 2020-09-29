#ifndef INSPECTOR_GEOREFERENCE_PRESENTER_INTERFACE_H
#define INSPECTOR_GEOREFERENCE_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class GeoreferencePresenter
  : public IPresenter
{

  Q_OBJECT

public:

  GeoreferencePresenter(){}
  ~GeoreferencePresenter() override = default;

public slots:


};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_PRESENTER_INTERFACE_H
