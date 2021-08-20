#ifndef GRAPHOS_COMPONENT_H
#define GRAPHOS_COMPONENT_H

#include "graphos/interfaces/Component.h"

#include <QObject>
#include <QString>


class QAction;
class QDialog;

namespace graphos
{

namespace ui
{

class Model;
class Presenter;
class ProgressHandler;


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
  Model *mModel;
  QDialog *mView;
  Presenter *mPresenter;

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

} // namespace graphos


#endif // GRAPHOS_COMPONENT_H
