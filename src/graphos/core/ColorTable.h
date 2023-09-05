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

class ColorTable
    : public QObject
{

    Q_OBJECT

public:

    using iterator = std::map<int, std::pair<std::string, tl::Color>>::iterator;
    using const_iterator = std::map<int, std::pair<std::string, tl::Color>>::const_iterator;

public:

    ColorTable(std::string name);

    std::string name() const;
    void add(int code,
             const std::string &name,
             const tl::Color &color);

    tl::Color color(int code) const;
    void setVisible(int code, bool visible);
    bool isVisible(int code) const;
    //std::string name(int code) const;
    size_t size() const;
    bool empty() const;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

signals:

    void change();

private:

    std::string mName;
    std::map<int, std::pair<std::string, tl::Color>> mColors;
};

} // namespace graphos


#endif // GRAPHOS_CORE_COLOR_TABLE_H
