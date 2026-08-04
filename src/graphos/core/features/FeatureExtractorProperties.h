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

#include <opencv2/features2d.hpp>

#include <tidop/core/base/Property.h>

namespace graphos
{


class FeatureExtractorProperties
{

protected:

    tl::Properties mProperties;

public:

    FeatureExtractorProperties(std::string name) 
      : mProperties(std::move(name)){ }

    virtual ~FeatureExtractorProperties() = default;

    /*!
     * \brief Recover the default values
     */
    virtual void clear() = 0;

    [[nodiscard]]
    auto name() const -> std::string;

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

};


class FeatureFactory
{

public:

    static auto create(const std::string &type) -> std::shared_ptr<FeatureExtractorProperties>;
};


} // namespace graphos