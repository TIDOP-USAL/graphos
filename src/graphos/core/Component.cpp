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


#include "Component.h"

#include "graphos/core/AppStatus.h"
#include "graphos/core/process/ProcessPresenter.h"

#include <QAction>

namespace graphos
{

ComponentBase::ComponentBase(Application *application)
  : mAction(nullptr),
    mModel(nullptr),
    mView(nullptr),
    mPresenter(nullptr),
    mApplication(application)
{
  ComponentBase::init();
}

ComponentBase::~ComponentBase()
{
  if (mAction) {
    delete mAction;
    mAction = nullptr;
  }

  if (mPresenter) {
    delete mPresenter;
    mPresenter = nullptr;
  }

  if (mModel) {
    delete mModel;
    mModel = nullptr;
  }

  if (mView) {
    delete mView;
    mView = nullptr;
  }

}

void ComponentBase::init()
{
  ComponentBase::createAction();

  connect(mAction, &QAction::triggered,
          this, &ComponentBase::createComponent);
  connect(mApplication->status(), &AppStatus::update,
          this, &ComponentBase::update);
}

void ComponentBase::createComponent()
{
  disconnect(mAction, &QAction::triggered,
             this, &ComponentBase::createComponent);

  this->createModel();
  this->createView();
  this->createPresenter();

  emit created();

  connect(mAction, &QAction::triggered,
          this, &ComponentBase::openComponent);

  openComponent();
}

void ComponentBase::openComponent()
{
  mPresenter->open();
}

void ComponentBase::createAction()
{
  mAction = new QAction();
}

QString ComponentBase::name() const
{
  return mName;
}

QAction *ComponentBase::action() const
{
  return mAction;
}

QString ComponentBase::menu() const
{
  return mMenu;
}

QString ComponentBase::toolbar() const
{
  return mToolbar;
}

void ComponentBase::setName(const QString &name)
{
  mName = name;
  mAction->setText(name);
}

void ComponentBase::setMenu(const QString &menu)
{
  mMenu = menu;
}

void ComponentBase::setToolbar(const QString &toolbar)
{
  mToolbar = toolbar;
}

Model *ComponentBase::model()
{
  return mModel;
}

View *ComponentBase::view()
{
  return mView;
}

Presenter *ComponentBase::presenter()
{
  return mPresenter;
}

void ComponentBase::setModel(Model *model)
{
  mModel = model;
}

void ComponentBase::setView(View *view)
{
  mView = view;
}

void ComponentBase::setPresenter(Presenter *presenter)
{
  mPresenter = presenter;
}

Application *ComponentBase::app()
{
  return mApplication;
}


ProcessComponent::ProcessComponent(Application *application)
  : ComponentBase(application),
    mProgressHandler(nullptr)
{
  connect(this, &Component::created, 
          this, &ProcessComponent::onComponentCreated);
}

void ProcessComponent::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void ProcessComponent::onComponentCreated()
{
  Presenter *presenter = this->presenter();
  connect(dynamic_cast<ProcessPresenter *>(presenter), &ProcessPresenter::running,
          this, &ProcessComponent::onRunning);
  connect(dynamic_cast<ProcessPresenter *>(presenter), &ProcessPresenter::finished,
          this, &ProcessComponent::onFinished);
  connect(dynamic_cast<ProcessPresenter *>(presenter), &ProcessPresenter::failed,
          this, &ProcessComponent::onFailed);

  dynamic_cast<ProcessPresenter *>(presenter)->setProgressHandler(mProgressHandler);
}

void ProcessComponent::onRunning()
{
  app()->status()->activeFlag(AppStatus::Flag::processing, true);
}

void ProcessComponent::onFinished()
{
  app()->status()->activeFlag(AppStatus::Flag::processing, false);
  app()->status()->activeFlag(AppStatus::Flag::project_modified, true);

  emit finished();
}

void ProcessComponent::onFailed()
{
  app()->status()->activeFlag(AppStatus::Flag::processing, false);
}

} // namespace graphos
