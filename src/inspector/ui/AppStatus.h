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

#ifndef INSPECTOR_APP_STATUS_H
#define INSPECTOR_APP_STATUS_H

#include "inspector/inspector_global.h"

#include <tidop/core/flags.h>

#include <QObject>

#include <memory>
#include <mutex>

namespace inspector
{

namespace ui
{


class AppStatus
  : public QObject
{

  Q_OBJECT

public:

  enum class Flag
  {
    none                  = (0 << 0),
    project_exists        = (1 << 0),  // Existe un proyecto
    project_modified      = (1 << 1),  // Se ha modificado el proyecto
    images_added          = (1 << 2),  // Se han añadido fotogramas
    image_open            = (1 << 3),  // Hay una imagen abierta
    feature_extraction    = (1 << 4),
    feature_matching      = (1 << 5),
    oriented              = (1 << 6),
    absolute_oriented     = (1 << 7),
    dense_model           = (1 << 8),
    dtm                   = (1 << 9),
    processing            = (1 << 20),
    loading_images        = (1 << 21),
    ground_truth          = (1 << 30)
  };

private:

  AppStatus();

public:

  static AppStatus &instance();
  ~AppStatus();

  AppStatus(const AppStatus &) = delete;
  AppStatus(AppStatus &&) = delete;
  AppStatus operator=(const AppStatus &) = delete;
  AppStatus operator=(AppStatus &&) = delete;

  void activeFlag(Flag flag, bool active);
  bool isActive(Flag flag) const;
  void flagOn(Flag flag);
  void flagOff(Flag flag);
  void switchFlag(Flag flag);
  void clear();

signals:

  void update();

private:

  static std::unique_ptr<AppStatus> sAppStatus;
  static std::mutex sMutex;
  tl::EnumFlags<Flag> mFlags;
  
};
ALLOW_BITWISE_FLAG_OPERATIONS(AppStatus::Flag)

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_APP_STATUS_H
