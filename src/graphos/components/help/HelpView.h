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

#include "graphos/framework/mvp/DialogView.h"

namespace graphos
{

/*!
 * \brief View interface for help component
 *
 * This class represents the view responsible for displaying help information.
 */
class HelpView
  : public DialogView
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor for HelpView
     * \param parent The parent widget
     */
    HelpView(QWidget *parent)
      : DialogView(parent, Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint) {}

    /*!
     * \brief Destructor for HelpView
     */
    ~HelpView() override = default;

public slots:

    /*!
     * \brief Navigates to the home page
     */
    virtual void navigateHome() = 0;

    /*!
     * \brief Navigates backward
     */
    virtual void navigateBackward() = 0;

    /*!
     * \brief Navigates forward
     */
    virtual void navigateForward() = 0;

    /*!
     * \brief Sets the base URL for help pages
     * \param urlBase The base URL
     */
    virtual void setUrlBase(const QString &urlBase) = 0;

    /*!
     * \brief Sets the URL for the home page
     * \param[in] homePage The URL for the home page
     */
    virtual void setHomePage(const QString &homePage) = 0;

    /*!
     * \brief Sets the URL for the help page
     * \param[in] helpPage The URL for the help page
     */
    virtual void setHelpPage(const QString &helpPage) = 0;

    virtual void searchInvoked() = 0;
    virtual void searchFinished(int hits) = 0;

};

} // namespace graphos
