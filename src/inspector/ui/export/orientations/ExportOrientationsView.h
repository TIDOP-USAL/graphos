#ifndef INSPECTOR_EXPORT_ORIENTATIONS_VIEW_INTERFACE_H
#define INSPECTOR_EXPORT_ORIENTATIONS_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{


class ExportOrientationsView
  : public DialogView
{

  Q_OBJECT

public:

  ExportOrientationsView(QWidget *parent) : DialogView(parent) {}
  ~ExportOrientationsView() override = default;

  virtual void addFormatWidget(QWidget *formatWidget) = 0;
  virtual QString format() const = 0;

public slots:

  virtual void setCurrentFormat(const QString &format) = 0;

signals:

  void formatChange(const QString &);
};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_ORIENTATIONS_VIEW_INTERFACE_H
