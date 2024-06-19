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
 
#ifndef GRAPHOS_ABOUT_MODEL_INTERFACE_H
#define GRAPHOS_ABOUT_MODEL_INTERFACE_H

#include <tidop/core/defs.h>
#include "graphos/core/mvp.h"

namespace tl
{
class Licence;
}

namespace graphos


{
/*!
 * \brief About model
 *
 * The `AboutModel` class represents the Model interface of the "About" component.
 * It inherits from `Model` and provides methods to access licensing information and iterate through licenses.
 */
class AboutModel
  : public Model
{

    Q_OBJECT

public:

    /*!
     * \brief Constant iterator type
     *
     * Defines a type alias for the constant iterator to iterate through licenses.
     */
    using const_iterator = std::list<tl::Licence>::const_iterator;

public:

    /*!
     * \brief Constructor
     *
     * Constructs an `AboutModel` object with the specified parent.
     *
     * \param[in] parent Pointer to the parent object (optional).
     */
    AboutModel(QObject *parent = nullptr) : Model(parent) {}

    ~AboutModel() override = default;

    /*!
     * \brief Get the license information
     *
     * Returns a reference to the license information.
     *
     * \return Reference to the license information.
     */
    virtual auto licence() const -> const tl::Licence& = 0;

    /*!
     * \brief Read a license
     *
     * Reads and returns the contents of the specified license file.
     *
     * \param[in] licence The filename of the license to read.
     * \return Contents of the license file as a string.
     */
    virtual auto readLicence(const QString& licence) -> QString = 0;

    /*!
     * \brief Get the beginning iterator
     *
     * Returns a constant iterator pointing to the beginning of the license list.
     *
     * \return Constant iterator pointing to the beginning of the license list.
     */
    virtual auto begin() const -> const_iterator = 0;

    /*!
     * \brief Get the ending iterator
     *
     * Returns a constant iterator pointing to the end of the license list.
     *
     * \return Constant iterator pointing to the end of the license list.
     */
    virtual auto end() const -> const_iterator = 0;

};

} // namespace graphos

#endif // GRAPHOS_ABOUT_MODEL_INTERFACE_H
