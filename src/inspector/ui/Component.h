#ifndef INSPECTOR_COMPONENT_H
#define INSPECTOR_COMPONENT_H

#include <QObject>
#include <QString>


class QAction;

namespace inspector
{

namespace ui
{

class IModel;
class IDialogView;
class IPresenter;


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

protected:

  virtual void setName(const QString &name) = 0;
  virtual void setMenu(const QString &menu) = 0;
  virtual void setToolbar(const QString &toolbar) = 0;

protected:

  virtual void update() = 0;
};


class ComponentBase
  : public Component
{

  Q_OBJECT

public:

  ComponentBase();
  ~ComponentBase();

protected:

  virtual void init();
  virtual void createComponent();
  virtual void createAction();
  virtual void createModel() = 0;
  virtual void createView() = 0;
  virtual void createPresenter() = 0;

// Component

public:

  QString name() const override;
  QAction *action() const override;
  QString menu() const override;
  QString toolbar() const override;

protected:

  void setName(const QString &name) override;
  void setMenu(const QString &menu) override;
  void setToolbar(const QString &toolbar) override;

protected:

  QString mName;
  QAction *mAction;
  IModel *mModel;
  IDialogView *mView;
  IPresenter *mPresenter;

private:

  QString mMenu;
  QString mToolbar;

};


class ProcessComponent
  : public ComponentBase
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
