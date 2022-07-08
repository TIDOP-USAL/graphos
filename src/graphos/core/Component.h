/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/


#ifndef GRAPHOS_COMPONENT_H
#define GRAPHOS_COMPONENT_H

#include "graphos/core/mvp.h"
#include "graphos/core/Application.h"

#include <QString>

class QAction;
class QDialog;
class QMenu;

namespace graphos
{

class ProgressHandler;
class Command;

class Component
  : public QObject
{

  Q_OBJECT

public:

  enum class ViewType : uint8_t
  {
    object,
    widget,
    dialog,
    dock_widget,
    menu
  };

public:

  Component() {}
  virtual ~Component() = default;

  virtual QString name() const = 0;
  virtual QAction *action() const = 0;
  virtual QString menu() const = 0;
  virtual QString toolbar() const = 0;
  virtual QWidget *widget() const = 0;
  virtual std::shared_ptr<Command> command() = 0;
  virtual void setName(const QString &name) = 0;
  virtual void setMenu(const QString &menu) = 0;
  virtual void setToolbar(const QString &toolbar) = 0;

  virtual void freeMemory() = 0;

signals:

  void created();
};


class ComponentBase
  : public Component
{

  Q_OBJECT

public:

  ComponentBase(Application *application);
  ~ComponentBase();

  virtual void createComponent();

protected:

  virtual void openComponent();
  virtual void createAction();
  virtual void createModel() = 0;
  virtual void createView() = 0;
  virtual void createPresenter() = 0;
  virtual void createCommand() = 0;
  virtual void update() = 0;

private:

  void init();

// Component

public:

  QString name() const override;
  QAction *action() const override;
  QString menu() const override;
  QString toolbar() const override;
  QWidget *widget() const override;
  std::shared_ptr<Command> command() override;

  void setName(const QString &name) override;
  void setMenu(const QString &menu) override;
  void setToolbar(const QString &toolbar) override;

  void freeMemory() override;

protected:

  Model *model();
  View *view();
  Presenter *presenter();
  ViewType viewType() const;
  void setModel(Model *model);
  void setView(View *view);
  void setPresenter(Presenter *presenter);
  void setCommand(std::shared_ptr<Command> command);
  void setViewType(ViewType viewType);
  Application *app();

private:

  QString mMenu;
  QString mToolbar;
  QString mName;
  QAction *mAction;
  Model *mModel;
  View *mView;
  Presenter *mPresenter;
  std::shared_ptr<Command> mCommand;
  Application *mApplication;
  ViewType mViewType;

};



class ProcessComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  ProcessComponent(Application *application);
  ~ProcessComponent() override = default;

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler);

private slots:

  void onComponentCreated();

protected slots:

  virtual void onRunning();
  virtual void onFinished();
  virtual void onFailed();
  virtual void onCanceled();

signals:

  void running();
  void finished();
  void failed();
  void canceled();

private:

  ProgressHandler *mProgressHandler;
};



class MultiComponent
  : public Component
{

  Q_OBJECT

public:

  MultiComponent() = default;
  virtual ~MultiComponent() = default;

  virtual QMenu *subMenu() const = 0;

};


class MultiComponentBase
  : public MultiComponent
{

public:

  MultiComponentBase(Application *application);
  ~MultiComponentBase();

protected:

  //virtual void createComponent();
  virtual void createModel() = 0;
  virtual void createView() = 0;
  virtual void createPresenter() = 0;
  //virtual void createCommand() = 0;
  virtual void update() = 0;

private:

  void init();

// MultiComponent

public:

  QMenu *subMenu() const override;

// Component

public:

  QString name() const override;
  QAction *action() const override;
  QString menu() const override;
  QString toolbar() const override;
  QWidget *widget() const override;
  std::shared_ptr<Command> command() override;

  void setName(const QString &name) override;
  void setMenu(const QString &menu) override;
  void setToolbar(const QString &toolbar) override;

  void freeMemory() override;

protected:

  Model *model();
  View *view();
  Presenter *presenter();
  void setModel(Model *model);
  void setView(View *view);
  void setPresenter(Presenter *presenter);
  void setCommand(std::shared_ptr<Command> command);
  Application *app();

private:

  QString mParentMenu;
  QString mToolbar;
  QString mName;
  Model *mModel;
  View *mView;
  Presenter *mPresenter;
  Application *mApplication;

};


} // namespace graphos


#endif // GRAPHOS_COMPONENT_H
