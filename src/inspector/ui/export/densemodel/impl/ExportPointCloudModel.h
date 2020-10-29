#ifndef INSPECTOR_POINT_CLOUD_EXPORT_MODEL_H
#define INSPECTOR_POINT_CLOUD_EXPORT_MODEL_H

#include "inspector/ui/export/densemodel/CameraPositionsModel.h"


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

public slots:

  void setCsvFile(const QString &csv) override;
  void setDelimiter(const QString &delimiter) override;
  void enableColor(bool active) override;
  void enableNormals(bool active) override;
  void exportPointCloud() override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QString mCsvFile;
  QString mDelimiter;
  bool mColor;
  bool mNormals;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_POINT_CLOUD_EXPORT_MODEL_H
