#ifndef INSPECTOR_EXPORT_ORIENTATIONS_MODEL_H
#define INSPECTOR_EXPORT_ORIENTATIONS_MODEL_H

#include "inspector/ui/export/orientations/ExportOrientationsModel.h"

namespace inspector
{

class Project;

namespace ui
{

class ExportOrientationsModelImp
  : public ExportOrientationsModel
{

public:

  ExportOrientationsModelImp(Project *project,
                             QObject *parent = nullptr);
  ~ExportOrientationsModelImp() override = default;

// ExportOrientationsModel interface

public:

  QString reconstruction() const override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_ORIENTATIONS_MODEL_H
