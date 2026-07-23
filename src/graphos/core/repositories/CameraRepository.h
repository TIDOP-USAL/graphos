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
#include <map>
#include <unordered_map>
#include <optional>

#include "graphos/core/camera/Camera.h"
#include "graphos/core/repositories/Repository.h"

namespace graphos
{

class CameraRepository
  : public Repository<int, Camera, std::map<int, Camera>>
{

private:

    int mNextId{1};
    std::unordered_map<std::string, int> mSignatureIndex;

public:

    CameraRepository() = default;

    auto add(Camera camera) -> int;

    auto update(int id, const Camera &camera) -> bool
    {
        auto *existing = find(id);
        if (!existing) return false;

        std::string oldSig = existing->signature();
        std::string newSig = camera.signature();

        if (oldSig != newSig) {
            mSignatureIndex.erase(oldSig);
            mSignatureIndex[newSig] = id;
        }

        *existing = camera;
        return true;
    }

    [[nodiscard]]
    auto findIdBySignature(std::string_view make,
                           std::string_view model,
                           std::string_view serialNumber = "",
                           std::string_view bandName = "RGB") const -> std::optional<int>
    {
        std::string sig = std::string(make) + "|" + std::string(model) + "|" +
                          std::string(serialNumber) + "|" + std::string(bandName);

        if (auto it = mSignatureIndex.find(sig); it != mSignatureIndex.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    auto erase(const int &id) -> bool
    {
        if (auto it = mData.find(id); it != mData.end()) {
            mSignatureIndex.erase(it->second.signature());
            mData.erase(it);
            return true;
        }
        return false;
    }

    void clear()
    {
        Repository::clear();
        mSignatureIndex.clear();
        mNextId = 1;
    }
};

} // end namespace graphos
