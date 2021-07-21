#ifndef INSPECTOR_EXPORT_ORIENTATIONS_PRESENTER_INTERFACE_H
#define INSPECTOR_EXPORT_ORIENTATIONS_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{


class ExportOrientationsPresenter
  : public Presenter
{

  Q_OBJECT

public:

  ExportOrientationsPresenter(){}
  ~ExportOrientationsPresenter() override = default;

public slots:

  virtual void save() = 0;
  virtual void setCurrentFormat(const QString &format) = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_ORIENTATIONS_PRESENTER_INTERFACE_H
