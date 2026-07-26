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

#include <tidop/core/base/Property.h>

namespace graphos
{

class FeatureMatching
{
protected:

    tl::Properties mProperties;

public:

    FeatureMatching();
    FeatureMatching(const FeatureMatching &featureMatching) noexcept = default;
    FeatureMatching(FeatureMatching &&featureMatching) noexcept = default;
    virtual ~FeatureMatching() = default;

    auto operator=(const FeatureMatching &featureMatching) -> FeatureMatching & = default;
    auto operator=(FeatureMatching &&featureMatching) noexcept -> FeatureMatching & = default;

    [[nodiscard]]
    auto crossCheck() const -> bool;
    void enableCrossCheck(bool enable);
    [[nodiscard]]
    auto ratio() const -> double;
    void setRatio(double ratio);
    [[nodiscard]]
    auto distance() const -> double;
    void setDistance(double distance);
    [[nodiscard]]
    auto maxError() const -> double;
    void setMaxError(double error);
    [[nodiscard]]
    auto confidence() const -> double;
    void setConfidence(double confidence);

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

private:

    bool mCrossCheck;
    double mRatio;
    double mDistance;
    double mMaxError;
    double mConfidence;

};




} // namespace graphos
