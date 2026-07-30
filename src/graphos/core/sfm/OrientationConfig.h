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

#include <tidop/core/base/Property.h>

namespace graphos
{

class OrientationConfig
{

private:

    tl::Properties mProperties;

public:

    OrientationConfig(std::string method);

    virtual ~OrientationConfig() = default;

    [[nodiscard]]
    auto name() const -> std::string;

    [[nodiscard]]
    auto fixCalibration() const -> bool;

    void enableFixCalibration(bool enable);

    [[nodiscard]]
    auto useGcp() const -> bool;

    void enableUseGcp(bool enable);

    [[nodiscard]]
    auto usePoses() const -> bool;

    void enableUsePoses(bool enable);

    [[nodiscard]]
    auto useRtkAccuracy() const -> bool;

    void enableUseRtkAccuracy(bool enable);

    [[nodiscard]]
    auto absoluteOrientation() const -> bool;

    void enableAbsoluteOrientation(bool enable);

    auto begin() const
    {
        return mProperties.begin();
    }

    auto end() const
    {
        return mProperties.end();
    }

    void setProperty(const std::string &key, const std::string &value);

    template<typename T>
    void setProperty(const std::string &key, T value)
    {
        mProperties.setProperty(key, value);
    }

    /*!
     * \brief Recover the default values
     */
    void clear();
};

} // namespace graphos

