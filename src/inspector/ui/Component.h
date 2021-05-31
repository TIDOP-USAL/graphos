#ifndef INSPECTOR_COMPONENT_H
#define INSPECTOR_COMPONENT_H

#include "inspector/interfaces/Component.h"

#include <QObject>
#include <QString>


class QAction;
class QDialog;

namespace inspector
{

namespace ui
{

class IModel;
class IPresenter;
class ProgressHandler;

//class Component
//  : public QObject
//{
//
//  Q_OBJECT
//
//public:
//
//  Component() {}
//  virtual ~Component() = default;
//
//  virtual QString name() const = 0;
//  virtual QAction *action() const = 0;
//  virtual QString menu() const = 0;
//  virtual QString toolbar() const = 0;
//
//protected:
//
//  virtual void setName(const QString &name) = 0;
//  virtual void setMenu(const QString &menu) = 0;
//  virtual void setToolbar(const QString &toolbar) = 0;
//
//protected:
//
//  virtual void update() = 0;
//
//signals:
//
//  void created();
//};


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

  void setName(const QString &name) override;
  void setMenu(const QString &menu) override;
  void setToolbar(const QString &toolbar) override;

protected:

  virtual void update() = 0;

protected:

  QString mName;
  QAction *mAction;
  IModel *mModel;
  QDialog *mView;
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

  ProcessComponent();
  ~ProcessComponent() override = default;

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler);

private slots:

  void onComponentCreated();

protected slots:

  virtual void onRunning();
  virtual void onFinished();
  virtual void onFailed();

//signals:
//
//  void running(QString);
//  void finished(QString);
//  void failed(QString);

private:

  ProgressHandler *mProgressHandler;
};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENT_H
