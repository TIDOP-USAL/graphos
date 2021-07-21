#ifndef INSPECTOR_EXPORT_ORIENTATIONS_MODEL_INTERFACE_H
#define INSPECTOR_EXPORT_ORIENTATIONS_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
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

};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_ORIENTATIONS_MODEL_INTERFACE_H
