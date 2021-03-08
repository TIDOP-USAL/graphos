#ifndef INSPECTOR_OPEN_PROJECT_MODEL_INTERFACE_H
#define INSPECTOR_OPEN_PROJECT_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{


class OpenProjectModel
  : public IModel
{

  Q_OBJECT

public:

  OpenProjectModel(QObject *parent = nullptr) : IModel(parent) {}
  ~OpenProjectModel() override = default;

  virtual QString inspectorDirectory() const = 0;

public slots:

  virtual void saveProject() = 0;
  virtual void loadProject(const QString &projectPath) = 0;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_OPEN_PROJECT_MODEL_INTERFACE_H
