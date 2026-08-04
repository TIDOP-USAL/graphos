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

#include "graphos/core/orientation/OrientationProperties.h"

namespace graphos
{


OrientationProperties::OrientationProperties(std::string method)
    : mProperties(std::move(method))
{
    clear();
}

auto OrientationProperties::name() const -> std::string
{
    return mProperties.name();
}

auto OrientationProperties::fixCalibration() const -> bool
{
    return mProperties.getProperty<bool>("FixCalibration");
}

void OrientationProperties::enableFixCalibration(bool enable)
{
    mProperties.setProperty("FixCalibration", enable);
}

auto OrientationProperties::useGcp() const -> bool
{
    return mProperties.getProperty<bool>("UseGcp");
}

void OrientationProperties::enableUseGcp(bool enable)
{
    mProperties.setProperty("UseGcp", enable);
}

auto OrientationProperties::usePoses() const -> bool
{
    return mProperties.getProperty<bool>("UsePoses");
}

void OrientationProperties::enableUsePoses(bool enable)
{
    mProperties.setProperty("UsePoses", enable);
}

auto OrientationProperties::useRtkAccuracy() const -> bool
{
    return mProperties.getProperty<bool>("UseRtkAccuracy");
}

void OrientationProperties::enableUseRtkAccuracy(bool enable)
{
    mProperties.setProperty("UseRtkAccuracy", enable);
}

auto OrientationProperties::absoluteOrientation() const -> bool
{
    return mProperties.getProperty<bool>("AbsoluteOrientation");
}

void OrientationProperties::enableAbsoluteOrientation(bool enable)
{
    mProperties.setProperty("AbsoluteOrientation", enable);
}

void OrientationProperties::setProperty(const std::string &key, const std::string &value)
{
    mProperties.setProperty(key, value);
}

void OrientationProperties::clear()
{
    enableAbsoluteOrientation(false);
    enableFixCalibration(false);
    enableUseGcp(false);
    enableUsePoses(false);
    enableUseRtkAccuracy(false);
}

} // namespace graphos

