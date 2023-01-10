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

#include "graphos/core/AppStatus.h"


namespace graphos
{

AppStatus::AppStatus()
{
}

AppStatus::~AppStatus()
{
}

void AppStatus::activeFlag(Flag flag, bool active)
{
  if (mFlags.isActive(flag) != active) {
    mFlags.activeFlag(flag, active);
    emit update();
  }
}

bool AppStatus::isActive(Flag flag) const
{
  return mFlags.isActive(flag);
}

void AppStatus::flagOn(Flag flag)
{
  if (!mFlags.isActive(flag)) {
    mFlags.flagOn(flag);
    emit update();
  }
}

void AppStatus::flagOff(Flag flag)
{
  if (mFlags.isActive(flag)) {
    mFlags.flagOff(flag);
    emit update();
  }
}

void AppStatus::switchFlag(Flag flag)
{
  mFlags.switchFlag(flag);
  emit update();
}

bool AppStatus::isActive(uint32_t flag) const
{
  return mUserFlags.isActive(flag);
}

void AppStatus::flagOn(uint32_t flag)
{
  if (!mUserFlags.isActive(flag)) {
    mUserFlags.flagOn(flag);
    emit update();
  }
}

void AppStatus::flagOff(uint32_t flag)
{
  if (mUserFlags.isActive(flag)) {
    mUserFlags.flagOff(flag);
    emit update();
  }
}

void AppStatus::switchFlag(uint32_t flag)
{
  mUserFlags.switchFlag(flag);
  emit update();
}

void AppStatus::clear()
{
  mFlags.clear();
  mUserFlags.clear();
  emit update();
}

} // namespace graphos
