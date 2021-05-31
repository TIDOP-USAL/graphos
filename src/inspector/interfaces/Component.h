#ifndef INSPECTOR_COMPONENT_INTERFACE_H
#define INSPECTOR_COMPONENT_INTERFACE_H

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

  virtual QString name() const = 0;
  virtual QAction *action() const = 0;
  virtual QString menu() const = 0;
  virtual QString toolbar() const = 0;

  virtual void setName(const QString &name) = 0;
  virtual void setMenu(const QString &menu) = 0;
  virtual void setToolbar(const QString &toolbar) = 0;

signals:

  void created();
};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENT_INTERFACE_H
