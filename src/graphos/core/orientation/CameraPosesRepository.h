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

#include "graphos/graphos_global.h"

#include <string>
#include <unordered_map>

#include "graphos/core/base/Repository.h"
#include "graphos/core/orientation/CameraPose.h"

namespace graphos
{

class CameraPosesRepository
  : public Repository<size_t, CameraPose>
{

//private:
//
//    std::unordered_map<size_t, CameraPose> mData;

public:

    CameraPosesRepository() = default;

    void add(size_t index, CameraPose pose);

    void populate(std::unordered_map<size_t, CameraPose> poses)
    {
        mData = std::move(poses);
    }

    //[[nodiscard]] 
    //auto toUnorderedMap() const noexcept -> const std::unordered_map<size_t, CameraPose>&
    //{
    //    return mData;
    //}
};

} // end namespace graphos
