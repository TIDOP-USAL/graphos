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

#pragma once

#include "graphos/framework/component/MultiComponent.h"
#include "graphos/framework/app/Application.h"
#include "graphos/framework/mvp/Model.h"
#include "graphos/framework/mvp/View.h"
#include "graphos/framework/mvp/Presenter.h"

#include <QString>

class QAction;
class QMenu;

namespace graphos
{

class Command;

class MultiComponentBase
    : public MultiComponent
{

public:

    MultiComponentBase(Application *application);
    ~MultiComponentBase();

protected:

    virtual void createModel() = 0;
    virtual void createView() = 0;
    virtual void createPresenter() = 0;
    virtual void update() = 0;

private:

    void init();

// MultiComponent

public:

    auto subMenu() const -> QMenu* override;

// Component

public:

    auto name() const -> QString override;
    auto action() const -> QAction* override;
    auto menu() const -> QString override;
    auto toolbar() const -> QString override;
    auto widget() const -> QWidget* override;
    auto command() -> std::shared_ptr<Command> override;

    void setName(const QString &name) override;
    void setMenu(const QString &menu) override;
    void setToolbar(const QString &toolbar) override;
    void setIcon(const QIcon &icon) override;

    void freeMemory() override;

protected:

    auto model() const -> Model*;
    auto view() const -> View*;
    auto presenter() const -> Presenter*;
    void setModel(Model *model);
    void setView(View *view);
    void setPresenter(Presenter *presenter);
    void setCommand(const std::shared_ptr<Command>& command);
    auto app() const -> Application*;

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