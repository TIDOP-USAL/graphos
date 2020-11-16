#ifndef INSPECTOR_POINT_CLOUD_EXPORT_MODEL_INTERFACE_H
#define INSPECTOR_POINT_CLOUD_EXPORT_MODEL_INTERFACE_H

#include <array>

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class ExportPointCloudModel
  : public IModel
{

  Q_OBJECT

public:

  ExportPointCloudModel(QObject *parent = nullptr) : IModel(parent) {}
  ~ExportPointCloudModel() override = default;

  virtual std::array<double, 3> offset() const = 0;
  virtual QString denseModel() const = 0;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_POINT_CLOUD_EXPORT_MODEL_INTERFACE_H
