#ifndef INSPECTOR_POINT_CLOUD_EXPORT_MODEL_H
#define INSPECTOR_POINT_CLOUD_EXPORT_MODEL_H

#include "inspector/ui/export/densemodel/ExportPointCloudModel.h"


namespace inspector
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

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_POINT_CLOUD_EXPORT_MODEL_H
