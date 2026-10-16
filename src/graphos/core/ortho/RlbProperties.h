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

#pragma once

#include "config_graphos.h"

#include "graphos/core/ortho/OrthomosaicProperties.h"

namespace graphos
{


class RlbProperties
  : public OrthomosaicProperties
{

public:

    RlbProperties();
    RlbProperties(const RlbProperties &properties) = default;
    RlbProperties(RlbProperties &&properties) noexcept = default;
    ~RlbProperties() override = default;

    auto operator =(const RlbProperties& properties) -> RlbProperties& = default;
    auto operator =(RlbProperties&& properties) noexcept -> RlbProperties& = default;

// OrthomosaicProperties

public:

    void clear() override;
};

} // namespace graphos
