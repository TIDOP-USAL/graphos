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

#include "graphos/core/features/Features.h"
#include "graphos/core/features/Sift.h"

namespace graphos
{


auto Feature::name() const -> std::string 
{
    return mProperties.name(); 
}

void Feature::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}



auto FeatureFactory::create(const std::string &type) -> std::shared_ptr<Feature>
{
    if (type == "SIFT") {
        return std::make_shared<Sift>();
    }

    tl::Message::warning("Unknown Feature type: {}", type);
    return nullptr;
}

} // graphos