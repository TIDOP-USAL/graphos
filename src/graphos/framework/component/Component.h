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

#include <QObject>
#include <QString>

class QAction;

namespace graphos
{

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


} // namespace graphos