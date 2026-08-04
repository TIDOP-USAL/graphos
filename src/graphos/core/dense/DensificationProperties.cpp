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

#include "graphos/core/dense/DensificationProperties.h"
#include "graphos/core/dense/CmvsPmvsProperties.h"
#include "graphos/core/dense/MvsProperties.h"
#include "graphos/core/dense/SmvsProperties.h"

namespace graphos
{

auto DensificationProperties::name() const -> std::string
{
    return mProperties.name();
}

void DensificationProperties::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}




auto DensificationPropertiesFactory::create(const std::string &type) -> std::shared_ptr<DensificationProperties>
{
    if (type == "mvs") {
        return std::make_shared<MvsProperties>();
    } else if (type == "pmvs") {
        return std::make_shared<CmvsPmvsProperties>();
    } else if (type == "smvs") {
        return std::make_shared<SmvsProperties>();
    }

    tl::Message::warning("Unknown FeatureExtractor type: {}", type);
    return nullptr;
}


}
