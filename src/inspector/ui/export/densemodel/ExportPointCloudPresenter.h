#ifndef INSPECTOR_EXPORT_POINT_CLOUD_PRESENTER_INTERFACE_H
#define INSPECTOR_EXPORT_POINT_CLOUD_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{

namespace ui
{

class ExportPointCloudPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  ExportPointCloudPresenter(){}
  ~ExportPointCloudPresenter() override = default;

signals:


};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_EXPORT_POINT_CLOUD_PRESENTER_H
