#ifndef GRAPHOS_POINT_CLOUD_EXPORT_MODEL_H
#define GRAPHOS_POINT_CLOUD_EXPORT_MODEL_H

#include "graphos/ui/export/densemodel/ExportPointCloudModel.h"


namespace graphos
{

class Project;

namespace ui
{

class ExportPointCloudModelImp
  : public ExportPointCloudModel
{

  Q_OBJECT

public:

  ExportPointCloudModelImp(Project *project,
                           QObject *parent = nullptr);
  ~ExportPointCloudModelImp() override = default;

// ExportPointCloudModel interface

public:

  std::array<double, 3> offset() const override;
  QString denseModel() const override;

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

#endif // GRAPHOS_POINT_CLOUD_EXPORT_MODEL_H
