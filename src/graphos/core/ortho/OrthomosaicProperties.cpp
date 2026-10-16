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

#include "graphos/core/ortho/OrthomosaicProperties.h"
#include "graphos/core/ortho/RlbProperties.h"
#include "graphos/core/ortho/SbbProperties.h"

namespace graphos
{

auto OrthomosaicPropertiesFactory::create(const std::string &type) -> std::shared_ptr<OrthomosaicProperties>
{
    if (tl::compareInsensitiveCase(type, "RLB")) {
        return std::make_shared<RlbProperties>();
    } else if (tl::compareInsensitiveCase(type, "SBB")) {
        return std::make_shared<SbbProperties>();
    }

    tl::Message::warning("Unknown Orthomosaic method type: {}", type);
    return nullptr;
}

} // namespace graphos
