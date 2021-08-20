#ifndef GRAPHOS_OPEN_PROJECT_VIEW_INTERFACE_H
#define GRAPHOS_OPEN_PROJECT_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

#include <QFileDialog>

namespace graphos
{

namespace ui
{

class OpenProjectView
  : public QFileDialog
{
  Q_OBJECT

public:

  OpenProjectView(QWidget *parent) : QFileDialog(parent) {}
  ~OpenProjectView() override = default;

  virtual void setInspectorPath(const QString &directory) = 0;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_OPEN_PROJECT_VIEW_INTERFACE_H