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

#include "graphos/core/Component.h"
#include "graphos/core/AppStatus.h"
#include "graphos/core/task/TaskPresenter.h"
#include "graphos/core/command.h"

#include <QAction>
#include <QMenu>
#include <QMainWindow>

namespace graphos
{


/* Component base class */


ComponentBase::ComponentBase(Application *application)
  : mAction(nullptr),
    mModel(nullptr),
    mView(nullptr),
    mPresenter(nullptr),
    mApplication(application),
    mDeleteView(true)
{
    init();
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

    if (mDeleteView && mView) {
        delete mView;
        mView = nullptr;
    }

}

void ComponentBase::init()
{

    ComponentBase::createAction();

    connect(mAction, &QAction::triggered,
            this, &ComponentBase::open);
    connect(mApplication->status(), &AppStatus::update,
            this, &ComponentBase::update);

}

void ComponentBase::create()
{
    if (!mPresenter) {
        createModel();
        createView();
        createPresenter();

        emit created();
    }
}

void ComponentBase::open()
{
    create();

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

QWidget *ComponentBase::widget() const
{
    return dynamic_cast<QWidget *>(mView);
}

std::shared_ptr<Command> ComponentBase::command()
{
    if (!mCommand) {
        createCommand();
    }
    return mCommand;
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

void ComponentBase::setIcon(const QIcon &icon)
{
    action()->setIcon(icon);
}

void ComponentBase::freeMemory()
{

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

    mCommand.reset();

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
    if (mView->parent()) {
        mDeleteView = false;
    }
}

void ComponentBase::setPresenter(Presenter *presenter)
{
    mPresenter = presenter;

    connect(mPresenter, &Presenter::help,
            this, &ComponentBase::help);
}

void ComponentBase::setCommand(std::shared_ptr<Command> command)
{
    if (command) {
        mCommand = command;
        mApplication->addComponent(this);
    }
}

Application *ComponentBase::app()
{
    return mApplication;
}




/* Task Component */

TaskComponent::TaskComponent(Application *application)
  : ComponentBase(application),
    mProgressHandler(nullptr)
{
    connect(this, &Component::created,
            this, &TaskComponent::onComponentCreated);
}

void TaskComponent::setProgressHandler(ProgressHandler *progressHandler)
{
    mProgressHandler = progressHandler;
}

void TaskComponent::onComponentCreated()
{
    Presenter *presenter = this->presenter();
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::running,
            this, &TaskComponent::onRunning);
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::finished,
            this, &TaskComponent::onFinished);
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::failed,
            this, &TaskComponent::onFailed);
    connect(dynamic_cast<TaskPresenter *>(presenter), &TaskPresenter::canceled,
            this, &TaskComponent::onCanceled);

    dynamic_cast<TaskPresenter *>(presenter)->setProgressHandler(mProgressHandler);
}

void TaskComponent::onRunning()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, true);
}

void TaskComponent::onFinished()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, false);

    emit finished();
}

void TaskComponent::onFailed()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, false);
}

void TaskComponent::onCanceled()
{
    app()->status()->activeFlag(AppStatus::Flag::processing, false);
}




/* Multi-component base class */

MultiComponentBase::MultiComponentBase(Application *application)
  : mModel(nullptr),
    mView(nullptr),
    mPresenter(nullptr),
    mApplication(application)
{
    init();
}

MultiComponentBase::~MultiComponentBase()
{
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

void MultiComponentBase::init()
{
    connect(mApplication->status(), &AppStatus::update,
            this, &MultiComponentBase::update);
}

QMenu *MultiComponentBase::subMenu() const
{
    return dynamic_cast<QMenu *>(mView);
}

QString MultiComponentBase::name() const
{
    return mName;
}

QAction *MultiComponentBase::action() const
{
    return nullptr;
}

QString MultiComponentBase::menu() const
{
    return mParentMenu;
}

QString MultiComponentBase::toolbar() const
{
    return mToolbar;
}

QWidget *MultiComponentBase::widget() const
{
    return dynamic_cast<QWidget *>(mView);
}

std::shared_ptr<Command> MultiComponentBase::command()
{
    //if(!mCommand) {
    //  this->createCommand();
    //}
    //return mCommand;
    return std::shared_ptr<Command>();
}

void MultiComponentBase::setName(const QString &name)
{
    mName = name;
}

void MultiComponentBase::setMenu(const QString &menu)
{
    mParentMenu = menu;
}

void MultiComponentBase::setToolbar(const QString &toolbar)
{
    mToolbar = toolbar;
}

void MultiComponentBase::setIcon(const QIcon &icon)
{
    action()->setIcon(icon);
}

void MultiComponentBase::freeMemory()
{
}

Model *MultiComponentBase::model()
{
    return mModel;
}

View *MultiComponentBase::view()
{
    return mView;
}

Presenter *MultiComponentBase::presenter()
{
    return mPresenter;
}

void MultiComponentBase::setModel(Model *model)
{
    mModel = model;
}

void MultiComponentBase::setView(View *view)
{
    mView = view;
}

void MultiComponentBase::setPresenter(Presenter *presenter)
{
    mPresenter = presenter;
}

void MultiComponentBase::setCommand(std::shared_ptr<Command> command)
{
    tl::unusedParameter(command);
    //if(command) {
    //  mCommand = command;
    //  mApplication->addComponent(this);
    //}
}

Application *MultiComponentBase::app()
{
    return mApplication;
}

} // namespace graphos
