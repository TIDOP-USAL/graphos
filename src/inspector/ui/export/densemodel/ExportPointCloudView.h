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

//  virtual void addFormatWidget(QWidget *formatWidget) = 0;
//  virtual QString format() const = 0;
  virtual bool isColorActive() const = 0;
  virtual bool isNormalsActive() const = 0;

public slots:

//  virtual void setCurrentFormat(const QString &format) = 0;
  virtual void setActiveColor(bool active) = 0;
  virtual void setActiveNormals(bool active) = 0;

signals:

//  void formatChange(const QString &);
  void colorChange(bool);
  void normalsChange(bool);

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_EXPORT_POINT_CLOUD_VIEW_INTERFACE_H
