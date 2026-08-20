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

#pragma once

#include "graphos/graphos_global.h"

#include <string>

#include <tidop/core/base/Property.h>

namespace graphos
{


class DemProperties
{

protected:

    tl::Properties mProperties;

public:

    DemProperties();
    DemProperties(const DemProperties &properties) noexcept = default;
    DemProperties(DemProperties &&properties) noexcept = default;
    virtual ~DemProperties() = default;
    
    auto operator=(const DemProperties &properties) -> DemProperties & = default;
    auto operator=(DemProperties &&properties) noexcept -> DemProperties & = default;

    [[nodiscard]]
    auto crs() const -> std::string;
    [[nodiscard]] 
    auto gsd() const -> double;
    [[nodiscard]] 
    auto dsm() const -> bool;
    [[nodiscard]] 
    auto dtm() const -> bool;

    void setCrs(std::string crs);
    void setGsd(double gsd);
    void setDsm(bool dsm);
    void setDtm(bool dtm);

    /*!
     * \brief Recover the default values
     */
    void clear();

    [[nodiscard]]
    auto name() const -> std::string
    {
        return mProperties.name();
    }

    auto begin() const { return mProperties.begin(); }

    auto end() const { return mProperties.end(); }

    void setProperty(const std::string &key, const std::string &value);

    template<typename T>
    void setProperty(const std::string &key, T value)
    {
        mProperties.setProperty(key, value);
    }

};


} // namespace graphos