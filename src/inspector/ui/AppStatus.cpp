/************************************************************************
 *                                                                      *
 * Copyright 2020 by Esteban Ruiz de Oña Crespo <estebanrdo@gmail.com>  *
 *                                                                      *
 * This file is part of Computer Vision Studio                          *
 *                                                                      *
 * CV Studio is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * CV Studio is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/


#include "AppStatus.h"


namespace inspector
{

namespace ui
{

std::unique_ptr<AppStatus> AppStatus::sAppStatus;
std::mutex AppStatus::sMutex;

AppStatus::AppStatus()
{
  //ImageManager &image_manager = ImageManager::instance();

  //connect(&image_manager, &ImageManager::imagesLoaded, [=](bool active) {
  //  AppStatus &app_status = AppStatus::instance();
  //  app_status.activeFlag(AppStatus::Flag::image_open, active);
  //});
}

AppStatus::~AppStatus()
{
  sAppStatus.release();
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

void AppStatus::clear()
{
  mFlags.clear();
  emit update();
}

AppStatus &AppStatus::instance()
{
  if (sAppStatus.get() == nullptr) {
    std::lock_guard<std::mutex> lck(AppStatus::sMutex);
    if (sAppStatus.get() == nullptr) {
      sAppStatus.reset(new AppStatus());
    }
  }
  return *sAppStatus;
}


} // namespace ui

} // namespace inspector
