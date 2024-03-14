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

#include "graphos/core/ColorTable.h"

/* TidopLib */
#include <tidop/math/math.h>

namespace graphos
{


ColorTable::ColorTable(std::string name)
  : mName(std::move(name))
{

}

std::string ColorTable::name() const
{
    return mName;
}

void ColorTable::add(int code,
                     const std::string &name,
                     const tl::Color &color)
{
    mColors[code] = std::make_pair(name, color);
}

auto ColorTable::color(int code) const -> tl::Color
{
    return mColors.at(code).second;
}

void ColorTable::setVisible(int code, bool visible)
{
    mColors.at(code).second.setOpacity(visible ?
                                       static_cast<uint8_t>(255) :
                                       tl::consts::zero<uint8_t>);

    emit change();
}

auto ColorTable::isVisible(int code) const -> bool
{
    return mColors.at(code).second.opacity() == static_cast<uint8_t>(255);
}

auto ColorTable::size() const -> size_t
{
    return mColors.size();
}

auto ColorTable::empty() const -> bool
{
    return mColors.empty();
}

auto ColorTable::begin() -> iterator
{
    return mColors.begin();
}

auto ColorTable::begin() const -> const_iterator
{
    return mColors.begin();
}

auto ColorTable::end() -> iterator
{
    return mColors.end();
}

auto ColorTable::end() const -> const_iterator
{
    return mColors.end();
}

} // namespace graphos
