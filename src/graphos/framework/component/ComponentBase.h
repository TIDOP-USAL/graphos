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

#include "graphos/framework/component/Component.h"
#include "graphos/framework/app/Application.h"
#include "graphos/framework/mvp/Model.h"
#include "graphos/framework/mvp/View.h"
#include "graphos/framework/mvp/Presenter.h"

#include <QString>

class QAction;

namespace graphos
{

/*!
 * \brief Base class for components
 *
 * The `ComponentBase` class serves as the base class for components in the application.
 * It inherits from `Component` and `QObject`.
 */
class ComponentBase
  : public Component
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor
     * \param application Pointer to the application instance
     */
    ComponentBase(Application *application);

    /*!
     * \brief Destructor
     */
    ~ComponentBase();

    /*!
     * \brief Create component
     */
    virtual void create();

    /*!
     * \brief Open the component
     */
    virtual void open();


protected:

    /*!
     * \brief Get the model associated with the component
     * \return Pointer to the model associated with the component
     */
    auto model() const -> Model*;

    /*!
     * \brief Get the view associated with the component
     * \return Pointer to the view associated with the component
     */
    auto view() const -> View*;

    /*!
     * \brief Get the presenter associated with the component
     * \return Pointer to the presenter associated with the component
     */
    auto presenter() const -> Presenter*;

    /*!
     * \brief Set the model associated with the component
     * \param model Pointer to the model
     */
    void setModel(Model *model);

    /*!
     * \brief Set the view associated with the component
     * \param view Pointer to the view
     */
    void setView(View *view);

    /*!
     * \brief Set the presenter associated with the component
     * \param presenter Pointer to the presenter
     */
    void setPresenter(Presenter *presenter);

    /*!
     * \brief Set the command associated with the component
     * \param command Shared pointer to the command
     */
    void setCommand(const std::shared_ptr<Command>& command);

    /*!
     * \brief Get the application instance
     * \return Pointer to the application instance
     */
    auto app() const -> Application*;

protected:

    /*!
     * \brief Create action for the component
     */
    virtual void createAction();

    /*!
     * \brief Create model for the component
     */
    virtual void createModel() = 0;

    /*!
     * \brief Create view for the component
     */
    virtual void createView() = 0;

    /*!
     * \brief Create presenter for the component
     */
    virtual void createPresenter() = 0;

    /*!
     * \brief Create command for the component
     */
    virtual void createCommand() = 0;

    /*!
     * \brief Update the component
     */
    virtual void update() = 0;

private:

    /*!
     * \brief Initialize the component
     */
    void init();

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

private:

    Application *mApplication;
    QString mMenu;
    QString mToolbar;
    QString mName;
    QAction *mAction = nullptr;
    Model *mModel = nullptr;
    View *mView = nullptr;
    Presenter *mPresenter = nullptr;
    std::shared_ptr<Command> mCommand;
    bool mDeleteView = true;
};

} // namespace graphos