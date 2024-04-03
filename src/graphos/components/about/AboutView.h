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

#ifndef GRAPHOS_ABOUT_VIEW_INTERFACE_H
#define GRAPHOS_ABOUT_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"

namespace graphos
{

/*!
* \brief About view
*
* The `AboutView` class represents the View interface of the "About" component.
* It inherits from `DialogView` and provides methods to display information about the application.
*/
class AboutView
  : public DialogView
{
    Q_OBJECT

public:

    /*!
     * \brief Constructor
     *
     * Constructs an `AboutView` object with the specified parent and window flags.
     *
     * \param[in] parent Pointer to the parent widget.
     */
    explicit AboutView(QWidget *parent) : DialogView(parent) {}

    ~AboutView() override = default;

    /*!
     * \brief Set the version information
     *
     * Sets the version information to be displayed in the view.
     *
     * \param[in] version The version string.
     */
    virtual void setVersion(const QString &version) = 0;

    /*!
     * \brief Set the license information
     *
     * Sets the application license to be displayed in the view.
     *
     * \param[in] licence The license text.
     */
    virtual void setLicence(const QString &licence) = 0;

    /*!
     * \brief Add license information for a third party product
     *
     * Adds license information for a specific product to be displayed in the view.
     *
     * \param[in] product The name of the product.
     * \param[in] licence The license text.
     */
    virtual void addLicence(const QString &product, const QString &licence) = 0;
};

} // namespace graphos

#endif // GRAPHOS_ABOUT_VIEW_INTERFACE_H
