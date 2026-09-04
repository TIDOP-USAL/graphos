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

#include "graphos/framework/component/MultiComponentBase.h"
#include "graphos/framework/task/TaskPresenter.h"
#include "graphos/framework/command/Command.h"
#include "graphos/core/AppStatus.h"

#include <tidop/core/base/Meta.h>

#include <QAction>
#include <QMenu>
#include <QMainWindow>

namespace graphos
{


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

auto MultiComponentBase::subMenu() const -> QMenu*
{
    return dynamic_cast<QMenu *>(mView);
}

auto MultiComponentBase::name() const -> QString
{
    return mName;
}

auto MultiComponentBase::action() const -> QAction*
{
    return nullptr;
}

auto MultiComponentBase::menu() const -> QString
{
    return mParentMenu;
}

auto MultiComponentBase::toolbar() const -> QString
{
    return mToolbar;
}

auto MultiComponentBase::widget() const -> QWidget*
{
    return dynamic_cast<QWidget *>(mView);
}

auto MultiComponentBase::command() -> std::shared_ptr<Command>
{
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

auto MultiComponentBase::model() const -> Model*
{
    return mModel;
}

auto MultiComponentBase::view() const -> View*
{
    return mView;
}

auto MultiComponentBase::presenter() const -> Presenter*
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

void MultiComponentBase::setCommand(const std::shared_ptr<Command>& command)
{
    tl::unusedParameter(command);
    //if(command) {
    //  mCommand = command;
    //  mApplication->addComponent(this);
    //}
}

auto MultiComponentBase::app() const -> Application*
{
    return mApplication;
}

} // namespace graphos
