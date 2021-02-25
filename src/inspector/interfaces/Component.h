#ifndef INSPECTOR_COMPONENT_H
#define INSPECTOR_COMPONENT_H

#include <QObject>
#include <QString>


class QAction;


namespace inspector
{

namespace ui
{

class Component
  : public QObject
{

  Q_OBJECT

public:

  Component() {}
  virtual ~Component() = default;

  virtual QAction *openAction() const = 0;
  virtual QString menu() const = 0;
  virtual QString toolbar() const = 0;

private:

};

class ProcessComponent
  : public Component
{

  Q_OBJECT

public:

  ProcessComponent() {}
  ~ProcessComponent() override = default;

signals:

  void running();
  void finished();
  void failed();

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENT_H
