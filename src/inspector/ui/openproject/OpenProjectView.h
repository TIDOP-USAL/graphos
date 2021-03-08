#ifndef INSPECTOR_OPEN_PROJECT_VIEW_INTERFACE_H
#define INSPECTOR_OPEN_PROJECT_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

#include <QFileDialog>

namespace inspector
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

} // namespace inspector

#endif // INSPECTOR_OPEN_PROJECT_VIEW_INTERFACE_H