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

#include "graphos/framework/component/ComponentBase.h"
#include "graphos/framework/task/TaskPresenter.h"
#include "graphos/framework/command/Command.h"
#include "graphos/core/AppStatus.h"

//#include <tidop/core/base/Meta.h>

#include <QAction>
#include <QWidget>
//#include <QMenu>
//#include <QMainWindow>

namespace graphos
{


/* Component base class */


ComponentBase::ComponentBase(Application *application)
  : mApplication(application)
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

auto ComponentBase::name() const -> QString
{
    return mName;
}

auto ComponentBase::action() const -> QAction*
{
    return mAction;
}

auto ComponentBase::menu() const -> QString
{
    return mMenu;
}

auto ComponentBase::toolbar() const -> QString
{
    return mToolbar;
}

auto ComponentBase::widget() const -> QWidget*
{
    //return dynamic_cast<QWidget *>(mView);
    return mView->widget();
}

auto ComponentBase::command() -> std::shared_ptr<Command>
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

    if (mView && mDeleteView) {
        delete mView;
        mView = nullptr;
    }

    mCommand.reset();

}

auto ComponentBase::model() const -> Model*
{
    return mModel;
}

auto ComponentBase::view() const -> View*
{
    return mView;
}

auto ComponentBase::presenter() const -> Presenter*
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
    if (mView->widget()->parent()) {
        mDeleteView = false;
    }
}

void ComponentBase::setPresenter(Presenter *presenter)
{
    mPresenter = presenter;

    connect(mPresenter, &Presenter::help,
            this, &ComponentBase::help);
}

void ComponentBase::setCommand(const std::shared_ptr<Command>& command)
{
    if (command) {
        mCommand = command;
        mApplication->addComponent(this);
    }
}

auto ComponentBase::app() const -> Application*
{
    return mApplication;
}

} // namespace graphos
