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


/*!
 * \brief Component interface
 */
class Component
  : public QObject
{

  Q_OBJECT

public:

  Component() {}
  virtual ~Component() = default;

  /*!
   * \brief Component name
   */
  virtual QString name() const = 0;

  /*!
   * \brief Component action.
   * Action that opens the tool.
   */
  virtual QAction *action() const = 0;

  /*!
   * \brief Menu where the tool is loaded.
   */
  virtual QString menu() const = 0;

  /*!
   * \brief Toolbar where the tool is loaded.
   */
  virtual QString toolbar() const = 0;

  /*!
   * \brief Widget associated to the component.
   */
  virtual QWidget *widget() const = 0;

  /*!
   * \brief Component associated command.
   */
  virtual std::shared_ptr<Command> command() = 0;

  /*!
   * \brief Set the component name
   * \param[in] name Component name
   */
  virtual void setName(const QString &name) = 0;

  /*!
   * \brief Set the component menu
   * \param[in] menu Component menu
   */
  virtual void setMenu(const QString &menu) = 0;

  /*!
   * \brief Set the component toolbar
   * \param[in] toolbar Component toolbar
   */
  virtual void setToolbar(const QString &toolbar) = 0;

  /*!
   * \brief Free memory
   * Release the component
   */
  virtual void freeMemory() = 0;

signals:

  /*!
   * \brief Signal emitted when the component is created
   */
  void created();
};


/*!
 * \brief Component base class
 */
class ComponentBase
  : public Component
{

  Q_OBJECT

public:

  ComponentBase(Application *application);
  ~ComponentBase();

  /*!
   * \brief Create component
   */
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
  void setModel(Model *model);
  void setView(View *view);
  void setPresenter(Presenter *presenter);
  void setCommand(std::shared_ptr<Command> command);
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
  bool mDeleteView;
};


/*!
 * \brief Component that executes a task
 */
class TaskComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  TaskComponent(Application *application);
  ~TaskComponent() override = default;

public slots:

  /*!
   * \brief Set the progress handler
   * \param[in] progressHandler Progress handler
   */
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
