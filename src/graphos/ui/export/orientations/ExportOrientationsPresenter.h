#ifndef GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_INTERFACE_H
#define GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
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

} // namespace graphos

#endif // GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_INTERFACE_H
