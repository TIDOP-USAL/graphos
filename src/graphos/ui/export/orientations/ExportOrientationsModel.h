#ifndef GRAPHOS_EXPORT_ORIENTATIONS_MODEL_INTERFACE_H
#define GRAPHOS_EXPORT_ORIENTATIONS_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

#include <tidop/geometry/entities/point.h>

namespace graphos
{

namespace ui
{

class ExportOrientationsModel
  : public Model
{

  Q_OBJECT

public:

  ExportOrientationsModel(QObject *parent = nullptr) : Model(parent) {}
  ~ExportOrientationsModel() override = default;

  virtual QString reconstruction() const = 0;
  virtual tl::Point3D offset() const = 0;

};


} // namespace ui

} // namespace graphos

#endif // GRAPHOS_EXPORT_ORIENTATIONS_MODEL_INTERFACE_H
