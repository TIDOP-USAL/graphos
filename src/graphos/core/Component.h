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
 * \brief Interface for components
 *
 * The `Component` class represents an interface for components in the application.
 * It inherits from `QObject`.
 */
class Component
    : public QObject
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor
     */
    Component() {}

    /*!
     * \brief Destructor
     */
    ~Component() override = default;

    /*!
     * \brief Get the component name
     * \return The component name
     */
    virtual auto name() const -> QString = 0;

    /*!
     * \brief Get the component action
     * \return Pointer to the component's action
     */
    virtual auto action() const -> QAction* = 0;

    /*!
     * \brief Get the menu where the tool is loaded
     * \return The menu where the tool is loaded
     */
    virtual auto menu() const -> QString = 0;

    /*!
     * \brief Get the toolbar where the tool is loaded
     * \return The toolbar where the tool is loaded
     */
    virtual auto toolbar() const -> QString = 0;

    /*!
     * \brief Get the widget associated with the component
     * \return Pointer to the widget associated with the component
     */
    virtual auto widget() const -> QWidget* = 0;

    /*!
     * \brief Get the command associated with the component
     * \return Shared pointer to the command associated with the component
     */
    virtual auto command() -> std::shared_ptr<Command> = 0;

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
     * \brief Set the component icon
     * \param[in] icon Component toolbar
     */
    virtual void setIcon(const QIcon &icon) = 0;

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

    /*!
     * \brief Signal to show help for the component
     */
    void help(QString);
};




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

    /*!
     * \brief Constructor
     * \param application Pointer to the application instance
     */
    TaskComponent(Application *application);

    /*!
     * \brief Destructor
     */
    ~TaskComponent() override = default;

public slots:

    /*!
     * \brief Set the progress handler
     * \param[in] progressHandler Progress handler
     */
    virtual void setProgressHandler(ProgressHandler *progressHandler);

private slots:

    /*!
     * \brief Slot invoked when the component is created
     */
    void onComponentCreated();

protected slots:

    /*!
     * \brief Slot invoked when the task is running
     */
    virtual void onRunning();

    /*!
     * \brief Slot invoked when the task is finished
     */
    virtual void onFinished();

    /*!
     * \brief Slot invoked when the task fails
     */
    virtual void onFailed();

    /*!
     * \brief Slot invoked when the task is canceled
     */
    virtual void onCanceled();

signals:

    /*!
     * \brief Signal emitted when the task is running
     */
    void running();

    /*!
     * \brief Signal emitted when the task is finished
     */
    void finished();

    /*!
     * \brief Signal emitted when the task fails
     */
    void failed();

    /*!
     * \brief Signal emitted when the task is canceled
     */
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

    virtual auto subMenu() const -> QMenu* = 0;

};


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


#endif // GRAPHOS_COMPONENT_H
