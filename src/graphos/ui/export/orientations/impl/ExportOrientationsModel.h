#ifndef GRAPHOS_EXPORT_ORIENTATIONS_MODEL_H
#define GRAPHOS_EXPORT_ORIENTATIONS_MODEL_H

#include "graphos/ui/export/orientations/ExportOrientationsModel.h"

namespace graphos
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
  tl::Point3D offset() const override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_EXPORT_ORIENTATIONS_MODEL_H
