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

#include "graphos/core/ortho/SbbProperties.h"

namespace graphos
{

SbbProperties::SbbProperties()
  : OrthomosaicProperties("SBB")
{
    clear();
}

auto SbbProperties::exposureCompensator() const -> std::string
{
    return mProperties.getProperty<std::string>("ExposureCompensator");
}

auto SbbProperties::seamFinder() const -> std::string
{
    return mProperties.getProperty<std::string>("SeamFinder");
}

auto SbbProperties::blenderType() const -> std::string
{
    return mProperties.getProperty<std::string>("BlenderType");
}

void SbbProperties::setExposureCompensator(const std::string &exposureCompensator)
{
    // Debería validarse que el valor sea uno de los permitidos: "GAIN", "GAIN_BLOCKS", "NO", "CHANNELS", "CHANNELS_BLOCKS"
    mProperties.setProperty("ExposureCompensator", exposureCompensator);
}

void SbbProperties::setSeamFinder(const std::string &seamFinder)
{
    // Debería validarse que el valor sea uno de los permitidos: "GRAPH_CUT", "VORONOI"
    mProperties.setProperty("SeamFinder", seamFinder);
}

void SbbProperties::setBlenderType(const std::string &blenderType)
{
    // Debería validarse que el valor sea uno de los permitidos: "NO", "FEATHER", "MULTI_BAND"
    mProperties.setProperty("BlenderType", blenderType);
}

void SbbProperties::clear()
{
    setExposureCompensator("GAIN");
    setSeamFinder("GRAPH_CUT");
    setBlenderType("FEATHER");
}


} // namespace graphos
