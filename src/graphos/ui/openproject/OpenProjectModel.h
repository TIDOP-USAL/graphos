#ifndef GRAPHOS_OPEN_PROJECT_MODEL_INTERFACE_H
#define GRAPHOS_OPEN_PROJECT_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{


class OpenProjectModel
  : public Model
{

  Q_OBJECT

public:

  OpenProjectModel(QObject *parent = nullptr) : Model(parent) {}
  ~OpenProjectModel() override = default;

  virtual QString inspectorDirectory() const = 0;

public slots:

  virtual void saveProject() = 0;
  virtual void loadProject(const QString &projectPath) = 0;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_OPEN_PROJECT_MODEL_INTERFACE_H
