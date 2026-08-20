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

#include "graphos/core/dem/DemProperties.h"

namespace graphos
{

DemProperties::DemProperties()
  : mProperties("PoissonRecon")
{
    clear();
}

auto DemProperties::crs() const -> std::string
{
    return mProperties.getProperty<std::string>("Crs");
}

auto DemProperties::gsd() const -> double
{
    return mProperties.getProperty<double>("Gsd");
}

auto DemProperties::dsm() const -> bool
{
    return mProperties.getProperty<bool>("Dsm");
}

auto DemProperties::dtm() const -> bool
{
    return mProperties.getProperty<bool>("Dtm");
}

void DemProperties::setCrs(std::string crs)
{
    mProperties.setProperty("Crs", std::move(crs));
}

void DemProperties::setGsd(double gsd)
{
    mProperties.setProperty("Gsd", gsd);
}

void DemProperties::setDsm(bool dsm)
{
    mProperties.setProperty("Dsm", dsm);
}

void DemProperties::setDtm(bool dtm)
{
    mProperties.setProperty("Dtm", dtm);
}

void DemProperties::clear()
{
    setCrs("");
    setGsd(0.1);
    setDsm(true);
    setDtm(false);
}

void DemProperties::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}

} // namespace graphos
