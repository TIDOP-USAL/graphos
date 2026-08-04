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

#include <unordered_map>
#include <cstddef>

namespace graphos
{

template <typename Key, 
          typename Value,
          typename Container = std::unordered_map<Key, Value>>
class Repository 
{

public:

    Repository() = default;
    ~Repository() = default;

    [[nodiscard]] 
    auto contains(const Key &key) const -> bool
    {
        return mData.contains(key);
    }

    [[nodiscard]] 
    auto find(const Key &key) -> Value *
    {
        auto it = mData.find(key);
        return it != mData.end() ? &it->second : nullptr;
    }

    [[nodiscard]] 
    auto find(const Key& key) const -> const Value *
    {
        auto it = mData.find(key);
        return it != mData.end() ? &it->second : nullptr;
    }

    [[nodiscard]] 
    auto size() const noexcept -> size_t
    {
        return mData.size(); 
    }

    [[nodiscard]] 
    auto empty() const noexcept -> bool
    {
        return mData.empty(); 
    }

    auto update(const Key &key, Value &&newValue) -> bool
    {
        auto it = mData.find(key);
        if (it == mData.end()) {
            return false;
        }
        it->second = std::forward<Value>(newValue);
        return true;
    }

    auto begin() noexcept { return mData.begin(); }
    auto end() noexcept { return mData.end(); }
    auto begin() const noexcept { return mData.begin(); }
    auto end() const noexcept { return mData.end(); }
    auto cbegin() const noexcept { return mData.cbegin(); }
    auto cend() const noexcept { return mData.cend(); }

    void clear() 
    { 
        mData.clear(); 
    }

    auto erase(const Key &key) -> bool
    { 
        return mData.erase(key) > 0; 
    }

    [[nodiscard]] 
    auto all() const noexcept -> const Container &
    {
        return mData; 
    }

protected:

    Container mData;
};

} // end namespace graphos
