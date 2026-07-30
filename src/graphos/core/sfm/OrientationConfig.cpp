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

#include "graphos/core/sfm/OrientationConfig.h"

namespace graphos
{




OrientationConfig::OrientationConfig(std::string method)
    : mProperties(std::move(method))
{
    clear();
}

auto OrientationConfig::name() const -> std::string
{
    return mProperties.name();
}

auto OrientationConfig::fixCalibration() const -> bool
{
    return mProperties.getProperty<bool>("FixCalibration");
}

void OrientationConfig::enableFixCalibration(bool enable)
{
    mProperties.setProperty("FixCalibration", enable);
}

auto OrientationConfig::useGcp() const -> bool
{
    return mProperties.getProperty<bool>("UseGcp");
}

void OrientationConfig::enableUseGcp(bool enable)
{
    mProperties.setProperty("UseGcp", enable);
}

auto OrientationConfig::usePoses() const -> bool
{
    return mProperties.getProperty<bool>("UsePoses");
}

void OrientationConfig::enableUsePoses(bool enable)
{
    mProperties.setProperty("UsePoses", enable);
}

auto OrientationConfig::useRtkAccuracy() const -> bool
{
    return mProperties.getProperty<bool>("UseRtkAccuracy");
}

void OrientationConfig::enableUseRtkAccuracy(bool enable)
{
    mProperties.setProperty("UseRtkAccuracy", enable);
}

auto OrientationConfig::absoluteOrientation() const -> bool
{
    return mProperties.getProperty<bool>("AbsoluteOrientation");
}

void OrientationConfig::enableAbsoluteOrientation(bool enable)
{
    mProperties.setProperty("AbsoluteOrientation", enable);
}

void OrientationConfig::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}

void OrientationConfig::clear()
{
    enableAbsoluteOrientation(false);
    enableFixCalibration(false);
    enableUseGcp(false);
    enableUsePoses(false);
    enableUseRtkAccuracy(false);
}

} // namespace graphos

