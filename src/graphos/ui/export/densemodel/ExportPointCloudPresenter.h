#ifndef GRAPHOS_EXPORT_POINT_CLOUD_PRESENTER_INTERFACE_H
#define GRAPHOS_EXPORT_POINT_CLOUD_PRESENTER_INTERFACE_H

#include "graphos/interfaces/mvp.h"


namespace graphos
{

namespace ui
{

class ExportPointCloudPresenter
  : public Presenter
{

  Q_OBJECT

public:

  ExportPointCloudPresenter(){}
  ~ExportPointCloudPresenter() override = default;

public slots:

  virtual void save() = 0;
  virtual void setCurrentFormat(const QString &format) = 0;

};

} // namespace ui

} // namespace graphos


#endif // GRAPHOS_EXPORT_POINT_CLOUD_PRESENTER_H
