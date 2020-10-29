#ifndef INSPECTOR_EXPORT_POINT_CLOUD_VIEW_INTERFACE_H
#define INSPECTOR_EXPORT_POINT_CLOUD_VIEW_INTERFACE_H


#include "inspector/interfaces/mvp.h"


namespace inspector
{

namespace ui
{

class ExportPointCloudView
  : public IDialogView
{

  Q_OBJECT

public:

  ExportPointCloudView(QWidget *parent) : IDialogView(parent) {}
  ~ExportPointCloudView() override = default;

  virtual QString delimiter() const = 0;

public slots:

  virtual void setDelimiter(const QString &delimiter) = 0;
  

signals:

  void delimiterChanged(QString);
  
};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_EXPORT_POINT_CLOUD_VIEW_INTERFACE_H
