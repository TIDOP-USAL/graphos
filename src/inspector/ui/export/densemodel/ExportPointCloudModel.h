#ifndef INSPECTOR_EXPORT_POINT_CLOUD_MODEL_INTERFACE_H
#define INSPECTOR_EXPORT_POINT_CLOUD_MODEL_INTERFACE_H

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

public slots:

  virtual void setCsvFile(const QString &csv) = 0;
  virtual void setDelimiter(const QString &delimiter) = 0;
  virtual void enableColor(bool active) = 0;
  virtual void enableNormals(bool active) = 0;
  virtual void exportPointCloud() = 0;

signals:


};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_EXPORT_POINT_CLOUD_MODEL_H
