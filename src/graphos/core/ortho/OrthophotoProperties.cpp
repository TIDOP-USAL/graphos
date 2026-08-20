/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#include "graphos/core/ortho/OrthophotoProperties.h"

namespace graphos
{

OrthophotoProperties::OrthophotoProperties()
  : mProperties("Orthophoto")
{
    clear();
}

auto OrthophotoProperties::crs() const -> std::string
{
    return mProperties.getProperty<std::string>("Crs");
}

auto OrthophotoProperties::gsd() const -> double
{
    return mProperties.getProperty<double>("Gsd");
}

auto OrthophotoProperties::interpolation() const -> std::string
{
    return mProperties.getProperty<std::string>("Interpolation");
}

void OrthophotoProperties::setCrs(std::string crs)
{
    mProperties.setProperty("Crs", std::move(crs));
}

void OrthophotoProperties::setGsd(double gsd)
{
    mProperties.setProperty("Gsd", gsd);
}

void OrthophotoProperties::setInterpolation(std::string interpolation)
{
    mProperties.setProperty("Interpolation", std::move(interpolation));
}

void OrthophotoProperties::clear()
{
    mProperties.setProperty("Crs", std::string());
    mProperties.setProperty("Gsd", 0.0);
    mProperties.setProperty("Interpolation", std::string("BILINEAR"));
}

auto OrthophotoProperties::name() const -> std::string
{
    return mProperties.name();
}

void OrthophotoProperties::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}

} // namespace graphos
