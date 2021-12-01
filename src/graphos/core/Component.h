/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/


#ifndef GRAPHOS_COMPONENT_H
#define GRAPHOS_COMPONENT_H

#include "graphos/interfaces/Component.h"
#include "graphos/interfaces/mvp.h"
#include "graphos/core/Application.h"

#include <QString>

class QAction;
class QDialog;

//namespace tl
//{
//class Command;
//}

namespace graphos
{

class ProgressHandler;
class Command;

class ComponentBase
  : public Component
{

  Q_OBJECT

public:

  ComponentBase(Application *application);
  ~ComponentBase();

protected:

  virtual void init();
  virtual void createComponent();
  virtual void openComponent();
  virtual void createAction();
  virtual void createModel() = 0;
  virtual void createView() = 0;
  virtual void createPresenter() = 0;
  virtual void createCommand() = 0;
  virtual void update() = 0;

// Component

public:

  QString name() const override;
  QAction *action() const override;
  QString menu() const override;
  QString toolbar() const override;
  std::shared_ptr<Command> command() override;

  void setName(const QString &name) override;
  void setMenu(const QString &menu) override;
  void setToolbar(const QString &toolbar) override;

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

signals:

  void running();
  void finished();
  void failed();

private:

  ProgressHandler *mProgressHandler;
};

} // namespace graphos


#endif // GRAPHOS_COMPONENT_H
