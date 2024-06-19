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

#ifndef GRAPHOS_CORE_COLOR_TABLE_H
#define GRAPHOS_CORE_COLOR_TABLE_H

#include "graphos/graphos_global.h"

#include <map>

/* Qt */
#include <QObject>

/* TidopLib */
#include <tidop/graphic/color.h>

namespace graphos
{


/*!
 * \brief Color table for CCViewer3D
 *
 * The `ColorTable` class represents a color table for CCViewer3D.
 * It inherits from QObject.
 */
class ColorTable
  : public QObject
{

    Q_OBJECT

public:

    using iterator = std::map<int, std::pair<std::string, tl::Color>>::iterator;
    using const_iterator = std::map<int, std::pair<std::string, tl::Color>>::const_iterator;

public:

    /*!
     * \brief Constructor
     *
     * Constructs a `ColorTable` object with the specified name.
     *
     * \param[in] name The name of the color table
     */
    ColorTable(std::string name);

    /*!
     * \brief Get the name of the color table
     *
     * \return The name of the color table
     */
    auto name() const -> std::string;

    /*!
     * \brief Add a color entry to the table
     *
     * Adds a color entry with the specified code, name, and color.
     *
     * \param[in] code The code associated with the color
     * \param[in] name The name of the color
     * \param[in] color The color value
     */
    void add(int code,
             const std::string &name,
             const tl::Color &color);

    /*!
     * \brief Get the color associated with a code
     *
     * \param code The code associated with the color
     * \return The color associated with the code
     */
    auto color(int code) const -> tl::Color;

    /*!
     * \brief Set the visibility of a color entry
     *
     * Sets the visibility of the color entry with the specified code.
     *
     * \param[in] code The code associated with the color
     * \param[in] visible The visibility status (true for visible, false for hidden)
     */
    void setVisible(int code, bool visible);

    /*!
     * \brief Check if a color entry is visible
     *
     * \param[in] code The code associated with the color
     * \return True if the color entry is visible, otherwise false
     */
    auto isVisible(int code) const -> bool;

    /*!
     * \brief Get the number of color entries in the table
     *
     * \return The number of color entries
     */
    auto size() const -> size_t;

    /*!
     * \brief Check if the color table is empty
     *
     * \return True if the color table is empty, otherwise false
     */
    auto empty() const -> bool;

    /*!
     * \brief Get an iterator to the beginning of the color table
     *
     * \return An iterator to the beginning of the color table
     */
    auto begin() -> iterator;

    /*!
     * \brief Get a const iterator to the beginning of the color table
     *
     * \return A const iterator to the beginning of the color table
     */
    auto begin() const -> const_iterator;

    /*!
     * \brief Get an iterator to the end of the color table
     *
     * \return An iterator to the end of the color table
     */
    auto end() -> iterator;

    /*!
     * \brief Get a const iterator to the end of the color table
     *
     * \return A const iterator to the end of the color table
     */
    auto end() const -> const_iterator;

signals:

    /*!
     * \brief Signal emitted when the color table changes
     */
    void change();

private:

    std::string mName;
    std::map<int, std::pair<std::string, tl::Color>> mColors;
};

} // namespace graphos


#endif // GRAPHOS_CORE_COLOR_TABLE_H
