#ifndef GRAPHOS_POINT_CLOUD_EXPORT_MODEL_INTERFACE_H
#define GRAPHOS_POINT_CLOUD_EXPORT_MODEL_INTERFACE_H

#include <array>

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{

class ExportPointCloudModel
  : public Model
{

  Q_OBJECT

public:

  ExportPointCloudModel(QObject *parent = nullptr) : Model(parent) {}
  ~ExportPointCloudModel() override = default;

  virtual std::array<double, 3> offset() const = 0;
  virtual QString denseModel() const = 0;

};

} // namespace ui

} // namespace graphos


#endif // GRAPHOS_POINT_CLOUD_EXPORT_MODEL_INTERFACE_H
